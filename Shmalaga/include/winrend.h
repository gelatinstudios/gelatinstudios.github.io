#ifndef WINREND_H
#define WINREND_H

typedef struct {
        SDL_Window *win;
        SDL_Renderer *rend;
} WinRend;

int init_winrend(WinRend *);

static inline void render_surf(SDL_Renderer *rend, SDL_Surface *surf, const SDL_Rect *rect) {
        SDL_Texture *text = SDL_CreateTextureFromSurface(rend, surf);
        SDL_FreeSurface(surf);
        SDL_RenderCopy(rend, text, NULL, rect);
        SDL_DestroyTexture(text);
}

#endif
