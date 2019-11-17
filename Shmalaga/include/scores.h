#ifndef SCORES_H
#define SCORES_H

typedef struct {
        char name[3];
        unsigned val;
        bool won;
} Score;

void load_scores(Score[10]);
void print_scores(Score[10]);
size_t scores_update(Score[10], Score);

#endif
