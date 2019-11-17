#include "essentials.h"

static inline void load_textures(SDL_Renderer *, TTF_Font *, Textures *);
static inline SDL_Texture *text_load(SDL_Renderer *, TTF_Font *, const char *);
static inline SDL_Texture *sprite_load(SDL_Renderer *, const char *);
static inline SDL_Texture *create_sparkle(SDL_Renderer *);
static inline void load_sounds(Sounds *);
static inline Mix_Chunk *sound_load(const char *);

int err = 0;

int load_assets(SDL_Renderer *rend, Assets *assets) {
        puts("loading font...");
        assets->font = TTF_OpenFont("assets/prstart.ttf", 8);
        if(!assets->font) {
                fprintf(stderr, "error loading font:\n\t%s\n", TTF_GetError());
                return 1;
        }

        puts("loading textures...");
        load_textures(rend, assets->font, &assets->textures);
        if(err) return 1;

        puts("loading sounds...");
        load_sounds(&assets->sounds);

        return 0;
}

static inline void load_textures(SDL_Renderer *rend, TTF_Font *font, Textures *textures) {
        const char *menu_texts[] = {"MAIN MENU", "Sound", "Controls", "Quit", "Back"};
        for(size_t i = 0; i < LNGTH(textures->menu); ++i)
                textures->menu[i] = text_load(rend, font, menu_texts[i]);

        const char *texts[] = {"PAUSED", "PRESS ENTER", "sound muted", "YOU WIN!!!", "YOU'RE DEAD", "Press ENTER",
                              "LIVES:", "BOSS BATTLE!!!", "YOU MADE THE LEADERBOARD!", "Press ENTER when you're done", "LEADERBOARD"};
        for(size_t i = 0; i < LNGTH(textures->texts); ++i)
                textures->texts[i] = text_load(rend, font, texts[i]);

        const char *filenames[] = {"spaceship.png", "enemy.png", "enemy1.png", "enemy2.png", "enemy3.png",
                                 "goldguy.png", "newboss.png", "good_bullet.png", "bad_bullet.png", "explosion.png"};
        for(size_t i = 0; i < LNGTH(textures->sprites); ++i)
                textures->sprites[i] = sprite_load(rend, filenames[i]);

        textures->sparkle = create_sparkle(rend);
}

static inline SDL_Texture *text_load(SDL_Renderer *rend, TTF_Font *font, const char *str) {
        SDL_Surface *surf = TTF_RenderText_Blended(font, str, white);
        if(!surf) {
                fprintf(stderr, "error rendering white text\"%s\":\n\t%s\n", str, TTF_GetError());
                err = 1;
                return NULL;
        }

        SDL_Texture *text = SDL_CreateTextureFromSurface(rend, surf);
        SDL_FreeSurface(surf);
        return text;
}

static inline SDL_Texture *sprite_load(SDL_Renderer *rend, const char *img) {
        char filepath[25] = "assets/sprites/";
        strcat(filepath, img);
        SDL_Surface *surf = IMG_Load(filepath);
        if(!surf) {
                fprintf(stderr, "error loading \"%s\":\n\t%s\n", img, IMG_GetError());
                err = 1;
                return NULL;
        }

        SDL_Texture *text = SDL_CreateTextureFromSurface(rend, surf);
        SDL_FreeSurface(surf);
        return text;
}

static inline SDL_Texture *create_sparkle(SDL_Renderer *rend) {
        const SDL_Rect rects[] = {{0, 0, 1, 1}, {2, 0, 1, 1}, {1, 1, 1, 1}, {0, 2, 1, 1}, {2, 2, 1, 1}};
        SDL_Surface *surf = SDL_CreateRGBSurfaceWithFormat(0, 3, 3, 8, SDL_PIXELFORMAT_RGBA32);
        SDL_FillRects(surf, rects, 5, SDL_MapRGB(surf->format, 255, 255, 255));
        SDL_Texture *text = SDL_CreateTextureFromSurface(rend, surf);
        SDL_FreeSurface(surf);
        return text;
}

static inline void load_sounds(Sounds *sounds) {
        Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024);

        sounds->main_music = Mix_LoadMUS("assets/sounds/music_1.ogg");
        if(!sounds->main_music)
                fprintf(stderr, "error loading main_music:\n\t%s\n", Mix_GetError());

        sounds->pause_music = Mix_LoadMUS("assets/sounds/waiting.ogg");
        if(!sounds->pause_music)
                fprintf(stderr, "error loading pause_music:\n\t%s\n", Mix_GetError());

        sounds->boss_music = Mix_LoadMUS("assets/sounds/bossbattle.ogg");
        if(!sounds->boss_music)
                fprintf(stderr, "error loading boss_music:\n\t%s\n", Mix_GetError());

        const char *sfx[] = {"laser.ogg", "badlaser.ogg", "newexplosion2.ogg", "thunk.ogg", "lowerthunk.ogg", "powerchord.ogg"};
        for(size_t i = 0; i < LNGTH(sounds->sfx); ++i)
                sounds->sfx[i] = sound_load(sfx[i]);
}

static inline Mix_Chunk *sound_load(const char *file) {
        char filepath[25] = "assets/sounds/";
        strcat(filepath, file);
        Mix_Chunk *chunk = Mix_LoadWAV(filepath);
        if(!chunk)
                fprintf(stderr, "error loading \"%s\":\n\t%s\n", file, Mix_GetError());
        return chunk;
}
