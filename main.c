#include <stdio.h>
#include "SDL2/SDL.h"
#include "sdl_fb.h"
#include "sdl_buttons.h"
//#include "SDL2/SDL_gfxPrimitives.h"
//#include "SDL2/SDL2_rotozoom.h"
//#include "SDL2/SDL_image.h"
#include "badge_apps.h"


//#define APP_UNDER_TEST boot_splash_task
//#define APP_UNDER_TEST groundwar_task
#define APP_UNDER_TEST badgelandia_task
//#define APP_UNDER_TEST screensaver_task


int main() {
    SDL_Thread *event_thread;
    int         threadReturnValue;
    struct sdl_context sdl_cxt;

    // Init SDL2 to display a screen and listen for I/O
    construct_sdl_context(&sdl_cxt);

    // Create thread to handle I/O events
    // Use SDL threads/mutexes for portability
    event_thread = SDL_CreateThread(handle_events,
                                      "EventThread",
                                      (void *)&sdl_cxt);

    if (NULL == event_thread) {
        printf("\nSDL_CreateThread failed: %s\n", SDL_GetError());
    }

    // Assigns a global pointer (G_Fb_ptr) to the sdl_cxt
    SET_GLOBAL_SDL_CXT(&sdl_cxt);

    // Main thread runs the app
    APP_UNDER_TEST(NULL);

    SDL_WaitThread(event_thread, &threadReturnValue);
    printf("\nThread returned value: %d", threadReturnValue);

    SDL_DestroyRenderer( sdl_cxt.renderer );
    SDL_DestroyWindow( sdl_cxt.window );
    SDL_Quit();

    return 0;
}

