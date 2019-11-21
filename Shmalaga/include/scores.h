#ifndef SCORES_H
#define SCORES_H

typedef struct {
        char name[3];
        unsigned val;
        bool won;
} Score;

void load_scores(Score[10]);
void print_scores(Score[10]);
size_t scores_update(Score[10], Score, SDL_Renderer *, TTF_Font*, SDL_Texture *[10]);
SDL_Texture *make_score_texture(SDL_Renderer *rend, TTF_Font *font, const Score *score, size_t n);

#endif
