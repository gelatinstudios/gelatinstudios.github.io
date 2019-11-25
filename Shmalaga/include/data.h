#ifndef DATA_H
#define DATA_H

//array macros
#define MAX_ENEMIES 202
#define MAX_BULLETS 30
//#define MAX_ENEMY_BULLETS 20
#define MAX_ENEMY_BULLETS 50
#define MAX_STARS 1250

#define ENEM_P 500

#define LEVEL_TIMEOUT 45
#define ONE_UP_TIMEOUT 60
#define RESPAWN_TIMEOUT 120
#define SPARKLE_TIMEOUT 5
#define EXPLOSION_TIMEOUT 5
#define NUM_EXP 20
#define NEW_EXP_SCALE 9
#define NEW_EXP_TIMEOUT (NEW_EXP_SCALE*NUM_EXP)
#define BOSS_EXPLOSION_TIMEOUT 6
#define BOSS_BULLET_TIMEOUT 12
#define BULLET_TIMEOUT 10
#define DAMAGE_TIMEOUT 6

#define MAX_STAR_POS 5000

#define BOSS_HP 500

//player/enemy status
enum {ALIVE, DYING_1, DYING_2, DYING_3, DYING_4, DEAD};

//gamestates
enum {IN_GAME, STARTING_SCREEN, YOU_WIN, NAME_ENTRY, LEADERBOARD, MENU, CONTROLS, KEYSET_MODE, SOUND};

//structures
typedef struct {
        Uint8 r, g, b;
} Color;

typedef struct {
        SDL_Rect rect;
        Color color;
} Star;

typedef struct {
        SDL_Point point;
        float angle;
        float dx, dy;
} BB;

typedef struct {
        SDL_Rect rect;
        Uint8 status;
        Uint8 explosion_timeout;

        float angle;
        Uint8 circle;
        Uint8 passes;

        Uint8 texture;
} Enemy;

typedef struct {
        SDL_Rect rect;
        Uint8 timeout;
} Sparkle;

typedef struct {
        SDL_Rect rect;
        Uint8 timeout;
} Explosion;

typedef struct {
        SDL_Rect rect;
        Uint8 status;
        Uint16 new_exp_timeout;
        Uint16 explosion_timeout;
        Uint16 angles[5];

        Explosion explosions[NUM_EXP];

        Uint8 pattern;
        Uint16 pattern_timeout;
        bool change;

        Uint8 bullet_timeout;
        Uint8 damage_timeout;
        Uint16 hp;
} Boss;

enum {UP_K, DOWN_K, LEFT_K, RIGHT_K, SHOOT_K, SLOW_K, MENU_K, FSCREEN_K, MUTE_K, RESET_K};
enum {VL_MASTER, VL_MUSIC, VL_SFX};
typedef struct {
        SDL_Keycode keys[10]; //contains SDL_Scancodes too i guess this is polymorphism not really

        Uint8 selected;

        bool fullscreen;
        bool muted;

        Uint8 volumes[3];

        const SDL_Keycode *sekritptr;
        Uint8 secret;

        Uint8 gamestate;
        Uint8 level;
        Uint8 level_timeout;

        Score leaderboard[10];
        Score score;
        Uint8 player_score_index;

        Uint8 lives;
        Uint8 respawn_timeout;
        Uint8 life_milestone;
        Uint8 one_up_timeout;

        Star stars[MAX_STARS];

        SDL_Rect ship;

        Uint8 player_status;
        Uint8 player_death_timeout;

        SDL_Point gb[MAX_BULLETS];
        Uint8 gb_count;
        Uint8 bullet_timeout;
        Uint8 boss_bullet_timeout;

        Enemy enemies[MAX_ENEMIES];
        Uint8 enemy_count;
        Uint8 living_enemies;

        Sparkle sparkle;
        Enemy gold_enemy;
        Boss boss;

        BB bb[MAX_ENEMY_BULLETS];
        Uint8 bb_count;
} GameData;

extern const SDL_Keycode *secret_codes[4];

extern const SDL_Color white;
extern const SDL_Color black;
extern const SDL_Color gold;


extern const SDL_Rect explosion_clips[4];
extern const SDL_Rect boss_clips[4];

int init_data(GameData *);

#endif
