#include "essentials.h"

#define STRT_POS (1280*2)
#define ENEMY_VEL 8
#define SQRT3 1.732050807568877

static inline void load_level_1(GameData *data);
static inline void load_level_2(GameData *data);
static inline void load_level_3(GameData *data);
static inline void load_level_4(GameData *data);
static inline void load_level_5(GameData *data);
static inline void load_level_6(GameData *data);
static inline void load_level_7(GameData *data);

void load_level(GameData *data) {
        void (*load_level[])(GameData *) = {load_level_1,
                                            load_level_2,
                                            load_level_3,
                                            load_level_4,
                                            load_level_5,
                                            load_level_6,
                                            load_level_7};
        assert(data->level && data->level <= 7);

        load_level[data->level-1](data);

        if(data->level >= 2 && data->level < 7) {
                data->gold_enemy.status = ALIVE;
                data->gold_enemy.rect.x = 1280*3;
                data->gold_enemy.rect.y = rng(720/3) + 720/3;
        }

        for(size_t i = 0; i < data->enemy_count; ++i) {
                data->enemies[i].status = ALIVE;
                data->enemies[i].passes = 0;
        }

        data->level_timeout = LEVEL_TIMEOUT;
}

static inline void load_level_1(GameData *data) {
        data->enemy_count = data->living_enemies = 1;

        // data->gold_enemy.status = ALIVE;
        // data->gold_enemy.rect.x = 1280*2 + 100;
        // data->gold_enemy.rect.y = 720/2 - data->gold_enemy.rect.h/2;

        data->enemies[0].rect.x = 1280 * 2;
        data->enemies[0].rect.y = (720 - 60) / 2;
}

//big arrow
static inline void load_level_2(GameData *data) {
        data->enemy_count = data->living_enemies = 201;

        for(size_t j = 0; j < 3; ++j) {
                const size_t n = j * 67;

                //central line
                data->enemies[n+0].rect.x = STRT_POS*(j+1);
                data->enemies[n+1].rect.x = STRT_POS*(j+1) + 260;
                data->enemies[n+2].rect.x = STRT_POS*(j+1) + 665;
                data->enemies[n+3].rect.x = STRT_POS*(j+1) + 1196;
                data->enemies[n+4].rect.x = STRT_POS*(j+1) + 1766;
                for(size_t i = 0; i < 5; ++i)
                        data->enemies[n+i].rect.y = 720/2 - data->enemies[n+i].rect.h/2;

                //first diag
                data->enemies[n+5].rect.x = STRT_POS*(j+1) + 100;
                data->enemies[n+6].rect.x = STRT_POS*(j+1) + 206;
                data->enemies[n+7].rect.x = STRT_POS*(j+1) + 318;
                data->enemies[n+8].rect.x = STRT_POS*(j+1) + 414;
                data->enemies[n+9].rect.x = STRT_POS*(j+1) + 510;
                data->enemies[n+5].rect.y = 269;
                data->enemies[n+6].rect.y = 210;
                data->enemies[n+7].rect.y = 150;
                data->enemies[n+8].rect.y = 85;
                data->enemies[n+9].rect.y = 20;

                //second diag
                data->enemies[n+10].rect.x = STRT_POS*(j+1) + 372;
                data->enemies[n+11].rect.x = STRT_POS*(j+1) + 480;
                data->enemies[n+12].rect.x = STRT_POS*(j+1) + 580;
                data->enemies[n+13].rect.x = STRT_POS*(j+1) + 684;
                data->enemies[n+14].rect.x = STRT_POS*(j+1) + 788;
                data->enemies[n+10].rect.y = 263;
                data->enemies[n+11].rect.y = 215;
                data->enemies[n+12].rect.y = 162;
                data->enemies[n+13].rect.y = 110;
                data->enemies[n+14].rect.y = 52;

                //first curve
                data->enemies[n+15].rect.x = STRT_POS*(j+1) + 765;
                data->enemies[n+16].rect.x = STRT_POS*(j+1) + 870;
                data->enemies[n+17].rect.x = STRT_POS*(j+1) + 975;
                data->enemies[n+18].rect.x = STRT_POS*(j+1) + 1090;
                data->enemies[n+19].rect.x = STRT_POS*(j+1) + 1206;
                data->enemies[n+20].rect.x = STRT_POS*(j+1) + 1323;
                data->enemies[n+21].rect.x = STRT_POS*(j+1) + 1442;
                data->enemies[n+22].rect.x = STRT_POS*(j+1) + 1545;
                data->enemies[n+23].rect.x = STRT_POS*(j+1) + 1652;
                data->enemies[n+24].rect.x = STRT_POS*(j+1) + 1762;
                data->enemies[n+25].rect.x = STRT_POS*(j+1) + 1864;
                data->enemies[n+15].rect.y = 262;
                data->enemies[n+16].rect.y = 218;
                data->enemies[n+17].rect.y = 180;
                data->enemies[n+18].rect.y = 150;
                data->enemies[n+19].rect.y = 124;
                data->enemies[n+20].rect.y = 120;
                data->enemies[n+21].rect.y = 114;
                data->enemies[n+22].rect.y = 104;
                data->enemies[n+23].rect.y = 84;
                data->enemies[n+24].rect.y = 58;
                data->enemies[n+25].rect.y = 26;

                //second curve
                data->enemies[n+26].rect.x = STRT_POS*(j+1) + 1320;
                data->enemies[n+27].rect.x = STRT_POS*(j+1) + 1450;
                data->enemies[n+28].rect.x = STRT_POS*(j+1) + 1582;
                data->enemies[n+29].rect.x = STRT_POS*(j+1) + 1710;
                data->enemies[n+30].rect.x = STRT_POS*(j+1) + 1834;
                data->enemies[n+31].rect.x = STRT_POS*(j+1) + 1946;
                data->enemies[n+32].rect.x = STRT_POS*(j+1) + 2052;
                data->enemies[n+33].rect.x = STRT_POS*(j+1) + 2164;
                data->enemies[n+26].rect.y = 250;
                data->enemies[n+27].rect.y = 222;
                data->enemies[n+28].rect.y = 198;
                data->enemies[n+29].rect.y = 184;
                data->enemies[n+30].rect.y = 162;
                data->enemies[n+31].rect.y = 126;
                data->enemies[n+32].rect.y = 74;
                data->enemies[n+33].rect.y = 40;

                //last diag
                data->enemies[n+34].rect.x = STRT_POS*(j+1) + 1984;
                data->enemies[n+35].rect.x = STRT_POS*(j+1) + 2188;
                data->enemies[n+34].rect.y = 270;
                data->enemies[n+35].rect.y = 224;

                //mirror
                for(size_t i = 0; i < 31; ++i) {
                        data->enemies[n+i+36].rect.x = data->enemies[n+i+5].rect.x;
                        data->enemies[n+i+36].rect.y = 720-60 - data->enemies[n+i+5].rect.y;
                }
        }
}

static inline void load_level_3(GameData *data) {
        data->enemy_count = data->living_enemies = 150;

        for(size_t i = 0; i < 25; ++i) {
                for(size_t j = 0; j < 3; ++j) {
                        data->enemies[j + i*3].rect.x = 350 + j*326 + rng(163) - 81;
                        data->enemies[j + i*3].rect.y = -i*200 - 200 + rng(100) - 50;
                        data->enemies[j + i*3].angle = 300;
                }
        }

        for(size_t i = 0; i < 75; ++i) {
                data->enemies[i+75].rect.x = data->enemies[i].rect.x + 50;
                data->enemies[i+75].rect.y = 1280*2-60 - data->enemies[i].rect.y;
                data->enemies[i+75].angle = 60;
        }
}

static inline void load_level_4(GameData *data) {
        data->enemy_count = data->living_enemies = 150;

        for(size_t i = 0; i < 25; ++i) {
                for(size_t j = 0; j < 3; ++j) {
                        data->enemies[j + i*3].rect.x = 200 + j*326 + rng(163) - 81;
                        data->enemies[j + i*3].rect.y = -i*400 - 200 + rng(200) - 100;
                        data->enemies[j + i*3].angle = 240;
                }
        }

        for(size_t i = 0; i < 75; ++i) {
                data->enemies[i+75].rect.x = data->enemies[i].rect.x + 50;
                data->enemies[i+75].rect.y = 1280*2-60 - data->enemies[i].rect.y;
                data->enemies[i+75].angle = 120;
        }
}

static inline void load_level_5(GameData *data) {
        data->enemy_count = data->living_enemies = 70;

        for(size_t i = 0; i < 7; ++i) {
                for(size_t j = 0; j < 10; ++j) {
                        data->enemies[j + i*10].rect.x = STRT_POS + 1280*i;
                        data->enemies[j + i*10].rect.y = 72*j;
                        data->enemies[j + i*10].angle = 0;
                }
        }
}

static inline void load_level_6(GameData *data) {
        data->enemy_count = data->living_enemies = 150;

        for(size_t j = 0; j < 50; ++j) {
                data->enemies[j].rect.x = 200 + rng(1280-200);
                data->enemies[j].rect.y = -500 - j * 300;
                data->enemies[j].angle = 270;

                for(size_t k = 0; k < j; ++k)
                        if(collision_detect(data->enemies[j].rect, data->enemies[k].rect))
                                --j;
        }
        for(size_t j = 0; j < 50; ++j) {
                data->enemies[j+50].rect.x = STRT_POS + j * 300;
                data->enemies[j+50].rect.y = rng(720);
                data->enemies[j+50].angle = 0;

                for(size_t k = 0; k < j; ++k)
                        if(collision_detect(data->enemies[j+50].rect, data->enemies[k+50].rect))
                                --j;
        }
        for(size_t j = 0; j < 50; ++j) {
                data->enemies[j+2*50].rect.x = 200 + rng(1280-200);
                data->enemies[j+2*50].rect.y = 720 + 500 + j * 300;
                data->enemies[j+2*50].angle = 90;

                for(size_t k = 0; k < j; ++k)
                        if(collision_detect(data->enemies[j+2*50].rect, data->enemies[k+2*50].rect))
                                --j;
        }
}

static inline void load_level_7(GameData *data) {
        data->enemy_count = 0;
        data->living_enemies = 1;
        data->gold_enemy.status = DEAD;
        data->boss.status = ALIVE;

        data->boss.rect.x = 1280*2;
        data->boss.rect.y = 720/2 - data->boss.rect.h/2;
}
