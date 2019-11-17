#include "essentials.h"

void load_scores(Score scores[10]) {
        FILE *file = fopen("scores", "r");
        if(!file) return;
        fread(scores, sizeof(Score), 10, file);
        fclose(file);
}

void print_scores(Score scores[10]) {
        FILE *file = fopen("scores", "w");
        fwrite(scores, sizeof(Score), 10, file);
        fclose(file);
}

size_t scores_update(Score scores[10], Score score) {
        assert(score.val > scores[9].val);

        size_t i = 8;
        while(score.val > scores[i].val && i < 9) {
                scores[i+1] = scores[i];
                --i;
        }
        scores[i+1] = score;
        return i + 1;
}
