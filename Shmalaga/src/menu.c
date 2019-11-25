#include "essentials.h"

#define VOL_SCALE 8

static inline void exit_menu(GameData *);
static inline void render_as_selected(SDL_Renderer *, SDL_Texture *, const SDL_Rect *);

int menu_handler(GameData *data, SDL_Renderer *rend, Menu_Textures *texts, Mix_Chunk *sfx[], TTF_Font *font, SDL_Event *event) {
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
                case KEYSET_MODE: {
                        SDL_DestroyTexture(texts->key_texts[data->selected]);
                        const char *actions[] = {"MOVE UP", "MOVE DOWN", "MOVE LEFT", "MOVE RIGHT", "SHOOT", "SLOW", "MENU", "FULLSCREEN", "MUTE", "RESET"};
                        if(data->selected < 6) {
                                data->keys[data->selected] = event->key.keysym.scancode;
                                texts->key_texts[data->selected] = make_key_text(rend, font, actions[data->selected], SDL_GetScancodeName(data->keys[data->selected]));
                        } else {
                                data->keys[data->selected] = event->key.keysym.sym;
                                texts->key_texts[data->selected] = make_key_text(rend, font, actions[data->selected], SDL_GetKeyName(data->keys[data->selected]));
                        }
                        data->gamestate = CONTROLS;
                        if(!data->muted) Mix_PlayChannel(-1, sfx[SND_THNK], 0);



                        return 0;
                }
                case SOUND: {
                        switch(event->key.keysym.sym) {
                                case SDLK_RETURN:       if(data->selected == 3) {
                                                                //FIX: music starts playing in the menu; it shouldn't
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
                        if(state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_RIGHT]) {
                                if(state[SDL_SCANCODE_LEFT]) data->volumes[data->selected] -= VOL_SCALE;
                                else data->volumes[data->selected] += VOL_SCALE;

                                SDL_DestroyTexture(texts->volume_texts[data->selected]);

                                const char *barnames[] = {"MASTER VOLUME", "MUSIC VOLUME", "SFX VOLUME"};
                                texts->volume_texts[data->selected] = make_volume_text(rend, font, barnames[data->selected], data->volumes[data->selected]);

                                Mix_VolumeMusic(data->volumes[VL_MUSIC] * data->volumes[VL_MASTER] / MIX_MAX_VOLUME);
                                Mix_Volume(-1, data->volumes[VL_SFX] * data->volumes[VL_MASTER] / MIX_MAX_VOLUME);
                        }
                }
        }

        return quit;
}

static inline void exit_menu(GameData *data) {
        data->selected = 0;
        data->gamestate = IN_GAME;
        if(!data->muted) Mix_ResumeMusic();
}

void render_menu(GameData *data, SDL_Renderer *rend, Menu_Textures *texts) {
        SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);

        const SDL_Rect title = {1280/2 - 250, 80, 500, 75};
        switch(data->gamestate) {
                case MENU: {
                        const SDL_Rect controls = {1280/2 - 175, 75+80+40, 350, 50};
                        const SDL_Rect sound = {1280/2 - 100, 75+80+40+50+40, 200, 50};
                        const SDL_Rect quit = {1280/2 - 100, 75+80+3*40+2*50, 200, 50};

                        SDL_RenderCopy(rend, texts->title_texts[MN_TITLE], NULL, &title);

                        if(data->selected == 0) render_as_selected(rend, texts->title_texts[MN_CONTROLS], &controls);
                        else SDL_RenderCopy(rend, texts->title_texts[MN_CONTROLS], NULL, &controls);

                        if(data->selected == 1) render_as_selected(rend, texts->title_texts[MN_SOUND], &sound);
                        else SDL_RenderCopy(rend, texts->title_texts[MN_SOUND], NULL, &sound);

                        if(data->selected == 2) render_as_selected(rend, texts->title_texts[MN_QUIT], &quit);
                        else SDL_RenderCopy(rend, texts->title_texts[MN_QUIT], NULL, &quit);
                } break;
                case CONTROLS:
                case KEYSET_MODE: {
                        SDL_Rect rect = {100, 80, 490, 75};
                        for(size_t i = 0; i < 10; ++i) {
                                if(i == 5) {
                                        rect.x = 200+490;
                                        rect.y = 80;
                                }
                                rect.y += 90;

                                if(data->gamestate == KEYSET_MODE && data->selected == i) SDL_RenderCopy(rend, texts->press_a_key, NULL, &rect);
                                else if(data->selected == i) render_as_selected(rend, texts->key_texts[i], &rect);
                                else SDL_RenderCopy(rend, texts->key_texts[i], NULL, &rect);
                        }

                        const SDL_Rect back = {1280/2 - 100, 80+6*75+6*15, 200, 50};

                        SDL_RenderCopy(rend, texts->title_texts[MN_CONTROLS], NULL, &title);

                        if(data->selected == 10) render_as_selected(rend, texts->title_texts[MN_BACK], &back);
                        else SDL_RenderCopy(rend, texts->title_texts[MN_BACK], NULL, &back);
                } break;
                case SOUND: {
                        const SDL_Rect master = {1280/2 - 400, 75+80+40, 800, 50};
                        const SDL_Rect music = {1280/2 - 400, 75+80+40+50+40, 800, 50};
                        const SDL_Rect sfx = {1280/2 - 400, 75+80+3*40+2*50, 800, 50};
                        const SDL_Rect mute = {1280/2 - 200, 75+80+4*40+3*50, 400, 50};
                        const SDL_Rect back = {1280/2 - 100, 75+80+5*40+4*50, 200, 50};

                        SDL_RenderCopy(rend, texts->title_texts[MN_SOUND], NULL, &title);

                        if(data->selected == 0) render_as_selected(rend, texts->volume_texts[VL_MASTER], &master);
                        else SDL_RenderCopy(rend, texts->volume_texts[VL_MASTER], NULL, &master);

                        if(data->selected == 1) render_as_selected(rend, texts->volume_texts[VL_MUSIC], &music);
                        else SDL_RenderCopy(rend, texts->volume_texts[VL_MUSIC], NULL, &music);

                        if(data->selected == 2) render_as_selected(rend, texts->volume_texts[VL_SFX], &sfx);
                        else SDL_RenderCopy(rend, texts->volume_texts[VL_SFX], NULL, &sfx);

                        if(data->muted) {
                                if(data->selected == 3) render_as_selected(rend, texts->mute_texts[MTTXT_OFF], &mute);
                                else SDL_RenderCopy(rend, texts->mute_texts[MTTXT_OFF], NULL, &mute);
                        } else {
                                if(data->selected == 3) render_as_selected(rend, texts->mute_texts[MTTXT_ON], &mute);
                                else SDL_RenderCopy(rend, texts->mute_texts[MTTXT_ON], NULL, &mute);
                        }

                        if(data->selected == 4) render_as_selected(rend, texts->title_texts[MN_BACK], &back);
                        else SDL_RenderCopy(rend, texts->title_texts[MN_BACK], NULL, &back);
                } break;
        }
}

static inline void render_as_selected(SDL_Renderer *rend, SDL_Texture *text, const SDL_Rect *rect) {
        SDL_SetTextureColorMod(text, 255, 215, 0);
        SDL_RenderCopy(rend, text, NULL, rect);
        SDL_SetTextureColorMod(text, 255, 255, 255);
}
