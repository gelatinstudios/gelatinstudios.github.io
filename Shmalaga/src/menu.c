#include "essentials.h"

#define VOL_SCALE 8

static inline void exit_menu(GameData *);
static inline void render_as_selected(SDL_Renderer *, SDL_Texture *, const SDL_Rect *);
static inline void render_key(SDL_Renderer *, TTF_Font *, const SDL_Rect *, const char *, const char *, int, Uint8);
static inline void render_volume(SDL_Renderer *, TTF_Font *, const SDL_Rect *, const char *, int, int);

int menu_handler(GameData *data, Mix_Chunk *sfx[], SDL_Event *event) {
        int quit = 0;

        if(event->type != SDL_KEYDOWN) return 0;
        switch(data->gamestate) {
                case MENU:
                        switch(event->key.keysym.sym) {
                                case SDLK_ESCAPE:       if(!data->muted) Mix_PlayChannel(-1, sfx[SND_THNK], 0);
                                                        exit_menu(data);
                                                        break;

                                case SDLK_UP:           data->selected = (data->selected - 1 + 3) % 3;
                                                        if(!data->muted) Mix_PlayChannel(-1, sfx[SND_LWTHNK], 0);
                                                        break;

                                case SDLK_DOWN:         data->selected = (data->selected + 1) % 3;
                                                        if(!data->muted) Mix_PlayChannel(-1, sfx[SND_LWTHNK], 0);
                                                        break;

                                case SDLK_RETURN:       switch(data->selected) {
                                                                case 0: data->gamestate = CONTROLS;
                                                                        data->selected = 0;
                                                                        break;

                                                                case 1: data->gamestate = SOUND;
                                                                        data->selected = 0;
                                                                        break;

                                                                case 2: quit = 1;
                                                        }
                                                        if(!data->muted) Mix_PlayChannel(-1, sfx[SND_THNK], 0);
                                                        break;
                        } break;
                case CONTROLS:
                        switch(event->key.keysym.sym) {
                                case SDLK_UP:           if(data->selected == 10) data->selected = 4;
                                                        else if(data->selected == 5) data->selected = 10;
                                                        else  data->selected = (data->selected - 1 + 11) % 11;
                                                        if(!data->muted) Mix_PlayChannel(-1, sfx[SND_LWTHNK], 0);
                                                        break;

                                case SDLK_DOWN:         if(data->selected == 4) data->selected = 10;
                                                        else data->selected = (data->selected + 1) % 11;
                                                        if(!data->muted) Mix_PlayChannel(-1, sfx[SND_LWTHNK], 0);
                                                        break;

                                case SDLK_LEFT:         if(data->selected > 4 && data->selected != 10) data->selected -= 5;
                                                        if(!data->muted) Mix_PlayChannel(-1, sfx[SND_LWTHNK], 0);
                                                        break;

                                case SDLK_RIGHT:        if(data->selected < 5) data->selected += 5;
                                                        if(!data->muted) Mix_PlayChannel(-1, sfx[SND_LWTHNK], 0);
                                                        break;

                                case SDLK_ESCAPE:       if(!data->muted) Mix_PlayChannel(-1, sfx[SND_THNK], 0);
                                                        exit_menu(data);
                                                        break;

                                case SDLK_RETURN:       if(data->selected == 10) {
                                                                data->selected = 0;
                                                                data->gamestate = MENU;
                                                        } else data->gamestate = KEYSET_MODE;
                                                        if(!data->muted) Mix_PlayChannel(-1, sfx[SND_THNK], 0);
                                                        return 0;
                        } break;
                case KEYSET_MODE:
                        if(data->selected < 6) data->keys[data->selected] = event->key.keysym.scancode;
                        else data->keys[data->selected] = event->key.keysym.sym;
                        data->gamestate = CONTROLS;
                        if(!data->muted) Mix_PlayChannel(-1, sfx[SND_THNK], 0);
                        return 0;
                case SOUND:
                        switch(event->key.keysym.sym) {
                                case SDLK_RETURN:       if(data->selected == 3) {
                                                                data->muted = !data->muted;
                                                                if(data->muted) Mix_PauseMusic();
                                                                else Mix_ResumeMusic();
                                                        } else if(data->selected == 4) {
                                                                data->selected = 0;
                                                                data->gamestate = MENU;
                                                        }
                                                        if(!data->muted) Mix_PlayChannel(-1, sfx[SND_THNK], 0);
                                                        break;

                                case SDLK_UP:           data->selected = (data->selected - 1 + 5) % 5;
                                                        if(!data->muted) Mix_PlayChannel(-1, sfx[SND_LWTHNK], 0);
                                                        break;

                                case SDLK_DOWN:         data->selected = (data->selected + 1) % 5;
                                                        if(!data->muted) Mix_PlayChannel(-1, sfx[SND_LWTHNK], 0);
                                                        break;

                                case SDLK_ESCAPE:       if(!data->muted) Mix_PlayChannel(-1, sfx[SND_LWTHNK], 0);
                                                        exit_menu(data);
                                                        break;
                        }

                        const Uint8 *state = SDL_GetKeyboardState(NULL);
                        if(state[SDL_SCANCODE_LEFT]) {
                                switch(data->selected) {
                                        case 0: if(data->volumes.master > 0) data->volumes.master -= VOL_SCALE; break;
                                        case 1: if(data->volumes.music > 0) data->volumes.music -= VOL_SCALE; break;
                                        case 2: if(data->volumes.sfx > 0) data->volumes.sfx -= VOL_SCALE; break;
                                }

                                Mix_VolumeMusic(data->volumes.music * data->volumes.master / MIX_MAX_VOLUME);
                                Mix_Volume(-1, data->volumes.sfx * data->volumes.master / MIX_MAX_VOLUME);
                        }
                        if(state[SDL_SCANCODE_RIGHT]) {
                                switch(data->selected) {
                                        case 0: if(data->volumes.master < MIX_MAX_VOLUME) data->volumes.master += VOL_SCALE; break;
                                        case 1: if(data->volumes.music < MIX_MAX_VOLUME) data->volumes.music += VOL_SCALE; break;
                                        case 2: if(data->volumes.sfx < MIX_MAX_VOLUME) data->volumes.sfx += VOL_SCALE; break;
                                }

                                Mix_VolumeMusic(data->volumes.music * data->volumes.master / MIX_MAX_VOLUME);
                                Mix_Volume(-1, data->volumes.sfx * data->volumes.master / MIX_MAX_VOLUME);
                        }
                        break;
        }

        return quit;
}

static inline void exit_menu(GameData *data) {
        data->selected = 0;
        data->gamestate = IN_GAME;
        if(!data->muted) Mix_ResumeMusic();
}

void render_menu(GameData *data, SDL_Renderer *rend, TTF_Font *font, SDL_Texture *menu[]) {
        SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);

        const SDL_Rect title = {1280/2 - 250, 80, 500, 75};
        switch(data->gamestate) {
                case MENU: {
                        const SDL_Rect controls = {1280/2 - 175, 75+80+40, 350, 50};
                        const SDL_Rect sound = {1280/2 - 100, 75+80+40+50+40, 200, 50};
                        const SDL_Rect quit = {1280/2 - 100, 75+80+3*40+2*50, 200, 50};

                        SDL_RenderCopy(rend, menu[MN_TITLE], NULL, &title);

                        if(data->selected == 0) render_as_selected(rend, menu[MN_CONTROLS], &controls);
                        else SDL_RenderCopy(rend, menu[MN_CONTROLS], NULL, &controls);

                        if(data->selected == 1) render_as_selected(rend, menu[MN_SOUND], &sound);
                        else SDL_RenderCopy(rend, menu[MN_SOUND], NULL, &sound);

                        if(data->selected == 2) render_as_selected(rend, menu[MN_QUIT], &quit);
                        else SDL_RenderCopy(rend, menu[MN_QUIT], NULL, &quit);
                        break;
                }
                case CONTROLS:
                case KEYSET_MODE: {
                        const char *actions[] = {"MOVE UP", "MOVE DOWN", "MOVE LEFT", "MOVE RIGHT", "SHOOT", "SLOW", "MENU", "FULLSCREEN", "MUTE", "RESET"};

                        SDL_Rect rect = {100, 80, 490, 75};
                        for(size_t i = 0; i < 10; ++i) {
                                if(i == 5) {
                                        rect.x = 200+490;
                                        rect.y = 80;
                                }
                                rect.y += 90;

                                const char *keyname = i < 6 ? SDL_GetScancodeName(data->keys[i]) : SDL_GetKeyName(data->keys[i]);
                                render_key(rend, font, &rect, actions[i], keyname, data->selected == i, data->gamestate);
                        }

                        const SDL_Rect back = {1280/2 - 100, 80+6*75+6*15, 200, 50};

                        SDL_RenderCopy(rend, menu[MN_CONTROLS], NULL, &title);

                        if(data->selected == 10) render_as_selected(rend, menu[MN_BACK], &back);
                        else SDL_RenderCopy(rend, menu[MN_BACK], NULL, &back);
                        break;
                }
                case SOUND: {
                        const SDL_Rect master = {1280/2 - 400, 75+80+40, 800, 50};
                        const SDL_Rect music = {1280/2 - 400, 75+80+40+50+40, 800, 50};
                        const SDL_Rect sfx = {1280/2 - 400, 75+80+3*40+2*50, 800, 50};
                        const SDL_Rect mute = {1280/2 - 200, 75+80+4*40+3*50, 400, 50};
                        const SDL_Rect back = {1280/2 - 100, 75+80+5*40+4*50, 200, 50};

                        SDL_RenderCopy(rend, menu[MN_SOUND], NULL, &title);

                        render_volume(rend, font, &master, "MASTER VOLUME", data->selected == 0, data->volumes.master);
                        render_volume(rend, font, &music, "MUSIC VOLUME", data->selected == 1, data->volumes.music);
                        render_volume(rend, font, &sfx, "SFX VOLUME", data->selected == 2, data->volumes.sfx);

                        char mute_str[16] = "MUTE: ";
                        strcat(mute_str, data->muted ? "SOUND OFF" : "SOUND ON");
                        SDL_Surface *mute_surf;
                        if(data->selected == 3) mute_surf = TTF_RenderText_Blended(font, mute_str, gold);
                        else mute_surf = TTF_RenderText_Blended(font, mute_str, white);
                        render_surf(rend, mute_surf, &mute);

                        if(data->selected == 4) render_as_selected(rend, menu[MN_BACK], &back);
                        else SDL_RenderCopy(rend, menu[MN_BACK], NULL, &back);
                        break;
                }
        }
}

static inline void render_as_selected(SDL_Renderer *rend, SDL_Texture *text, const SDL_Rect *rect) {
        SDL_SetTextureColorMod(text, 255, 215, 0);
        SDL_RenderCopy(rend, text, NULL, rect);
        SDL_SetTextureColorMod(text, 255, 255, 255);
}

static inline void render_key(SDL_Renderer *rend, TTF_Font *font, const SDL_Rect *rect, const char *action, const char *keyname, int selected, Uint8 mode) {
        char str[50] = {0};
        sprintf(str, "%s: %s", action, keyname);

        SDL_Surface *surf;
        if(mode == KEYSET_MODE && selected) surf = TTF_RenderText_Blended(font, "PRESS A KEY", gold);
        else if(selected) surf = TTF_RenderText_Blended(font, str, gold);
        else surf = TTF_RenderText_Blended(font, str, white);
        render_surf(rend, surf, rect);
}

static inline void render_volume(SDL_Renderer *rend, TTF_Font *font, const SDL_Rect *rect, const char *barname, int selected, int volume) {
        char bar[] = "-----------";
        bar[10*volume/MIX_MAX_VOLUME] = '|';
        char str[27] = {0};
        sprintf(str, "%s: %s", barname, bar);

        SDL_Surface *surf;
        if(selected) surf = TTF_RenderText_Blended(font, str, gold);
        else surf = TTF_RenderText_Blended(font, str, white);
        render_surf(rend, surf, rect);
}
