#include "essentials.h"

static inline void render_entry(const Score *, SDL_Renderer *, TTF_Font *, SDL_Rect *, size_t, int);

void name_handler(GameData *data, SDL_Event *event) {
        const char *keyname;
        switch(event->key.keysym.sym) {
                case SDLK_UP:           data->score.name[data->selected] = (data->score.name[data->selected] - 1 + 26 - 'A') % 26 + 'A'; break;
                case SDLK_DOWN:         data->score.name[data->selected] = (data->score.name[data->selected] + 1 - 'A') % 26 + 'A'; break;
                case SDLK_RIGHT:        data->selected = (data->selected + 1) % 3; break;
                case SDLK_LEFT:         data->selected = (data->selected - 1 + 3) % 3; break;
                case SDLK_RETURN:       data->selected = 0;
                                        data->player_score_index = scores_update(data->leaderboard, data->score);
                                        data->gamestate = LEADERBOARD;
                                        break;

                default:                keyname = SDL_GetKeyName(event->key.keysym.sym);
                                        if(strlen(keyname) == 1 && isalpha(keyname[0]))
                                                data->score.name[data->selected] = keyname[0];
        }
}

void render_name(GameData *data, SDL_Renderer *rend, SDL_Texture *texts[], TTF_Font *font) {
        const SDL_Rect title = {(1280 - 1100) / 2, 50, 1100, 125};
        SDL_RenderCopy(rend, texts[TXT_MADE], NULL, &title);

        SDL_Rect rect = {(1280 - 75) / 2 - 2*150, (720 - 200) / 2, 75, 200};
        for(size_t i = 0; i < 3; ++i) {
                char letter_str[] = {data->score.name[i], 0};
                SDL_Surface *surf;
                if(data->selected == i) surf = TTF_RenderText_Blended(font, letter_str, gold);
                else surf = TTF_RenderText_Blended(font, letter_str, white);
                rect.x += 150;
                render_surf(rend, surf, &rect);
        }

        const SDL_Rect enter_name = {(1280 - 1000) / 2, 600, 1000, 75};
        SDL_RenderCopy(rend, texts[TXT_ENTER_NAME], NULL, &enter_name);

        SDL_RenderPresent(rend);
}

void render_leaderboard(Score scores[], SDL_Renderer *rend, SDL_Texture *texts[], TTF_Font *font, size_t player_score_index) {
        const SDL_Rect title = {(1280 - 700) / 2, 50, 700, 100};
        SDL_RenderCopy(rend, texts[TXT_LDRBRD], NULL, &title);

        SDL_Rect rect = {50, 75, 0, 50};
        for(size_t i = 0; i < 10; ++i) {
                if(i == 5) {
                        rect.x = 1280/2 + 50;
                        rect.y = 75;
                } else if(i == 9) rect.x -= 30;
                rect.y += 100;
                render_entry(&scores[i], rend, font, &rect, i, player_score_index == i);
        }


        SDL_Rect press_enter_rect = {(1280 - 250) / 2, 650, 250, 50};
        SDL_RenderCopy(rend, texts[TXT_PRESSR], NULL, &press_enter_rect);

        SDL_RenderPresent(rend);
}

static inline void render_entry(const Score *score, SDL_Renderer *rend, TTF_Font *font, SDL_Rect *rect, size_t n, int current_player) {
        char name[4] = {0};
        if(!score->name[0])
                strcpy(name, "---");
        else {
                name[0] = score->name[0];
                name[1] = score->name[1];
                name[2] = score->name[2];
        }

        char str[30] = {0};
        sprintf(str, "%c%c%zu %s: %u", score->won ? 'W' : ' ', ' ', n+1, name, score->val);
        rect->w = strlen(str) * 30;
        SDL_Surface *surf = TTF_RenderText_Blended(font, str, current_player ? gold : white);
        assert(surf);
        render_surf(rend, surf, rect);
}
