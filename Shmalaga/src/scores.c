#include "essentials.h"

void load_scores(Score scores[10]) {
        FILE *file = fopen("scores", "rb");
        if(!file) return;
        size_t n = fread(scores, sizeof(Score), 10, file);
        assert(n == 10);
        fclose(file);
}

void print_scores(Score scores[10]) {
        FILE *file = fopen("scores", "wb");
        fwrite(scores, sizeof(Score), 10, file);
        fclose(file);
}

size_t scores_update(Score scores[10], Score score, SDL_Renderer *rend, TTF_Font *font, SDL_Texture *texts[10]) {
        assert(score.val > scores[9].val);

        size_t i = 8;
        while(score.val > scores[i].val && i < 9) {
                scores[i+1] = scores[i];
                --i;
        }
        ++i;
        scores[i] = score;

        for (size_t j = i; j < 10; ++j) {
                SDL_ClearError();
                SDL_DestroyTexture(texts[j]);
                printf("error at %zu: %s\n", j, SDL_GetError());
                texts[j] = make_score_texture(rend, font, &scores[j], j);
        }

#ifdef PRNT_LDBRD
        for (size_t i = 0; i < 10; ++i)
                printf("%zu %s: %u\n", i + 1, scores[i].name, scores[i].val);
#endif

        return i;
}

SDL_Texture *make_score_texture(SDL_Renderer *rend, TTF_Font *font, const Score *score, size_t n) {
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
