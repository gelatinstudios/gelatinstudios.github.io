
#include "essentials.h"

SDL_Texture *make_leaderboard_text(SDL_Renderer *rend, TTF_Font *font, const Score *score, const size_t n) {
        char name[4] = {0};
        if(!score->name[0])
                strcpy(name, "---");
        else {
                name[0] = score->name[0];
                name[1] = score->name[1];
                name[2] = score->name[2];
        }

        char str[30] = {0};
        sprintf(str, "%c%c%zu %s: %u", score->won ? 'W' : ' ', ' ', n+1, name, score->val);
        SDL_Surface *surf = TTF_RenderText_Blended(font, str, white);
        assert(surf);
        SDL_Texture *text = SDL_CreateTextureFromSurface(rend, surf);
        SDL_FreeSurface(surf);
        return text;
}

SDL_Texture *make_score_text(SDL_Renderer *rend, TTF_Font *font, unsigned score) {
        //CNT_START;
        char str[50] = {0};
        sprintf(str, "SCORE: %u", score);
        SDL_Surface *surf = TTF_RenderText_Blended(font, str, white);
        //SDL_UpdateTexture(*text, NULL, surf->pixels, surf->pitch * 100);
        assert(surf);
        SDL_Texture *text = SDL_CreateTextureFromSurface(rend, surf);
        SDL_FreeSurface(surf);
        //CNT_PRINT("score update");
        return text;
}

SDL_Texture *make_highscore_text(SDL_Renderer *rend, TTF_Font *font, unsigned score) {
        //CNT_START;
        char str[50] = {0};
        sprintf(str, "HIGHSCORE: %u", score);
        SDL_Surface *surf = TTF_RenderText_Blended(font, str, white);
        //SDL_UpdateTexture(*text, NULL, surf->pixels, surf->pitch * 100);
        assert(surf);
        SDL_Texture *text = SDL_CreateTextureFromSurface(rend, surf);
        SDL_FreeSurface(surf);
        //CNT_PRINT("score update");
        return text;
}

// TODO: put the const arrays IN the function and just pass the index 

SDL_Texture *make_key_text(SDL_Renderer *rend, TTF_Font *font, const char *action, const char *keyname) {
        char str[50] = {0};
        sprintf(str, "%s: %s", action, keyname);

        SDL_Surface *surf = TTF_RenderText_Blended(font, str, white);
        assert(surf);
        SDL_Texture *text = SDL_CreateTextureFromSurface(rend, surf);
        SDL_FreeSurface(surf);
        return text;
}

SDL_Texture *make_volume_text(SDL_Renderer *rend, TTF_Font *font, const char *barname, int volume) {
        char bar[] = "-----------";
        bar[10*volume/MIX_MAX_VOLUME] = '|';
        char str[27] = {0};
        sprintf(str, "%s: %s", barname, bar);

        SDL_Surface *surf = TTF_RenderText_Blended(font, str, white);
        assert(surf);
        SDL_Texture *text = SDL_CreateTextureFromSurface(rend, surf);
        SDL_FreeSurface(surf);
        return text;
}
