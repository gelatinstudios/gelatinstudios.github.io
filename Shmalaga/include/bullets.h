#ifndef BULLETS_H
#define BULLETS_H

#define BULLET_VEL 12

void create_gb(GameData *);
void destroy_gb(GameData *, size_t);

void create_bb(GameData *, const SDL_Rect *, double angle);
void destroy_bb(GameData *, size_t);

#endif
