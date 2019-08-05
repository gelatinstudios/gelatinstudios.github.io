
#ifndef GAME_H
#define GAME_H

#include <stdlib.h>
#include <time.h>
#include <tgmath.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include <emscripten.h>

#define STAR_COUNT 600
#define ENEMY_COUNT 5

typedef struct {
        Uint8 r, g, b;
} Color;

typedef struct {
        SDL_Rect rect;
        Color color;
} Star;

typedef struct {
        SDL_Rect rect;
        Uint8 status;
        Uint8 explosion_timeout;

        double angle;

        Uint8 texture;
} Enemy;

typedef struct {
        SDL_Window *win;
        SDL_Renderer *rend;

        TTF_Font *font;

        SDL_Texture *title;
        SDL_Rect title_rect;

        SDL_Texture *ship;
        SDL_Rect ship_rect;

        SDL_Texture *bullet;
        SDL_Texture *enemy_text[3];
        SDL_Texture *exp;

        SDL_Rect exp_src[4];

        Mix_Chunk *laser;
        Mix_Chunk *explosion;

        Star stars[STAR_COUNT];

        SDL_Rect gb[50];
        Uint8 gb_count;
        Uint8 bullet_timeout;
        Uint8 player_status;
        Uint8 living_enemies;
        Uint8 player_death_timeout;

        Enemy enemies[ENEMY_COUNT];
} Game;

        enum {ALIVE, DYING_1, DYING_2, DYING_3, DYING_4, DEAD};

#endif
