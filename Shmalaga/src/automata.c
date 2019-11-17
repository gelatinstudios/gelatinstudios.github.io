#include "essentials.h"

#define STAR_VEL 8
#define ENEMY_VEL 6
#define SQRT3 1.732050807568877

static inline void move_enemies(GameData *, Sounds *);
static inline void lvl3(Enemy *, size_t);
static inline void lvl4(Enemy *, size_t);
static inline void mov_circ(Enemy *);
static inline void mov_circ_ccwise(Enemy *);
static inline void lvl5(Enemy *, const SDL_Rect *);
static inline void lvl6(Enemy *, const SDL_Rect *, int, size_t);
static inline void lvl6_enemy_move(Enemy *, const SDL_Rect *, int);
static inline void boss_battle(Boss *, GameData *, Mix_Chunk *);

void automata(GameData *data, Sounds *sounds) {
        if(data->gamestate == IN_GAME || data->gamestate == NAME_ENTRY || data->gamestate == LEADERBOARD) {
                for(size_t i = 0; i < MAX_STARS; ++i) {
                        data->stars[i].rect.x -= STAR_VEL;
                        if(data->stars[i].rect.x < 0) data->stars[i].rect.x = MAX_STAR_POS;
                }
        }
        if(data->gamestate == IN_GAME) {

                //move player if exploding
                // if(data->player_status != ALIVE && data->player_status != DEAD)
                //         data->ship.x -= ENEMY_VEL;

                if(data->level < 7) move_enemies(data, sounds);
                else boss_battle(&data->boss, data, sounds->sfx[SND_BADLASER]);

                for(size_t i = 0; i < data->gb_count; ++i)
                        data->gb[i].x += BULLET_VEL;

                for(size_t i = 0; i < data->bb_count; ++i) {
                        data->bb[i].point.x -= rint(data->bb[i].dx);
                        data->bb[i].point.y -= rint(data->bb[i].dy);
                }

                while(data->bb_count && !is_on_screen(MK_BB_RECT(data->bb[0].point)))
                        destroy_bb(data, 0);
        }
}

static inline void move_enemies(GameData *data, Sounds *sounds) {
        for(size_t i = 0; i < data->enemy_count; ++i) {
                if(data->enemies[i].status == DEAD) continue;
                else if(data->enemies[i].status > ALIVE && data->enemies[i].status < DEAD) {
                        data->enemies[i].rect.x -= ENEMY_VEL;
                        continue;
                }

                switch(data->level) {
                        case 1:
                        case 2:         data->enemies[i].rect.x -= ENEMY_VEL;
                                        if(data->enemies[i].rect.x < 0 - data->enemies[i].rect.w) {
                                                data->enemies[i].rect.x = 1280 * (data->level == 1 ? 2 : 6);
                                                ++data->enemies[i].passes;
                                        }
                                        break;
                        case 3:         lvl3(&data->enemies[i], i); break;
                        case 4:         lvl4(&data->enemies[i], i); break;
                        case 5:         lvl5(&data->enemies[i], &data->ship); break;
                        case 6:         lvl6(&data->enemies[i], &data->ship, data->player_status == ALIVE, i); break;
                }
                if(is_on_screen(data->enemies[i].rect) && !rng(800)) {
                        create_bb(data, &data->enemies[i].rect, data->enemies[i].angle);
                        if (!data->muted) play_sound(sounds->sfx[SND_BADLASER], rint(data->enemies[i].rect.x + data->enemies[i].rect.w/2.0 * (1.0 - cos(DEGRAD(data->enemies[i].angle))) - 12.0/2.0), SND_BADLASER);
                }
        }

        if(data->gold_enemy.status == ALIVE) {
                data->gold_enemy.rect.x -= 1.5 * ENEMY_VEL;
                data->sparkle.rect.x -= 1.5 * ENEMY_VEL;
        }
}

static inline void lvl3(Enemy *enemy, size_t i) {
        if(i < 75) {
                if(enemy->rect.y > 720) {
                        enemy->rect.x += (720+60)/SQRT3;
                        enemy->rect.y = -24*200 + 50;
                        ++enemy->passes;
                }

                if(enemy->rect.y < -60) enemy->rect.y += ENEMY_VEL;
                else {
                        enemy->rect.x -= ENEMY_VEL/2;
                        enemy->rect.y += SQRT3*ENEMY_VEL/2.0;
                }
        } else {
                if(enemy->rect.y < -60) {
                        enemy->rect.x += (720+60)/SQRT3;
                        enemy->rect.y = 1280*2-60 + 24*200 - 200 + 50;
                        ++enemy->passes;
                }

                if(enemy->rect.y > 720) enemy->rect.y -= ENEMY_VEL;
                else {
                        enemy->rect.x -= ENEMY_VEL/2;
                        enemy->rect.y -= SQRT3*ENEMY_VEL/2.0;
                }
        }
}

static inline void lvl4(Enemy *enemy, size_t i) {
        if(i < 75) {
                if(!enemy->circle && enemy->rect.y > enemy->rect.h && (!rng(75) || enemy->rect.y > 460))
                        enemy->circle = 1;

                if(enemy->rect.y < 0 - enemy->rect.h) enemy->rect.y += ENEMY_VEL;
                else if(!enemy->circle) {
                        enemy->rect.x += ENEMY_VEL/2;
                        enemy->rect.y += SQRT3*ENEMY_VEL/2.0;
                } else mov_circ(enemy);
        } else {
                if(!enemy->circle && enemy->rect.y < 720-60-60 && (!rng(75) || enemy->rect.y < 120))
                        enemy->circle = 1;

                if(enemy->rect.y > 720) enemy->rect.y -= ENEMY_VEL;
                else if(!enemy->circle) {
                        enemy->rect.x += ENEMY_VEL/2;
                        enemy->rect.y -= SQRT3*ENEMY_VEL/2.0;
                } else mov_circ_ccwise(enemy);
        }
}

static inline void mov_circ(Enemy *enemy) {
        enemy->rect.x -= rint(ENEMY_VEL * cos(DEGRAD(enemy->angle)));
        enemy->rect.y -= rint(ENEMY_VEL * sin(DEGRAD(enemy->angle)));
        enemy->angle = ((int) enemy->angle + 3) % 360;
        if(enemy->angle == 240) ++enemy->passes;
        assert(enemy->angle <= 360);
}

static inline void mov_circ_ccwise(Enemy *enemy) {
        enemy->rect.x -= rint(ENEMY_VEL * cos(DEGRAD(enemy->angle)));
        enemy->rect.y -= rint(ENEMY_VEL * sin(DEGRAD(enemy->angle)));
        enemy->angle = ((int) enemy->angle - 3 + 360) % 360;
        if(enemy->angle == 120) ++enemy->passes;
        assert(enemy->angle >= 0);
}

static inline void lvl5(Enemy *enemy, const SDL_Rect *ship) {
        if(enemy->rect.x < 0 - enemy->rect.w) {
                enemy->rect.x = 1280*7;
                ++enemy->passes;
        }
        if(enemy->rect.x > 1000) enemy->rect.x -= ENEMY_VEL;
        else {
                enemy->rect.x -= rint(2.0*ENEMY_VEL * cos(DEGRAD(enemy->angle)));
                enemy->rect.y -= rint(2.0*ENEMY_VEL * sin(DEGRAD(enemy->angle)));
                enemy->angle = RADDEG(atan( ((double) (enemy->rect.y - ship->y)) / ((double) abs(enemy->rect.x - ship->x))) );
        }
}

static inline void lvl6(Enemy *enemy, const SDL_Rect *ship, int status, size_t i) {
        if(i < 50) {
                if(enemy->rect.y > 720 || enemy->rect.x < 0 - enemy->rect.w) {
                        enemy->rect.x = 200 + rng(1280-200);
                        enemy->rect.y = -49 * 300;
                        enemy->angle = 270;
                        ++enemy->passes;
                }

                if(enemy->rect.y < 0) enemy->rect.y += ENEMY_VEL;
                else lvl6_enemy_move(enemy, ship, status);
        } else if(i < 100) {
                if(enemy->rect.x < 0 - enemy->rect.w) {
                        enemy->rect.x = 69 * 300;
                        enemy->rect.y = rng(720);
                        enemy->angle = 0;
                        ++enemy->passes;
                }

                if(enemy->rect.x > 1280 - enemy->rect.w) enemy->rect.x -= ENEMY_VEL;
                else lvl6_enemy_move(enemy, ship, status);
        } else {
                if(enemy->rect.y < 0 || enemy->rect.x < 0 - enemy->rect.w) {
                        enemy->rect.x = 200 + rng(1280-200);
                        enemy->rect.y = 500 + 49 * 300;
                        enemy->angle = 90;
                        ++enemy->passes;
                }

                if(enemy->rect.y > 720 - enemy->rect.h) enemy->rect.y -= ENEMY_VEL;
                else lvl6_enemy_move(enemy, ship, status);
        }
}

static inline void lvl6_enemy_move(Enemy *enemy, const SDL_Rect *ship, int status) {
        enemy->rect.x -= rint(ENEMY_VEL * cos(DEGRAD(enemy->angle)));
        enemy->rect.y -= rint(ENEMY_VEL * sin(DEGRAD(enemy->angle)));
        if(status) enemy->angle = RADDEG( atan(((double) (enemy->rect.y - ship->y)) / ((double) abs(enemy->rect.x - ship->x))) );
}

static inline void boss_battle(Boss *boss, GameData *data, Mix_Chunk *laser) {
        assert(data->level == 7);
        if(boss->status != ALIVE) return;
        if(boss->rect.x > 2*1280/3) {
                boss->rect.x -= ENEMY_VEL;
                return;
        }

        if(!boss->bullet_timeout) {
                switch(boss->pattern) {
                        case 3:
                        case 1:         if(!boss->change && boss->angles[0] == 60) boss->change = 1;
                                        else if(boss->change && boss->angles[2] == 300) boss->change = 0;

                                        for(size_t i = 0; i < (boss->pattern == 1 ? 3 : 5); ++i) {
                                                if(!boss->change) boss->angles[i] = (boss->angles[i] + 5) % 360;
                                                else boss->angles[i] = (boss->angles[i] - 5 + 360) % 360;
                                        } break;

                        case 4:
                        case 2:         if(!boss->change && boss->angles[0] == 300) boss->change = 1;
                                        else if(boss->change && boss->angles[0] == 60) boss->change = 0;

                                        if(!boss->change) {
                                                boss->angles[0] = (boss->angles[0] - 5 + 360) % 360;
                                                boss->angles[2] = (boss->angles[2] + 5) % 360;
                                        } else {
                                                boss->angles[0] = (boss->angles[0] + 5) % 360;
                                                boss->angles[2] = (boss->angles[2] - 5 + 360) % 360;
                                        } break;
                }
                create_bb(data, &boss->rect, boss->angles[0]);
                create_bb(data, &boss->rect, boss->angles[1]);
                create_bb(data, &boss->rect, boss->angles[2]);
                if(boss->pattern >= 3) {
                        create_bb(data, &boss->rect, boss->angles[3]);
                        create_bb(data, &boss->rect, boss->angles[4]);
                }
                if(!data->muted) play_sound(laser, boss->rect.x, SND_BADLASER);
                boss->bullet_timeout = BOSS_BULLET_TIMEOUT;
        } else --boss->bullet_timeout;

        if(!boss->pattern_timeout) {
                boss->pattern_timeout = (BOSS_BULLET_TIMEOUT+1) * (30 / 5) * 8;
                boss->pattern = boss->pattern % 4 + 1;
        } else --boss->pattern_timeout;
}
