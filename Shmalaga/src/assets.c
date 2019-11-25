#include "essentials.h"

static inline void load_textures(SDL_Renderer *, TTF_Font *, Textures *, Score [10]);
static inline void init_menu_texts(SDL_Renderer *rend, TTF_Font *font, Menu_Textures *texts);
static inline SDL_Texture *text_load(SDL_Renderer *, TTF_Font *, const char *);
static inline SDL_Texture *sprite_load(SDL_Renderer *, const char *);
static inline SDL_Texture *gold_number_load(SDL_Renderer *rend, TTF_Font *font, const size_t i);
static inline SDL_Texture *white_number_load(SDL_Renderer *rend, TTF_Font *font, const size_t i);
static inline SDL_Texture *white_letter_load(SDL_Renderer *rend, TTF_Font *font, const char i);
static inline SDL_Texture *create_sparkle(SDL_Renderer *);
static inline void load_sounds(Sounds *);
static inline Mix_Chunk *sound_load(const char *);

int err = 0;

int load_assets(SDL_Renderer *rend, Assets *assets, Score leaderboard[10]) {
        puts("loading font...");
        assets->font = TTF_OpenFont("prstart.ttf", 8);
        if(!assets->font) {
                fprintf(stderr, "error loading font:\n\t%s\n", TTF_GetError());
                return 1;
        }

        puts("loading textures...");
        load_textures(rend, assets->font, &assets->textures, leaderboard);
        if(err) return 1;

        puts("loading sounds...");
        load_sounds(&assets->sounds);

        return 0;
}

static inline void load_textures(SDL_Renderer *rend, TTF_Font *font, Textures *textures, Score leaderboard[10]) {
        init_menu_texts(rend, font, &textures->menu_texts);

        const char *texts[] = {"PAUSED", "PRESS ENTER", "STAGE", "sound muted", "YOU WIN!!!", "YOU'RE DEAD", "Press ENTER",
                              "LIVES:", "BOSS BATTLE!!!", "YOU MADE THE LEADERBOARD!", "Press ENTER when you're done", "LEADERBOARD"};
        for(size_t i = 0; i < LNGTH(textures->texts); ++i)
                textures->texts[i] = text_load(rend, font, texts[i]);

        const char *filenames[] = {"spaceship.png", "enemy.png", "enemy1.png", "enemy2.png", "enemy3.png",
                                 "goldguy.png", "newboss.png", "good_bullet.png", "bad_bullet.png", "explosion.png"};
        for(size_t i = 0; i < LNGTH(textures->sprites); ++i)
                textures->sprites[i] = sprite_load(rend, filenames[i]);

        for(size_t i = 0; i < LNGTH(textures->gold_numbers); ++i)
                textures->gold_numbers[i] = gold_number_load(rend, font, i);

        for(size_t i = 0; i < LNGTH(textures->white_numbers); ++i)
                textures->white_numbers[i] = white_number_load(rend, font, i);

        for(size_t i = 0; i < 26; ++i)
                textures->white_letters[i] = white_letter_load(rend, font, i);

        for(size_t i = 0; i < 10; ++i)
                textures->leaderboard_texts[i] = make_leaderboard_text(rend, font, &leaderboard[i], i);

        //score texts
        textures->score_texts[SCORE_TXT] = make_score_text(rend, font, 0);
        textures->score_texts[HSCORE_TXT] = make_highscore_text(rend, font, leaderboard[0].val);

        textures->sparkle = create_sparkle(rend);
}

static inline void init_menu_texts(SDL_Renderer *rend, TTF_Font *font, Menu_Textures *texts) {
        const char *title_texts[] = {"MAIN MENU", "Sound", "Controls", "Quit", "Back"};
        for(size_t i = 0; i < LNGTH(texts->title_texts); ++i)
                texts->title_texts[i] = text_load(rend, font, title_texts[i]);

        const char *actions[] = {"MOVE UP", "MOVE DOWN", "MOVE LEFT", "MOVE RIGHT", "SHOOT", "SLOW", "MENU", "FULLSCREEN", "MUTE", "RESET"};
        const SDL_Keycode codes[] = {SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_Z, SDL_SCANCODE_LSHIFT, SDLK_ESCAPE, SDLK_f, SDLK_m, SDLK_r};
        for(size_t i = 0; i < LNGTH(texts->key_texts); ++i)
                texts->key_texts[i] = make_key_text(rend, font, actions[i], i < 6 ? SDL_GetScancodeName(codes[i]) : SDL_GetKeyName(codes[i]));

        texts->press_a_key = text_load(rend, font, "PRESS A KEY");
        SDL_SetTextureColorMod(texts->press_a_key, 255, 215, 0); //should be gold

        texts->volume_texts[VL_MASTER] = make_volume_text(rend, font, "MASTER VOLUME", MIX_MAX_VOLUME/2);
        texts->volume_texts[VL_MUSIC] = make_volume_text(rend, font, "MUSIC VOLUME", MIX_MAX_VOLUME/2);
        texts->volume_texts[VL_SFX] = make_volume_text(rend, font, "SFX VOLUME", MIX_MAX_VOLUME/2);

        texts->mute_texts[MTTXT_ON] = text_load(rend, font, "MUTE: SOUND ON");
        texts->mute_texts[MTTXT_OFF] = text_load(rend, font, "MUTE: SOUND OFF");

}

static inline SDL_Texture *white_letter_load(SDL_Renderer *rend, TTF_Font *font, const char i) {
        char str[2] = {0};
        sprintf(str, "%c", i + 'A');

        SDL_Surface *surf = TTF_RenderText_Blended(font, str, white);
        if(!surf) {
                fprintf(stderr, "error rendering score_num text\"%s\":\n\t%s\n", str, TTF_GetError());
                err = 1;
                return NULL;
        }

        SDL_Texture *text = SDL_CreateTextureFromSurface(rend, surf);
        SDL_FreeSurface(surf);
        return text;
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
        char filepath[25] = "sprites/";
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

static inline SDL_Texture *gold_number_load(SDL_Renderer *rend, TTF_Font *font, const size_t i) {
        char str[5] = {0};
        if(i == GOLD_SCORE_NUM) strcpy(str, "1000");
        else if (i == GOLD_1UP) strcpy(str, "1UP");
        else sprintf(str, "%u", ENEM_P / (int) pow(2, i));

        SDL_Surface *surf = TTF_RenderText_Blended(font, str, gold);
        if(!surf) {
                fprintf(stderr, "error rendering score_num text\"%s\":\n\t%s\n", str, TTF_GetError());
                err = 1;
                return NULL;
        }

        SDL_Texture *text = SDL_CreateTextureFromSurface(rend, surf);
        SDL_FreeSurface(surf);
        return text;
}

static inline SDL_Texture *white_number_load(SDL_Renderer *rend, TTF_Font *font, const size_t i) {
        char str[3] = {0};
        sprintf(str, "%zu", i+1);

        SDL_Surface *surf = TTF_RenderText_Blended(font, str, white);
        if(!surf) {
                fprintf(stderr, "error rendering score_num text\"%s\":\n\t%s\n", str, TTF_GetError());
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

        sounds->main_music = Mix_LoadMUS("sounds/music_1.ogg");
        if(!sounds->main_music)
                fprintf(stderr, "error loading main_music:\n\t%s\n", Mix_GetError());

        sounds->pause_music = Mix_LoadMUS("sounds/waiting.ogg");
        if(!sounds->pause_music)
                fprintf(stderr, "error loading pause_music:\n\t%s\n", Mix_GetError());

        sounds->boss_music = Mix_LoadMUS("sounds/bossbattle.ogg");
        if(!sounds->boss_music)
                fprintf(stderr, "error loading boss_music:\n\t%s\n", Mix_GetError());

        const char *sfx[] = {"laser.ogg", "badlaser.ogg", "newexplosion2.ogg", "thunk.ogg", "lowerthunk.ogg", "powerchord.ogg"};
        for(size_t i = 0; i < LNGTH(sounds->sfx); ++i)
                sounds->sfx[i] = sound_load(sfx[i]);
}

static inline Mix_Chunk *sound_load(const char *file) {
        char filepath[25] = "sounds/";
        strcat(filepath, file);
        Mix_Chunk *chunk = Mix_LoadWAV(filepath);
        if(!chunk)
                fprintf(stderr, "error loading \"%s\":\n\t%s\n", file, Mix_GetError());
        return chunk;
}
