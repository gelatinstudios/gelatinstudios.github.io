#include "essentials.h"

void create_gb(GameData *data) {
        data->gb[data->gb_count].x = data->ship.x + data->ship.w - 11;
        data->gb[data->gb_count].y = data->ship.y + data->ship.h/2 - 10/2;
        ++data->gb_count;
        data->bullet_timeout = BULLET_TIMEOUT;
}

void destroy_gb(GameData *data, size_t n) {
        for(size_t i = n; i < data->gb_count - 1; ++i)
                data->gb[i] = data->gb[i+1];
        --data->gb_count;
}

void create_bb(GameData *data, const SDL_Rect *rect, double angle) {
        data->bb[data->bb_count].point.x = rint(rect->x + rect->w/2.0 * (1.0 - cos(DEGRAD(angle))) - 12.0/2.0);
        data->bb[data->bb_count].point.y = rint(rect->y + rect->h/2.0 * (1.0 - sin(DEGRAD(angle))) - 8.0/2.0);

        data->bb[data->bb_count].dx = BULLET_VEL * cos(DEGRAD(angle));
        data->bb[data->bb_count].dy = BULLET_VEL * sin(DEGRAD(angle));
        data->bb[data->bb_count].angle = angle;

        ++data->bb_count;
}

void destroy_bb(GameData *data, size_t n) {
        for(size_t i = n; i < data->bb_count - 1; ++i)
                data->bb[i] = data->bb[i+1];
        --data->bb_count;
}
