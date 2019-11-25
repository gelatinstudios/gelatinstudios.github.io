#include "essentials.h"

#define ONE_UP 100000U

static inline void enemy_handler(GameData *, Enemy *, Mix_Chunk *, SDL_Texture **, SDL_Renderer *, TTF_Font *);
static inline void player_death(GameData *, Mix_Chunk *);

void update(GameData *data, SDL_Renderer *rend, Sounds *sounds, SDL_Texture **score_text, TTF_Font *font) {
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
                enemy_handler(data, &data->enemies[i], sounds->sfx[SND_EXP], score_text, rend, font);

        enemy_handler(data, &data->gold_enemy, sounds->sfx[SND_EXP], score_text, rend, font);

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
                        SDL_DestroyTexture(*score_text);
                        *score_text = make_score_text(rend, font, data->score.val);
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

static inline void enemy_handler(GameData *data, Enemy *enemy, Mix_Chunk *explosion, SDL_Texture **score_text, SDL_Renderer *rend, TTF_Font *font) {
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
                        SDL_DestroyTexture(*score_text);
                        *score_text = make_score_text(rend, font, data->score.val);
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
