#ifndef WINREND_H
#define WINREND_H

typedef struct {
        SDL_Window *win;
        SDL_Renderer *rend;
} WinRend;

int init_winrend(WinRend *);

#endif
