#include "essentials.h"

#define FREE_CHNK(chunk) { if(chunk) Mix_FreeChunk(chunk); }

void clean(WinRend *winrend, Assets *assets) {
        if(winrend->rend) SDL_DestroyRenderer(winrend->rend);
        if(winrend->win) SDL_DestroyWindow(winrend->win);

        puts("freeing font...");;
        if(assets->font) TTF_CloseFont(assets->font);

        puts("freeing sound assets...");
        if(assets->sounds.main_music) Mix_FreeMusic(assets->sounds.main_music);
        if(assets->sounds.pause_music) Mix_FreeMusic(assets->sounds.pause_music);
        if(assets->sounds.boss_music) Mix_FreeMusic(assets->sounds.boss_music);

        SDL_ClearError();
        for(size_t i = 0; i < LNGTH(assets->sounds.sfx); ++i)
                FREE_CHNK(assets->sounds.sfx[i]);
        puts(SDL_GetError());

        puts("quitting SDL...");
        IMG_Quit();
        Mix_Quit();
        TTF_Quit();
        SDL_Quit();
}
