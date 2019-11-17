#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#define DEGRAD(x) ((x) * (M_PI/180.0))
#define RADDEG(x) ((x) * (180.0/M_PI))

#define LNGTH(x) ((sizeof (x)) / (sizeof (x[0])))

#define GB_W 15
#define GB_H 10
#define BB_W 20
#define BB_H 8

#define MK_GB_RECT(a) (const SDL_Rect){a.x, a.y, GB_W, GB_H}
#define MK_BB_RECT(a) (const SDL_Rect){a.x, a.y, BB_W, BB_H}

static inline int rng(int n) {
        int x;
        do { x = rand();} while(x >= (RAND_MAX - RAND_MAX % n));
        return x % n;
}

static inline int collision_detect(SDL_Rect rect1, SDL_Rect rect2) {
        const int x = (rect1.x < rect2.x + rect2.w) && (rect1.x + rect1.w > rect2.x);
        const int y = (rect1.y < rect2.y + rect2.h) && (rect1.y + rect1.h > rect2.y);
        return x && y;
}

static inline int is_on_screen(SDL_Rect rect) {
        const int x = rect.x + rect.w > 0 && rect.x < 1280;
        const int y = rect.y + rect.h > 0 && rect.y < 720;
        return x && y;
}

#endif
