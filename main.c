#include <unistd.h>
#include <stdio.h>
//#include <SDL_opengles2_gl2.h>
#include "SDL2/SDL.h"
#include "sdl_fb.h"
#include "sdl_buttons.h"
#include "colors.h"
//#include "SDL2/SDL_gfxPrimitives.h"
//#include "SDL2/SDL2_rotozoom.h"
//#include "SDL2/SDL_image.h"
//#include "sdl_fb.h"
#include "badge_apps.h"

//void badgelandia_task(void* p_arg);
//test_badge_app(NULL);
//badgelandia_task(NULL);

#define APP_UNDER_TEST badgelandia_task
//#define APP_UNDER_TEST screensaver_task


void test_badge_app(void *p_arg){
    uint i = 0;

    FbBackgroundColor(RED);
    FbClear();
    FbColor(BLUE);
    FbMove(30, 30);
    FbFilledRectangle(30, 30);
    //FbHorizontalLine(10, 10, 30, 30);

    //for(i=0; i < 100; i++)
    //    write_buffer_pixel(&G_Fb, 30, 10+i,
    //                            0, 200, 100, 100);

    FbSwapBuffers();
    while(G_Fb.status)
    {

    }
}


int main() {
    SDL_Thread *event_thread;
    int         threadReturnValue;
    struct sdl_context sdl_cxt;
    sdl_cxt.width = WIDTH;
    sdl_cxt.height = HEIGHT;

    construct_sdl_context(&sdl_cxt);

    /* Simply create a thread */
    event_thread = SDL_CreateThread(handle_events,
                                      "EventThread",
                                      (void *)&sdl_cxt);

    if (NULL == event_thread) {
        printf("\nSDL_CreateThread failed: %s\n", SDL_GetError());
    }

    SET_GLOBAL_SDL_CXT(&sdl_cxt);
    //test_badge_app(NULL);
    //badgelandia_task(NULL);
    APP_UNDER_TEST(NULL);
    SDL_WaitThread(event_thread, &threadReturnValue);
    printf("\nThread returned value: %d", threadReturnValue);

    SDL_DestroyRenderer( sdl_cxt.renderer );
    SDL_DestroyWindow( sdl_cxt.window );
    SDL_Quit();

    return 0;
}

int old_main() {
    SDL_Thread *thread;
    int         threadReturnValue;
    struct sdl_context sdl_cxt;
    sdl_cxt.width = WIDTH;
    sdl_cxt.height = HEIGHT;

    construct_sdl_context(&sdl_cxt);

    /* Simply create a thread */
    thread = SDL_CreateThread(handle_events,
                              "EventThread",
                              (void *)&sdl_cxt);

    if (NULL == thread) {
        printf("\nSDL_CreateThread failed: %s\n", SDL_GetError());
    }

    unsigned int i = 0;

    while( sdl_cxt.status ) {

        SDL_SetRenderDrawColor(sdl_cxt.renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(sdl_cxt.renderer);

        rand_pixels_stream(&sdl_cxt, 50,
                            25, 200, 240);
        SDL_RenderPresent(sdl_cxt.renderer);
    }
    SDL_WaitThread(thread, &threadReturnValue);
    printf("\nThread returned value: %d", threadReturnValue);

    SDL_DestroyRenderer( sdl_cxt.renderer );
    SDL_DestroyWindow( sdl_cxt.window );
    SDL_Quit();


    sleep(5);
#ifdef DONT_SKIP
	if (SDL_MUSTLOCK(sdl_cxt.surface))
		SDL_LockSurface(sdl_cxt.surface);

		Uint8 r, g, b, a;
        int access, w, h;
        SDL_QueryTexture (sdl_cxt.buffer, &main_format, &access, &w, &h);

		// This will probably warn you about addressing locals
		SDL_GetRGBA(color, sdl_cxt.surface->format, &r, &g, &b, &a);

		// Now give it transparent pixels
		color = SDL_MapRGBA(sdl_cxt.surface->format, r, g, b, a);

		int bpp = sdl_cxt.surface->format->BytesPerPixel;

		/* Here p is the address to the pixel we want to set */
		Uint8 *p = (Uint8 *)sdl_cxt.surface->pixels + y * sdl_cxt.surface->pitch + x * bpp;

		//prevent placeing pixel outside of screens bounds
		if( ((y*(sdl_cxt.surface->pitch/4)) < HEIGHT*WIDTH)
		    && (y*(sdl_cxt.surface->pitch/4)) >=0 )
				*(Uint32 *)p = color;

	if( SDL_MUSTLOCK(sdl_cxt.surface))
		SDL_UnlockSurface(sdl_cxt.surface);

    SDL_RenderCopy(sdl_cxt.renderer, sdl_cxt.surface, NULL, NULL);
    SDL_RenderPresent(sdl_cxt.renderer);
#endif

    return 0;
}