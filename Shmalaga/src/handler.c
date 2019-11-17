#include "essentials.h"

#define SHIP_VEL 8
//#define ONE_UP 250000U
#define ONE_UP 100000U

static inline int input_handler(GameData *, SDL_Window *, Sounds *);
static inline void action_handler(GameData *, Sounds *);
static inline void enemy_handler(GameData *, Enemy *, Mix_Chunk *);
static inline void player_death(GameData *, Mix_Chunk *);
static void reset(GameData *, Mix_Music *);

int handler(GameData *data, SDL_Window *win, Sounds *sounds) {
        int quit = input_handler(data, win, sounds);
        action_handler(data, sounds);

        return quit;
}

static inline int input_handler(GameData *data, SDL_Window *win, Sounds *sounds) {
        int quit = 0;

        if(data->gamestate == IN_GAME && data->lives && (data->player_status == ALIVE || data->player_status == DEAD)) {
                if(data->bullet_timeout) --data->bullet_timeout;

                register int vel = SHIP_VEL;

                const Uint8 *state = SDL_GetKeyboardState(NULL);
                if(state[data->keys[SLOW_K]]) vel /= 2;
                if(state[data->keys[SHOOT_K]] && !data->bullet_timeout) {
                        create_gb(data);
                        if(!data->muted) play_sound(sounds->sfx[SND_LASER], data->ship.x + data->ship.w, SND_LASER);
                }
                if(state[data->keys[LEFT_K]] && data->ship.x > 0) data->ship.x -= vel;
                if(state[data->keys[RIGHT_K]] && data->ship.x < 1280 - data->ship.w) data->ship.x += vel;
                if(state[data->keys[UP_K]] && data->ship.y > 0) data->ship.y -= vel;
                if(state[data->keys[DOWN_K]] && data->ship.y < 720 - data->ship.h) data->ship.y += vel;
        }

        SDL_Event event;
        while(SDL_PollEvent(&event)) {
                if(event.type == SDL_WINDOWEVENT_CLOSE || event.type == SDL_QUIT) quit |= 1;
                else if(event.type != SDL_KEYDOWN) continue;

                const SDL_Keycode pressed = event.key.keysym.sym;

                if(data->gamestate != NAME_ENTRY) {
                        if(pressed == data->keys[MENU_K] && data->gamestate < MENU) {
                                data->gamestate = MENU;
                                Mix_PauseMusic();
                                //Mix_PlayMusic(sounds->pause_music, -1);
                                break;
                        //}
                        // else if(pressed == data->keys[PAUSE_K]) {
                        //         if(data->gamestate == IN_GAME) {
                        //                 data->gamestate = PAUSED;
                        //                 Mix_PauseMusic();
                        //         } else if(data->gamestate == PAUSED) {
                        //                 data->gamestate = IN_GAME;
                        //                 Mix_ResumeMusic();
                        //         }
                        } else if(pressed == data->keys[MUTE_K]) {
                                data->muted = !data->muted;
                                if(data->muted) Mix_PauseMusic();
                                else Mix_ResumeMusic();
                        } else if(pressed == data->keys[FSCREEN_K]) {
                                //data->gamestate = PAUSED;
                                SDL_SetWindowFullscreen(win, data->fullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
                                data->fullscreen = !data->fullscreen;
                        } else if(pressed == data->keys[RESET_K]) reset(data, sounds->main_music);
                        else if((pressed == SDLK_RETURN && data->gamestate == IN_GAME) && ((data->level == 7 && data->boss.status == DEAD) || !data->lives)) {
                                Mix_PlayMusic(sounds->pause_music, -1);
                                if(data->muted) Mix_PauseMusic();
                                if(data->score.val > data->leaderboard[9].val) {
                                        data->gamestate = NAME_ENTRY;
                                        break;
                                } else {
                                        data->gamestate = LEADERBOARD;
                                        break;
                                }
                        }
                }

                if(data->gamestate == NAME_ENTRY) name_handler(data, &event);
                else if(data->gamestate == LEADERBOARD) {
                        if(pressed == SDLK_RETURN) reset(data, sounds->main_music);
                } else if(data->gamestate >= MENU) quit |= menu_handler(data, sounds->sfx, &event);
                else if(data->gamestate == STARTING_SCREEN) {
                        if(data->selected == 10) data->secret |= data->secret >> 4;

                        if(data->selected) {
                                if(data->selected < 10 && pressed == data->sekritptr[data->selected])
                                        ++data->selected;
                                else {
                                        data->secret &= 0x0F;
                                        data->selected = 0;
                                        data->sekritptr = NULL;
                                }
                        } else if(pressed == SDLK_RETURN) {
                                data->gamestate = IN_GAME;
                                Mix_PlayMusic(sounds->main_music, -1);
                                data->selected = 0;
                                if(data->secret & 0x04) {
                                        data->ship.w = 12;
                                        data->ship.h = 12;
                                } else if(data->secret & 0x02) {
                                        data->ship.w = 100;
                                        data->ship.h = 100;
                                }
                                if(data->secret & 0x08) data->lives = 0xFF;
                        } else {
                                for(size_t i = 0; i < LNGTH(secret_codes); ++i) {
                                        if(pressed != secret_codes[i][0]) continue;

                                        data->sekritptr = secret_codes[i];
                                        data->secret |= (1 << i) << 4;
                                        data->selected = 1;
                                        break;
                                }
                        }
                }
        }


        return quit;
}

static inline void action_handler(GameData *data, Sounds *sounds) {
        if(data->gamestate != IN_GAME) return;

        //all enemies dead
        if(!data->living_enemies && data->level <= 7) {
                ++data->level;
                load_level(data);

                if(data->level == 7) {
                        Mix_PlayMusic(sounds->boss_music, -1);
                        if(data->muted) Mix_PauseMusic();
                }
        }

        //player death sprite update
        if(data->player_status > ALIVE && data->player_status < DEAD) {
                if(!data->player_death_timeout) {
                        ++data->player_status;
                        if(data->player_status == DEAD && data->lives) {
                                 --data->lives;
                                 data->respawn_timeout = RESPAWN_TIMEOUT;
                        }
                        data->player_death_timeout = EXPLOSION_TIMEOUT;
                } else --data->player_death_timeout;
        }

        for(size_t i = 0; i < data->enemy_count; ++i)
                enemy_handler(data, &data->enemies[i], sounds->sfx[SND_EXP]);

        enemy_handler(data, &data->gold_enemy, sounds->sfx[SND_EXP]);

        //enemy bullet hits player
        for(size_t i = 0; i < data->bb_count; ++i)
                if(data->player_status == ALIVE && collision_detect(data->ship, MK_BB_RECT(data->bb[i].point)))
                        player_death(data, sounds->sfx[SND_EXP]);

        //boss
        for(size_t i = 0; i < data->gb_count; ++i) {
                if(data->boss.status == ALIVE && collision_detect(MK_GB_RECT(data->gb[i]), data->boss.rect)) {
                        --data->boss.hp;
                        data->score.val += 5;
                        data->boss.damage_timeout = DAMAGE_TIMEOUT;
                        destroy_gb(data, i);
                }
        }
        if(data->player_status == ALIVE && data->boss.status == ALIVE && collision_detect(data->ship, data->boss.rect))
                player_death(data, sounds->sfx[SND_EXP]);
        if(data->boss.status == ALIVE && !data->boss.hp) {
                data->boss.status = DYING_1;
                //data->boss.explosion_timeout = BOSS_EXPLOSION_TIMEOUT;
                data->boss.new_exp_timeout = NEW_EXP_TIMEOUT;
                //if(!data->muted) Mix_PlayChannel(-1, sounds->explosion, 0);
                data->score.won = 1;
        }
        if(data->boss.status > ALIVE && data->boss.status < DEAD) {
                if(data->boss.new_exp_timeout) {
                        if(!(data->boss.new_exp_timeout % NEW_EXP_SCALE)) {
                                const size_t i = (data->boss.new_exp_timeout / NEW_EXP_SCALE) - 1;
                                data->boss.explosions[i].rect.x = rng(data->boss.rect.w - data->boss.explosions[i].rect.w) + data->boss.rect.x;
                                data->boss.explosions[i].rect.y = rng(data->boss.rect.h - data->boss.explosions[i].rect.h) + data->boss.rect.y;
                                data->boss.explosions[i].timeout = EXPLOSION_TIMEOUT * 4;
                        }
                        --data->boss.new_exp_timeout;
                        if(!data->boss.new_exp_timeout) data->boss.explosion_timeout = BOSS_EXPLOSION_TIMEOUT;
                } else if(data->boss.explosion_timeout) {
                        --data->boss.explosion_timeout;
                } else {
                        ++data->boss.status;
                        data->boss.explosion_timeout = BOSS_EXPLOSION_TIMEOUT;
                }

                for(size_t i = 0; i < NUM_EXP; ++i) {
                        if(data->boss.explosions[i].timeout) --data->boss.explosions[i].timeout;
                }
        }

        //respawn
        if(data->player_status == DEAD && data->lives) {
                if(!data->respawn_timeout) data->player_status = ALIVE;
                else --data->respawn_timeout;
        }

        //1UP
        if(data->score.val >= ONE_UP * (data->life_milestone + 1)) {
                ++data->lives;
                ++data->life_milestone;
                data->one_up_timeout = ONE_UP_TIMEOUT;
                if(!data->muted) Mix_PlayChannel(SND_CHRD+2, sounds->sfx[SND_CHRD], 0);
        }

        //delete off-screen bullets
        if(data->gb[0].x >= 1280 && data->gb_count) destroy_gb(data, 0);
}

static inline void player_death(GameData *data, Mix_Chunk *explosion) {
        data->player_status = DYING_1;
        data->player_death_timeout = EXPLOSION_TIMEOUT;
        if(!data->muted) play_sound(explosion, data->ship.x + data->ship.w/2, SND_EXP);
}

static inline void enemy_handler(GameData *data, Enemy *enemy, Mix_Chunk *explosion) {
        if(data->player_status == ALIVE && enemy->status == ALIVE && collision_detect(data->ship, enemy->rect))
                player_death(data, explosion);

        for(size_t i = 0; i < data->gb_count; ++i) {
                if(enemy->status == ALIVE && collision_detect(MK_GB_RECT(data->gb[i]), enemy->rect)) {
                        enemy->status = DYING_1;
                        enemy->explosion_timeout = EXPLOSION_TIMEOUT;
                        destroy_gb(data, i);
                        if(!data->muted) play_sound(explosion, enemy->rect.x + enemy->rect.w/2, SND_EXP);
                        if(enemy->texture == SPR_GOLD_ENEMY) data->score.val += 2*ENEM_P;
                        else data->score.val += ENEM_P / pow(2, data->enemies[i].passes);
                }
        }
        if(data->gamestate == IN_GAME && enemy->status > ALIVE && enemy->status < DEAD) {
                if(!enemy->explosion_timeout) {
                        ++enemy->status;
                        if(enemy->status == DEAD && enemy->texture != SPR_GOLD_ENEMY) --data->living_enemies;
                        enemy->explosion_timeout = EXPLOSION_TIMEOUT;
                } else --enemy->explosion_timeout;
        }
}

static void reset(GameData *data, Mix_Music *music) {
        data->gamestate = IN_GAME;

        data->level = 1;

        data->score.val = 0;
        data->score.won = 0;
        data->player_score_index = -1;
        data->lives = 4;
        data->life_milestone = 0;

        data->ship.x = 1280/4;
        data->ship.y = 720/2 - 56/2;
        data->player_status = ALIVE;

        while(data->gb_count) destroy_gb(data, 0);

        for(size_t i = 0; i < MAX_ENEMIES; ++i)
                data->enemies[i].angle = 0;
        data->gold_enemy.status = DEAD;

        while(data->bb_count) destroy_bb(data, 0);

        data->boss.rect.w = 500;
        data->boss.rect.h = 500;
        data->boss.status = DEAD;
        data->boss.hp = BOSS_HP;
        data->boss.pattern_timeout = 60 * 5;
        data->boss.angles[0] = 30;
        data->boss.angles[1] = 0;
        data->boss.angles[2] = 330;
        data->boss.angles[3] = 15;
        data->boss.angles[4] = 345;

        Mix_PlayMusic(music, -1);
        if(data->muted) Mix_PauseMusic();

        load_level(data);
}
