#include "essentials.h"

#define FREE_CHNK(chunk) { if(chunk) Mix_FreeChunk(chunk); }

void clean(WinRend *winrend, Assets *assets) {
        // puts("freeing allocated data...");
        // if(data->stars) free(data->stars);
        // //if(data->gb) free(data->gb);
        // if(data->enemies) free(data->enemies);
        // //if(data->bb) free(data->bb);

        puts("destroying window and renderer...");
        if(winrend->win) SDL_DestroyWindow(winrend->win);
        if(winrend->rend) SDL_DestroyRenderer(winrend->rend);

        puts("freeing font...");;
        if(assets->font) TTF_CloseFont(assets->font);

        puts("destroying textures...");
        for(size_t i = 0; i < LNGTH(assets->textures.texts); ++i)
                if(assets->textures.texts[i]) SDL_DestroyTexture(assets->textures.texts[i]);
        for(size_t i = 0; i < LNGTH(assets->textures.menu); ++i)
                if(assets->textures.menu[i]) SDL_DestroyTexture(assets->textures.menu[i]);
        for(size_t i = 0; i < LNGTH(assets->textures.sprites); ++i)
                if(assets->textures.sprites[i]) SDL_DestroyTexture(assets->textures.sprites[i]);
        if(assets->textures.sparkle) SDL_DestroyTexture(assets->textures.sparkle);

        puts("freeing sound assets...");
        if(assets->sounds.main_music) Mix_FreeMusic(assets->sounds.main_music);
        if(assets->sounds.pause_music) Mix_FreeMusic(assets->sounds.pause_music);
        if(assets->sounds.boss_music) Mix_FreeMusic(assets->sounds.boss_music);

        for(size_t i = 0; i < LNGTH(assets->sounds.sfx); ++i)
                FREE_CHNK(assets->sounds.sfx[i]);

        puts("quitting SDL...");
        IMG_Quit();
        Mix_Quit();
        TTF_Quit();
        SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
        SDL_Quit();
}
