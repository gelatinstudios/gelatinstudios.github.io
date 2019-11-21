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

int intro(Star *, SDL_Renderer *, TTF_Font *);

int handler(GameData *, WinRend *, Assets *);

int menu_handler(GameData *, Mix_Chunk *[], SDL_Event *);
void render_menu(GameData *, SDL_Renderer *, TTF_Font *, SDL_Texture *[]);

void load_level(GameData *);

void automata(GameData *, Sounds *sounds);

void update(GameData *data, SDL_Renderer *rend, Sounds *sounds, SDL_Texture *score_text, TTF_Font *font);

void render(GameData *, SDL_Renderer *, Assets *);

void name_handler(GameData *, SDL_Renderer *,TTF_Font *, SDL_Event*, SDL_Texture *[10]);
void render_name(GameData *, SDL_Renderer *, Textures *);
void render_leaderboard(Score[], SDL_Renderer *, Textures *, size_t);

void clean(WinRend *, Assets *);

#endif
