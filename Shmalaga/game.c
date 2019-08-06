
#include "game.h"

#define LOAD(x) surf = IMG_Load(x)
#define CREATE_TEXT SDL_CreateTextureFromSurface(game.rend, surf)
#define FREE_SURF SDL_FreeSurface(surf)

#define WIDTH 500
#define HEIGHT 100

#define SHIP_VEL 8
#define ENEMY_VEL 6.0
#define BULLET_VEL 12
#define BULLET_TIMEOUT 10
#define EXPLOSION_TIMEOUT 4

#define DEGRAD(x) ((x) * (M_PI/180.0))

Uint8 collision_detect(SDL_Rect rect1, SDL_Rect rect2) {
        Uint8 x = (rect1.x < rect2.x + rect2.w) && (rect1.x + rect1.w > rect2.x);
        Uint8 y = (rect1.y < rect2.y + rect2.h) && (rect1.y + rect1.h > rect2.y);
        return x && y;
}

Mix_Chunk *sound_load(const char *file) {
        char filepath[25] = {0};
        sprintf(filepath, "assets/sounds/%s", file);
        return Mix_LoadWAV(filepath);
}

void loop(void *arg) {
        Game *game = arg;

        //input_handler
        SDL_Event event = {0};
        while(SDL_PollEvent(&event))
                if(event.type == SDL_WINDOWEVENT_CLOSE || event.type == SDL_QUIT) {
                        SDL_DestroyWindow(game->win);
                        SDL_DestroyRenderer(game->rend);
                        TTF_CloseFont(game->font);
                        SDL_DestroyTexture(game->title);
                        SDL_DestroyTexture(game->ship);
                        SDL_DestroyTexture(game->bullet);
                        for(size_t i = 0; i < 3; ++i)
                                SDL_DestroyTexture(game->enemy_text[i]);
                        SDL_DestroyTexture(game->exp);
                        Mix_FreeChunk(game->laser);
                        Mix_FreeChunk(game->explosion);
                        TTF_Quit();
                        IMG_Quit();
                        Mix_Quit();
                        SDL_Quit();
                        emscripten_cancel_main_loop();
                }
        if(game->player_status == ALIVE) {
                const Uint8 *state = SDL_GetKeyboardState(NULL);
                if(state[SDL_SCANCODE_LEFT] && game->ship_rect.x > 0) game->ship_rect.x -= SHIP_VEL;
                if(state[SDL_SCANCODE_RIGHT] && game->ship_rect.x < 1280-56) game->ship_rect.x += SHIP_VEL;
                if(state[SDL_SCANCODE_UP] && game->ship_rect.y > 0) game->ship_rect.y -= SHIP_VEL;
                if(state[SDL_SCANCODE_DOWN] && game->ship_rect.y < 720-56) game->ship_rect.y += SHIP_VEL;
                if(state[SDL_SCANCODE_Z] && !game->bullet_timeout) {
                        game->gb[game->gb_count].x = game->ship_rect.x + 45;
                        game->gb[game->gb_count].y = game->ship_rect.y + 56/2 - 10/2;
                        ++game->gb_count;
                        game->bullet_timeout = BULLET_TIMEOUT;
                        Mix_PlayChannel(-1, game->laser, 0);
                }
                if(game->bullet_timeout) --game->bullet_timeout;
        }

        //action handler
        if(game->player_status == DEAD) {
                game->ship_rect.x = 100;
                game->ship_rect.y = 780/2 - 56/2;
                game->player_status = ALIVE;
        }
        if(!game->living_enemies) {
                game->living_enemies = ENEMY_COUNT;
                for(size_t i = 0; i < ENEMY_COUNT; ++i) {
                        game->enemies[i].rect.x = 100 + rand()%(1280-65-200);
                        game->enemies[i].rect.y = 100 + rand()%(720-65-200);
                        game->enemies[i].rect.w = game->enemies[i].rect.h = 55 + rand()%11;

                        game->enemies[i].status = ALIVE;
                        game->enemies[i].angle = 0;
                        game->enemies[i].texture = rand()%3;
                }
        }
        if(game->player_status > ALIVE && game->player_status < DEAD) {
                if(!game->player_death_timeout) {
                        ++game->player_status;
                        game->player_death_timeout = EXPLOSION_TIMEOUT;
                } else --game->player_death_timeout;
        }
        for(size_t i = 0; i < ENEMY_COUNT; ++i) {
                if(game->player_status == ALIVE && !game->enemies[i].status && collision_detect(game->ship_rect, game->enemies[i].rect)) {
                        game->player_status = DYING_1;
                        game->player_death_timeout = EXPLOSION_TIMEOUT;
                        ++game->deaths;
                        Mix_PlayChannel(-1, game->explosion, 0);
                }

                for(size_t j = 0; j < game->gb_count; ++j) {
                        if(game->enemies[i].status == ALIVE && collision_detect(game->gb[j], game->enemies[i].rect)) {
                                game->enemies[i].status = DYING_1;
                                game->enemies[i].explosion_timeout = EXPLOSION_TIMEOUT;
                                for(size_t k = j; k < game->gb_count - 1; ++k)
                                        game->gb[k] = game->gb[k+1];
                                --game->gb_count;
                                ++game->kills;
                                Mix_PlayChannel(-1, game->explosion, 0);
                        }
                }

                if(game->enemies[i].status > ALIVE && game->enemies[i].status < DEAD) {
                        if(!game->enemies[i].explosion_timeout) {
                                ++game->enemies[i].status;
                                if(game->enemies[i].status == DEAD) --game->living_enemies;
                                else game->enemies[i].explosion_timeout = EXPLOSION_TIMEOUT;
                        } else --game->enemies[i].explosion_timeout;
                }
        }
        if(game->gb[0].x > 1280 && game->gb_count) {
                for(size_t i = 0; i < game->gb_count - 1; ++i)
                        game->gb[i] = game->gb[i+1];
                --game->gb_count;
        }

        //automata
        for(size_t i = 0; i < ENEMY_COUNT; ++i) {
                if(game->enemies[i].status != ALIVE) continue;
                game->enemies[i].rect.x -= rint(ENEMY_VEL * cos(DEGRAD(game->enemies[i].angle)));
                game->enemies[i].rect.y -= rint(ENEMY_VEL * sin(DEGRAD(game->enemies[i].angle)));
                game->enemies[i].angle += 3;
                if(game->enemies[i].angle > 360) game->enemies[i].angle = 360 - game->enemies[i].angle;
        }
        for(size_t i = 0; i < game->gb_count; ++i) game->gb[i].x += BULLET_VEL;

        //render
        SDL_SetRenderDrawColor(game->rend, 0, 0, 0, 255);
        SDL_RenderClear(game->rend);

        //stars
        for(size_t i = 0; i < STAR_COUNT; ++i) {
                SDL_SetRenderDrawColor(game->rend, game->stars[i].color.r, game->stars[i].color.g, game->stars[i].color.b, 255);
                SDL_RenderFillRect(game->rend, &game->stars[i].rect);
        }
        SDL_SetRenderDrawColor(game->rend, 0, 0, 0, 0);

        //title
        SDL_RenderCopy(game->rend, game->title, NULL, &game->title_rect);

        //K/D
        char killstr[30] = {0};
        sprintf(killstr, "kills: %zu", game->kills);
        SDL_Surface *surf = TTF_RenderText_Blended(game->font, killstr, white);
        SDL_Texture *kills_text = SDL_CreateTextureFromSurface(game->rend, surf);
        FREE_SURF;
        char deathstr[30] = {0};
        sprintf(deathstr, "deaths: %zu", game->deaths);
        surf = TTF_RenderText_Blended(game->font, deathstr, white);
        SDL_Texture *deaths_text = SDL_CreateTextureFromSurface(game->rend, surf);
        FREE_SURF;

        SDL_Rect kills_rect = {20, 720-20-50, 200, 50};
        SDL_Rect deaths_rect = {1280-20-200, 720-20-50, 200, 50};
        SDL_RenderCopy(game->rend, kills_text, NULL, &kills_rect);
        SDL_RenderCopy(game->rend, deaths_text, NULL, &deaths_rect);
        SDL_DestroyTexture(kills_text);
        SDL_DestroyTexture(deaths_text);

        //player
        if(game->player_status == ALIVE)
                SDL_RenderCopy(game->rend, game->ship, NULL, &game->ship_rect);
        else SDL_RenderCopy(game->rend, game->exp, &game->exp_src[game->player_status-1], &game->ship_rect);

        //enemies
        for(size_t i = 0; i < ENEMY_COUNT; ++i) {
                if(game->enemies[i].status == ALIVE)
                        SDL_RenderCopyEx(game->rend, game->enemy_text[game->enemies[i].texture], NULL, &game->enemies[i].rect, game->enemies[i].angle, NULL, SDL_FLIP_NONE);
                else if(game->enemies[i].status != DEAD)
                        SDL_RenderCopy(game->rend, game->exp, &game->exp_src[game->enemies[i].status-1], &game->enemies[i].rect);
        }

        //bullets
        for(size_t i = 0; i < game->gb_count; ++i)
                SDL_RenderCopy(game->rend, game->bullet, NULL, &game->gb[i]);

        SDL_RenderPresent(game->rend);
}

int main(void) {
        SDL_Init(SDL_INIT_EVERYTHING);
        TTF_Init();
        srand(time(NULL));

        Game game = {0};

        game.win = SDL_CreateWindow("Shmalaga Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
        game.rend = SDL_CreateRenderer(game.win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

        game.font = TTF_OpenFont("assets/prstart.ttf", 8);
        SDL_Surface *surf = TTF_RenderText_Blended(game.font, "Shmalaga", white);
        game.title = CREATE_TEXT;
        FREE_SURF;
        game.title_rect.x = 1280/2 - WIDTH/2;
        game.title_rect.y = 100;
        game.title_rect.w = WIDTH;
        game.title_rect.h = HEIGHT;

        LOAD("assets/sprites/spaceship.png");
        game.ship = CREATE_TEXT;
        FREE_SURF;
        game.ship_rect.x = 100;
        game.ship_rect.y = 780/2 - 56/2;
        game.ship_rect.w = 56;
        game.ship_rect.h = 56;

        LOAD("assets/sprites/good_bullet.png");
        game.bullet = CREATE_TEXT;
        FREE_SURF;

        LOAD("assets/sprites/enemy.png");
        game.enemy_text[0] = CREATE_TEXT;
        FREE_SURF;
        LOAD("assets/sprites/enemy1.png");
        game.enemy_text[1] = CREATE_TEXT;
        FREE_SURF;
        LOAD("assets/sprites/enemy2.png");
        game.enemy_text[2] = CREATE_TEXT;
        FREE_SURF;

        LOAD("assets/sprites/explosion.png");
        game.exp = CREATE_TEXT;
        FREE_SURF;
        game.exp_src[0].x = 0;
        game.exp_src[0].y = 0;
        game.exp_src[1].x = 32;
        game.exp_src[1].y = 0;
        game.exp_src[2].x = 0;
        game.exp_src[2].y = 32;
        game.exp_src[3].x = 32;
        game.exp_src[3].y = 32;
        for(size_t i = 0; i < 4; ++i) {
                game.exp_src[i].w = 32;
                game.exp_src[i].h = 32;
        }

        Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024);
        game.laser = sound_load("laser.wav");
        // if(!game.laser)
        //         fprintf(stderr, "error loading laser sound:\n\t%s\n", Mix_GetError());
        game.explosion = sound_load("explosion.wav");
        // if(!game.explosion)
        //         fprintf(stderr, "error loading explosion sound:\n\t%s\n", Mix_GetError());
        //Mix_Chunk *bad_laser = sound_load("sounds/bad_laser.wav");

        for(size_t i = 0; i < STAR_COUNT; ++i) {
                game.stars[i].rect.x = rand() % 1280;
                game.stars[i].rect.y = rand() % 720;
                game.stars[i].rect.w = game.stars[i].rect.h = 1 + rand()%4;

                Uint8 n = 1 + rand() % 100;
                if(n <= 80) { //white
                        game.stars[i].color.r = 255;
                        game.stars[i].color.g = 255;
                        game.stars[i].color.b = 255;
                } else if(n <= 86) { //red
                        game.stars[i].color.r = 255;
                        game.stars[i].color.g = 0;
                        game.stars[i].color.b = 0;
                } else if(n <= 93) { //blue
                        game.stars[i].color.r = 0;
                        game.stars[i].color.g = 0;
                        game.stars[i].color.b = 255;
                } else if(n <= 98) { //yellow
                        game.stars[i].color.r = 255;
                        game.stars[i].color.g = 255;
                        game.stars[i].color.b = 0;
                } else { //orange
                        game.stars[i].color.r = 255;
                        game.stars[i].color.g = 165;
                        game.stars[i].color.b = 0;
                }
        }

        for(size_t i = 0; i < 50; ++i) {
                game.gb[i].w = 10;
                game.gb[i].h = 10;
        }

        game.player_status = ALIVE;

        emscripten_set_main_loop_arg(loop, &game, 0, 1);
}
