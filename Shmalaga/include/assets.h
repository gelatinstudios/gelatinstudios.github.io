#ifndef ASSETS_H
#define ASSETS_H

enum {MN_TITLE, MN_SOUND, MN_CONTROLS, MN_QUIT, MN_BACK};
enum {TXT_PAUSED, TXT_START_SCREEN, TXT_STAGE, TXT_MUTED, TXT_YOU_WIN, TXT_YOU_DIED, TXT_PRESSR, TXT_LIVES, TXT_BOSS, TXT_MADE, TXT_ENTER_NAME, TXT_LDRBRD};
enum {SPR_SHIP, SPR_ENEMY, SPR_ENEMY1, SPR_ENEMY2, SPR_ENEMY3, SPR_GOLD_ENEMY, SPR_BOSS, SPR_GB, SPR_BB, SPR_EXPLOSION};
#define GOLD_SCORE_NUM 10
#define GOLD_1UP 11
enum {SCORE_TXT, HSCORE_TXT};
typedef struct {
        SDL_Texture *menu[5];
        SDL_Texture *texts[12];
        SDL_Texture *sprites[10];
        SDL_Texture *gold_numbers[12];
        SDL_Texture *white_numbers[10];
        SDL_Texture *white_letters[26];
        SDL_Texture *score_texts[2];
        SDL_Texture *name_texts[3];
        SDL_Texture *leaderboard_texts[10];
        SDL_Texture *sparkle;
} Textures;

enum {SND_LASER, SND_BADLASER, SND_EXP, SND_THNK, SND_LWTHNK, SND_CHRD};
typedef struct {
        Mix_Music *main_music;
        Mix_Music *pause_music;
        Mix_Music *boss_music;
        Mix_Chunk *sfx[6];
} Sounds;

typedef struct {
        TTF_Font *font;
        Textures textures;
        Sounds sounds;
} Assets;

int load_assets(SDL_Renderer *, Assets *, Score[10]);

static inline void play_sound(Mix_Chunk *sound, int x_pos, size_t index) {
        x_pos = 2 * (x_pos - 1280/2) * 254 / 1280;
        int left = 254, right = 254;
        if (x_pos > 0) {
                left -= x_pos;
        } else {
                right += x_pos;
        }

        //printf("left: %d\nright: %d\n\n", left, right);

        Mix_SetPanning(index+2, left, right);
        Mix_PlayChannel(index+2, sound, 0);
}

#endif
