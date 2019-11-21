#include "essentials.h"

void name_handler(GameData *data, SDL_Renderer *rend, TTF_Font *font, SDL_Event *event, SDL_Texture *texts[10]) {
        const char *keyname;
        switch(event->key.keysym.sym) {
                case SDLK_UP:           data->score.name[data->selected] = (data->score.name[data->selected] - 1 + 26 - 'A') % 26 + 'A'; break;
                case SDLK_DOWN:         data->score.name[data->selected] = (data->score.name[data->selected] + 1 - 'A') % 26 + 'A'; break;
                case SDLK_RIGHT:        data->selected = (data->selected + 1) % 3; break;
                case SDLK_LEFT:         data->selected = (data->selected - 1 + 3) % 3; break;
                case SDLK_RETURN:       data->selected = 0;
                                        data->player_score_index = scores_update(data->leaderboard, data->score, rend, font, texts);
                                        data->gamestate = LEADERBOARD;
                                        break;

                default:                keyname = SDL_GetKeyName(event->key.keysym.sym);
                                        if(strlen(keyname) == 1 && isalpha(keyname[0]))
                                                data->score.name[data->selected] = keyname[0];
        }
}

void render_name(GameData *data, SDL_Renderer *rend, Textures *texts) {
        const SDL_Rect title = {(1280 - 1100) / 2, 50, 1100, 125};
        SDL_RenderCopy(rend, texts->texts[TXT_MADE], NULL, &title);

        SDL_Rect rect = {(1280 - 75) / 2 - 2*150, (720 - 200) / 2, 75, 200};
        for(size_t i = 0; i < 3; ++i) {
                rect.x += 150;
                if (i == data->selected) {
                        SDL_SetTextureColorMod(texts->white_letters[data->score.name[i] - 'A'], 255, 215, 0);
                        SDL_RenderCopy(rend, texts->white_letters[data->score.name[i] - 'A'], NULL, &rect);
                        SDL_SetTextureColorMod(texts->white_letters[data->score.name[i] - 'A'], 255, 255, 255);
                } else SDL_RenderCopy(rend, texts->white_letters[data->score.name[i] - 'A'], NULL, &rect);
        }

        const SDL_Rect enter_name = {(1280 - 1000) / 2, 600, 1000, 75};
        SDL_RenderCopy(rend, texts->texts[TXT_ENTER_NAME], NULL, &enter_name);
}

void render_leaderboard(Score scores[], SDL_Renderer *rend, Textures *texts, size_t player_score_index) {
        const SDL_Rect title = {(1280 - 700) / 2, 50, 700, 100};
        SDL_RenderCopy(rend, texts->texts[TXT_LDRBRD], NULL, &title);

        SDL_Rect rect = {50, 75, 0, 50};
        for(size_t i = 0; i < 10; ++i) {
                if(i == 5) {
                        rect.x = 1280/2 + 50;
                        rect.y = 75;
                } else if(i == 9) {
                        rect.x -= 30;
                }
                rect.y += 100;
                rect.w = 30 * (9 + digits(scores[i].val));
                if(i == 9) rect.w += 30;
                if (i == player_score_index) {
                        SDL_SetTextureColorMod(texts->leaderboard_texts[i], 255, 215, 0);
                        SDL_RenderCopy(rend, texts->leaderboard_texts[i], NULL, &rect);
                        SDL_SetTextureColorMod(texts->leaderboard_texts[i], 255, 255, 255);
                } else SDL_RenderCopy(rend, texts->leaderboard_texts[i], NULL, &rect);
        }

        SDL_Rect press_enter_rect = {(1280 - 250) / 2, 650, 250, 50};
        SDL_RenderCopy(rend, texts->texts[TXT_PRESSR], NULL, &press_enter_rect);
}
