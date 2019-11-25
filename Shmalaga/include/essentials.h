#ifndef ESSENTIALS_H
#define ESSENTIALS_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <tgmath.h>
#include <assert.h>

#ifdef SHMALAGA_DEBUG

        #include <errno.h>
        #include <sched.h>

        static inline size_t rdtsc(){
            unsigned int lo,hi;
            __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
            return ((size_t)hi << 32) | lo;
        }

        size_t cycles;
        size_t sdl_ticks;

        #define CNT_START (cycles = rdtsc())
        // #define CNT_RESTART (cycles = rdstc())
        #define CNT_PRINT(x) (printf("%s: clock cycles: %zu\n", (x), rdtsc() - cycles))

        #define SCNT_START (sdl_ticks = SDL_GetPerformanceCounter())
        #define SCNT_PRINT(x) (printf("%s: sdl_ticks: %zu\n", (x), SDL_GetPerformanceCounter() - sdl_ticks))

        #define D_START ({CNT_START; SCNT_START;})
        #define D_PRINT(x) ({CNT_PRINT(x); SCNT_PRINT(x);})

#else

        #define CNT_START
        #define CNT_PRINT(x)
        #define SCNT_START
        #define SCNT_PRINT(x)
        #define D_START
        #define D_PRINT(x)

        #define NDEBUG

#endif

#include <emscripten.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "algorithms.h"
#include "scores.h"
#include "data.h"
#include "assets.h"
#include "bullets.h"
#include "winrend.h"

typedef struct {
        GameData *data;
        WinRend *winrend;
        Assets *assets;
} Everything;

//intro.c
int intro(Star *, SDL_Renderer *, TTF_Font *);

//handler.c
int handler(GameData *, WinRend *, Assets *);

//textures.c
SDL_Texture *make_leaderboard_text(SDL_Renderer *rend, TTF_Font *font, const Score *score, const size_t n);
SDL_Texture *make_score_text(SDL_Renderer *rend, TTF_Font *font, unsigned score);
SDL_Texture *make_highscore_text(SDL_Renderer *rend, TTF_Font *font, unsigned score);
SDL_Texture *make_key_text(SDL_Renderer *rend, TTF_Font *font, const char *action, const char *keyname);
SDL_Texture *make_volume_text(SDL_Renderer *rend, TTF_Font *font, const char *barname, int volume);

//menu.c
int menu_handler(GameData *data, SDL_Renderer *rend, Menu_Textures *texts, Mix_Chunk *sfx[], TTF_Font *font, SDL_Event *event);
void render_menu(GameData *data, SDL_Renderer *rend, Menu_Textures *texts);

//levels.c
void load_level(GameData *);

//automata.c
void automata(GameData *, Sounds *sounds);

//update.c
void update(GameData *data, SDL_Renderer *rend, Sounds *sounds, SDL_Texture **score_text, TTF_Font *font);

//render.c
void render(GameData *, SDL_Renderer *, Assets *);

//leaderboard.c
void name_handler(GameData *, SDL_Renderer *,TTF_Font *, SDL_Event*, SDL_Texture *[10], SDL_Texture *[2]);
void render_name(GameData *, SDL_Renderer *, Textures *);
void render_leaderboard(Score[], SDL_Renderer *, Textures *, size_t);

//clean.c
void clean(WinRend *, Assets *);


#endif
