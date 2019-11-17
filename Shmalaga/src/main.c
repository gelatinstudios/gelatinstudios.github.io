#include "essentials.h"

//graphically intense
#define FPS (data.secret & 0x01 ? 30 : 60)
//#define FPS 30

static inline void error(const char *, WinRend *, Assets *);

void loop(void *arg);

int main(int argc, char *argv[]) {
        GameData data = {0};
        WinRend winrend = {0};
        Assets assets = {0};

        srand(time(NULL));

        SDL_Init(SDL_INIT_EVERYTHING);
        TTF_Init();
        Mix_Init(MIX_INIT_OGG);

        Uint8 err = init_data(&data);
        if(err) error("initialize data", &winrend, &assets);

        err = init_winrend(&winrend);
        if(err) error("load window and renderer", &winrend, &assets);

        err = load_assets(winrend.rend, &assets);
        if(err) error("load assets", &winrend, &assets);

        Mix_Volume(-1, MIX_MAX_VOLUME/2);
        Mix_VolumeMusic(MIX_MAX_VOLUME/2);
        //Mix_VolumeChunk(assets.sounds.explosion, MIX_MAX_VOLUME/8);

        puts("\nlaunching...\n");

        err = intro(data.stars, winrend.rend, assets.font);
        if(err) error("render intro", &winrend, &assets);

        Everything everything = {&data, &winrend, &assets};
        emscripten_set_main_loop_arg(loop, &everything, 0, 1);
}

void loop(void *arg) {
        Everything *everything = arg;

        GameData *data = everything->data;
        WinRend *winrend = everything->winrend;
        Assets *assets = everything->assets;

        const bool quit = handler(data, winrend->win, &assets->sounds);
        automata(data, &assets->sounds);
        render(data, winrend->rend, assets);

        if(quit) {
                SDL_SetWindowFullscreen(winrend->win, 0);
                print_scores(data->leaderboard);
                puts("exiting...");
                clean(winrend, assets);
                emscripten_cancel_main_loop();
        }
}

static inline void error(const char *str, WinRend *winrend, Assets *assets) {
        fprintf(stderr, "failed to %s\n\nterminating.\n\n", str);
        clean(winrend, assets);
        puts("bruh moment");
        exit(EXIT_FAILURE);
}
