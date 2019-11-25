#include "essentials.h"

void load_scores(Score scores[10]) {
        FILE *file = fopen("assets/scores.dat", "rb");
        if(!file) return;
        fread(scores, sizeof(Score), 10, file);
        //assert(n == 10);
        fclose(file);
}

void print_scores(Score scores[10]) {
        FILE *file = fopen("assets/scores.dat", "wb");
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
                texts[j] = make_leaderboard_text(rend, font, &scores[j], j);
        }

#ifdef PRNT_LDBRD
        for (size_t i = 0; i < 10; ++i)
                printf("%zu %s: %u\n", i + 1, scores[i].name, scores[i].val);
#endif

        return i;
}
