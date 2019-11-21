#include "essentials.h"

#define SHIP_VEL 8
//#define ONE_UP 250000U


static void reset(GameData *, Mix_Music *);

int handler(GameData *data, WinRend *winrend, Assets *assets) {
        int quit = 0;

        if(data->gamestate == IN_GAME && data->lives && (data->player_status == ALIVE || data->player_status == DEAD)) {
                if(data->bullet_timeout) --data->bullet_timeout;

                register int vel = SHIP_VEL;

                const Uint8 *state = SDL_GetKeyboardState(NULL);
                if(state[data->keys[SLOW_K]]) vel /= 2;
                if(state[data->keys[SHOOT_K]] && !data->bullet_timeout) {
                        create_gb(data);
                        if(!data->muted) play_sound(assets->sounds.sfx[SND_LASER], data->ship.x + data->ship.w, SND_LASER);
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
                                //Mix_PlayMusic(assets->sounds.pause_music, -1);
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
                                SDL_SetWindowFullscreen(winrend->win, data->fullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
                                data->fullscreen = !data->fullscreen;
                        } else if(pressed == data->keys[RESET_K]) reset(data, assets->sounds.main_music);
                        else if((pressed == SDLK_RETURN && data->gamestate == IN_GAME) && ((data->level == 7 && data->boss.status == DEAD) || !data->lives)) {
                                Mix_PlayMusic(assets->sounds.pause_music, -1);
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

                if(data->gamestate == NAME_ENTRY) name_handler(data, winrend->rend, assets->font, &event, assets->textures.leaderboard_texts);
                else if(data->gamestate == LEADERBOARD) {
                        if(pressed == SDLK_RETURN) reset(data, assets->sounds.main_music);
                } else if(data->gamestate >= MENU) quit |= menu_handler(data, assets->sounds.sfx, &event);
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
                                Mix_PlayMusic(assets->sounds.main_music, -1);
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
