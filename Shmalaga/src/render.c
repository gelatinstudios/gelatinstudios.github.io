#include "essentials.h"

static inline void render_player(GameData *, SDL_Renderer *, Textures *);
static inline void render_enemy(Enemy *, Sparkle *, TTF_Font *, SDL_Renderer *, Textures *);
static inline void render_boss(Boss *, SDL_Renderer *, SDL_Texture *[]);
static inline void render_scores(GameData *, SDL_Renderer *, TTF_Font *);

const SDL_Rect muted_rect = {10, 720-30-10, 250, 30};
const SDL_Rect lives_rect = {1280-100-10, 720-20-50, 100, 20};
const SDL_Rect dead_rect = {(1280 - 600) / 2, 390-100+10, 600, 100};
const SDL_Rect pressr_rect = {(1280 - 310) / 2, 400, 310, 50};
//const SDL_Rect pause_rect = {640-250+10, 390-(390/2)-100, 500, 100};
const SDL_Rect press_enter_rect = {(1280 - 900) / 2, (720 - 100) / 2, 900, 100};
const SDL_Rect one_up_rect = {10, 10+25+10, 50, 20};
const SDL_Rect stage_rect = {(1280 - 400) / 2, (720 - 55) / 2, 400, 55};
const SDL_Rect boss_battle_rect = {640-250+10, 390-(390/2)-100, 500, 100};
const SDL_Rect win_rect = {(1280 - 700) / 2, (720 - 100) / 2, 700, 100};

void render(GameData *data, SDL_Renderer *rend, Assets *assets) {
        //music                               VVVthis is for the menuVVV
        // if(data->gamestate == IN_GAME || (data->gamestate == SOUND && data->selected <= 1)) {
        //         if(!data->muted && Mix_PlayingMusic() && Mix_PausedMusic()) Mix_ResumeMusic();
        //         if(data->muted && !Mix_PausedMusic()) Mix_PauseMusic();
        // } else Mix_PauseMusic();

        if(!(data->secret & 0x01)) SDL_RenderClear(rend);

        //stars
        for(size_t i = 0; i < MAX_STARS; ++i) {
                if(!is_on_screen(data->stars[i].rect)) continue;
                SDL_SetRenderDrawColor(rend, data->stars[i].color.r, data->stars[i].color.g, data->stars[i].color.b, 255);
                SDL_RenderFillRect(rend, &data->stars[i].rect);
        }

        SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);

        if(data->gamestate == STARTING_SCREEN) {
                SDL_RenderCopy(rend, assets->textures.texts[TXT_START_SCREEN], NULL, &press_enter_rect);
                SDL_RenderPresent(rend);
                return;
        } else if(data->gamestate == NAME_ENTRY) {
                render_name(data, rend, assets->textures.texts, assets->font);
                return;
        } else if(data->gamestate == LEADERBOARD) {
                render_leaderboard(data->leaderboard, rend, assets->textures.texts, assets->font, data->player_score_index);
                return;
        }

        render_player(data, rend, &assets->textures);

        for(size_t i = 0; i < data->enemy_count; ++i)
                render_enemy(&data->enemies[i], NULL, assets->font, rend, &assets->textures);
        render_enemy(&data->gold_enemy, &data->sparkle, assets->font, rend, &assets->textures);

        render_boss(&data->boss, rend, assets->textures.sprites);


        //bullets
        for(size_t i = 0; i < data->gb_count; ++i)
                SDL_RenderCopy(rend, assets->textures.sprites[SPR_GB], NULL, &MK_GB_RECT(data->gb[i]));
        for(size_t i = 0; i < data->bb_count; ++i)
                SDL_RenderCopyEx(rend, assets->textures.sprites[SPR_BB], NULL, &MK_BB_RECT(data->bb[i].point), data->bb[i].angle, NULL, SDL_FLIP_NONE);

        render_scores(data, rend, assets->font);

        //"STAGE N"
        if(data->gamestate == IN_GAME && data->level_timeout) {
                if(data->secret & 0x01) SDL_RenderClear(rend);
                char stage[10] = {0};
                sprintf(stage, "STAGE %u", data->level);
                SDL_Surface *surf = TTF_RenderText_Blended(assets->font, stage, white);
                render_surf(rend, surf, &stage_rect);
                --data->level_timeout;

                if(data->level == 7 && data->level_timeout % 10 - 3 > 0) {
                        SDL_RenderCopy(rend, assets->textures.texts[TXT_BOSS], NULL, &boss_battle_rect);
                }
        }

        //"sound muted"
        if(data->muted) SDL_RenderCopy(rend, assets->textures.texts[TXT_MUTED], NULL, &muted_rect);

        //lives
        SDL_RenderCopy(rend, assets->textures.texts[TXT_LIVES], NULL, &lives_rect);
        for(int i = 0; i < data->lives - 1; ++i) {
                const SDL_Rect life = {1280-30*(i+1)-10, 720-30-10, 30, 30};
                SDL_RenderCopy(rend, assets->textures.sprites[SPR_SHIP], NULL, &life);
        }

        //"YOU'RE DEAD"
        if(data->player_status == DEAD && !data->lives && data->gamestate < MENU) {
                SDL_RenderCopy(rend, assets->textures.texts[TXT_YOU_DIED], NULL, &dead_rect);
                SDL_RenderCopy(rend, assets->textures.texts[TXT_PRESSR], NULL, &pressr_rect);
        }

        //"YOU WIN"
        if(data->level == 7 && data->boss.status == DEAD) {
                SDL_RenderCopy(rend, assets->textures.texts[TXT_YOU_WIN], NULL, &win_rect);
                SDL_RenderCopy(rend, assets->textures.texts[TXT_PRESSR], NULL, &pressr_rect);
        }

        switch(data->gamestate) {
                case IN_GAME:
                                SDL_RenderPresent(rend);
                                break;
                // case PAUSED:
                //                 //if(!Mix_PausedMusic()) Mix_PauseMusic();
                //                 SDL_RenderCopy(rend, assets->textures.texts[TXT_PAUSED], NULL, &pause_rect);
                //                 SDL_RenderPresent(rend);
                //                 break;
                case MENU:
                case CONTROLS:
                case KEYSET_MODE:
                case SOUND:
                                render_menu(data, rend, assets->font, assets->textures.menu);
                                break;
                default:
                                fprintf(stderr, "gamestate error. fatal.\nterminating.\n");
                                exit(EXIT_FAILURE);
                                break;
        }
}

static inline void render_player(GameData *data, SDL_Renderer *rend, Textures *texts) {
                if(data->player_status == ALIVE)
                        SDL_RenderCopy(rend, texts->sprites[SPR_SHIP], NULL, &data->ship);
                else if(data->player_status != DEAD)
                        SDL_RenderCopy(rend, texts->sprites[SPR_EXPLOSION], &explosion_clips[data->player_status-1], &data->ship);
                else if(data->player_status == DEAD && data->lives && !rng(4))
                        SDL_RenderCopy(rend, texts->sprites[SPR_SHIP], NULL, &data->ship);
}

static inline void render_enemy(Enemy *enemy, Sparkle *sparkle, TTF_Font *font, SDL_Renderer *rend, Textures *texts) {
        if(is_on_screen(enemy->rect)) {
                if(enemy->status == ALIVE) {
                        SDL_RenderCopyEx(rend, texts->sprites[enemy->texture], NULL, &enemy->rect, enemy->angle, NULL, SDL_FLIP_NONE);

                        if(enemy->texture == SPR_GOLD_ENEMY) {
                                if(!sparkle->timeout) {
                                        sparkle->rect.x = rng(enemy->rect.w) + enemy->rect.x;
                                        sparkle->rect.y = rng(enemy->rect.h) + enemy->rect.y;
                                        sparkle->rect.w = sparkle->rect.h = (rng(2) ? 0 : 10);
                                        sparkle->timeout = SPARKLE_TIMEOUT;
                                } else --sparkle->timeout;
                                SDL_RenderCopy(rend, texts->sparkle, NULL, &sparkle->rect);
                        }
                } else if(enemy->status != DEAD) {
                        SDL_RenderCopy(rend, texts->sprites[SPR_EXPLOSION], &explosion_clips[enemy->status-1], &enemy->rect);

                        char points[5] = {0};
                        if(enemy->texture == SPR_GOLD_ENEMY) sprintf(points, "%u", 2*ENEM_P);
                        else  sprintf(points, "%u", ENEM_P / (unsigned) pow(2, enemy->passes));
                        SDL_Surface *surf = TTF_RenderText_Blended(font, points, gold);
                        const int width = 10 * strlen(points);
                        SDL_Rect points_rect = {enemy->rect.x + (enemy->rect.w - width) / 2, enemy->rect.y - 12, width, 10};
                        render_surf(rend, surf, &points_rect);
                }
        }
}

static inline void render_boss(Boss *boss, SDL_Renderer *rend, SDL_Texture *sprites[]) {
        if(boss->status == ALIVE) {
                if(boss->damage_timeout) {
                        SDL_SetTextureColorMod(sprites[SPR_BOSS], 255, 0, 0);
                        SDL_RenderCopy(rend, sprites[SPR_BOSS], &boss_clips[3 - (4 * (boss->hp-1) / BOSS_HP)], &boss->rect);
                        SDL_SetTextureColorMod(sprites[SPR_BOSS], 255, 255, 255);
                        --boss->damage_timeout;
                }
                else SDL_RenderCopy(rend, sprites[SPR_BOSS], &boss_clips[3 - (4 * (boss->hp-1) / BOSS_HP)], &boss->rect);
        } else if(boss->status != DEAD) {
                if(boss->new_exp_timeout || boss->explosions[0].timeout) {
                        SDL_RenderCopy(rend, sprites[SPR_BOSS], &boss_clips[3], &boss->rect);
                        for(size_t i = 0; i < NUM_EXP; ++i) {
                                if(boss->explosions[i].timeout)
                                        SDL_RenderCopy(rend, sprites[SPR_EXPLOSION], &explosion_clips[3 - (boss->explosions[i].timeout / EXPLOSION_TIMEOUT)], &boss->explosions[i].rect);
                        }
                } else {
                        SDL_RenderCopy(rend, sprites[SPR_BOSS], &boss_clips[3], &boss->rect);
                        SDL_RenderCopy(rend, sprites[SPR_EXPLOSION], &explosion_clips[boss->status-1], &boss->rect);
                }
        }
}

static inline void render_scores(GameData *data, SDL_Renderer *rend, TTF_Font *font) {
        char score[50] = {0};
        char highscore[50] = {0};
        sprintf(score, "SCORE: %u", data->score.val);
        sprintf(highscore, "HIGHSCORE: %u", data->leaderboard[0].val);

        SDL_Surface *score_surf = TTF_RenderText_Blended(font, score, white);
        SDL_Surface *highscore_surf = TTF_RenderText_Blended(font, highscore, white);

        const SDL_Rect score_rect = {10, 10, 200, 25};
        const SDL_Rect highscore_rect = {1000, 10, 270, 25};

        render_surf(rend, score_surf, &score_rect);
        render_surf(rend, highscore_surf, &highscore_rect);

        if(data->one_up_timeout) {
                SDL_Surface *one_up_surf = TTF_RenderText_Blended(font, "1UP", gold);
                render_surf(rend, one_up_surf, &one_up_rect);
                --data->one_up_timeout;
        }
}
