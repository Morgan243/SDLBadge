#include "sdl_buttons.h"
#include <stdio.h>
#include "sdl_fb.h"
#include "SDL2/SDL.h"
//
// Created by morgan on 4/28/17.
//


unsigned char G_pressed_button = 0;

// Solo bottom right btn
unsigned char G_button_cnt = 0;

// D-Pad
unsigned char G_up_button_cnt = 0;
unsigned char G_down_button_cnt = 0;
unsigned char G_left_button_cnt = 0;
unsigned char G_right_button_cnt = 0;

// Cap Touch
unsigned char G_up_touch_cnt = 0;
unsigned char G_middle_touch_cnt = 0;
unsigned char G_down_touch_cnt = 0;
char G_touch_pct = 40;


unsigned int G_entropy_pool = 961748927;
unsigned int timestamp = 0;

int handle_events(void *p){
    printf("Event thread\n");
    struct sdl_context *sdl_cxt = (struct sdl_context*)p;
    while( sdl_cxt->status ) {
        while (SDL_PollEvent(&sdl_cxt->event)) {
            if ((SDL_QUIT == sdl_cxt->event.type) ||
                (SDL_KEYDOWN == sdl_cxt->event.type &&
                 SDL_SCANCODE_ESCAPE == sdl_cxt->event.key.keysym.scancode)) {
                if (SDL_LockMutex(sdl_cxt->status_lock) == 0) {
                    sdl_cxt->status = 0;
                    SDL_UnlockMutex(sdl_cxt->status_lock);
                } else {
                    fprintf(stderr, "Couldn't lock mutex\n");
                }
                break;
            }

            switch(sdl_cxt->event.type){
                case SDL_KEYUP:
                    switch(sdl_cxt->event.key.keysym.scancode) {
                        case SDL_SCANCODE_A:
                            G_left_button_cnt = 0;
                            REMOVE_FROM_MASK(G_pressed_button, LEFT_BTN_MASK);
                            break;
                        case SDL_SCANCODE_D:
                            G_right_button_cnt = 0;
                            REMOVE_FROM_MASK(G_pressed_button, RIGHT_BTN_MASK);
                            break;
                        case SDL_SCANCODE_S:
                            G_down_button_cnt = 0;
                            REMOVE_FROM_MASK(G_pressed_button, DOWN_BTN_MASK);
                            break;
                        case SDL_SCANCODE_W:
                            G_up_button_cnt = 0;
                            REMOVE_FROM_MASK(G_pressed_button, UP_BTN_MASK);
                            break;
                        case SDL_SCANCODE_SPACE:
                            G_button_cnt = 0;
                            REMOVE_FROM_MASK(G_pressed_button, SOLO_BTN_MASK);
                            break;

                        case SDL_SCANCODE_I:
                            G_up_touch_cnt = 0;
                            REMOVE_FROM_MASK(G_pressed_button, SOLO_BTN_MASK);
                            break;
                        case SDL_SCANCODE_J:
                            G_middle_touch_cnt = 0;
                            REMOVE_FROM_MASK(G_pressed_button, SOLO_BTN_MASK);
                            break;
                        case SDL_SCANCODE_N:
                            G_down_touch_cnt = 0;
                            REMOVE_FROM_MASK(G_pressed_button, SOLO_BTN_MASK);
                            break;
                    }

                case SDL_KEYDOWN:
                    switch(sdl_cxt->event.key.keysym.scancode){
                        case SDL_SCANCODE_A:
                            if(G_left_button_cnt < 255)
                                G_left_button_cnt++;
                            else {
                                G_left_button_cnt = 0;
                                REMOVE_FROM_MASK(G_pressed_button, LEFT_BTN_MASK);
                            }
                            break;
                        case SDL_SCANCODE_W:
                            if(G_up_button_cnt < 255)
                                G_up_button_cnt++;
                            else {
                                G_up_button_cnt = 0;
                                REMOVE_FROM_MASK(G_pressed_button, UP_BTN_MASK);
                            }
                            break;
                        case SDL_SCANCODE_S:
                           if(G_down_button_cnt < 255)
                                G_down_button_cnt++;
                           else {
                                G_down_button_cnt = 0;
                                REMOVE_FROM_MASK(G_pressed_button, DOWN_BTN_MASK);
                           }
                           break;
                        case SDL_SCANCODE_D:
                           if(G_right_button_cnt < 255)
                                G_right_button_cnt++;
                           else {
                                G_right_button_cnt = 0;
                                REMOVE_FROM_MASK(G_pressed_button, RIGHT_BTN_MASK);
                           }
                            break;
                         case SDL_SCANCODE_SPACE:
                           if(G_button_cnt < 255)
                                G_button_cnt++;
                           else {
                                G_button_cnt = 0;
                                REMOVE_FROM_MASK(G_pressed_button, SOLO_BTN_MASK);
                           }
                            break;

                        case SDL_SCANCODE_I:
                            if(G_up_touch_cnt < 255)
                                G_up_touch_cnt++;
                           else {
                                G_up_touch_cnt = 0;
                                REMOVE_FROM_MASK(G_pressed_button, UP_TOUCH_MASK);
                           }
                            break;

                        case SDL_SCANCODE_J:
                             if(G_middle_touch_cnt < 255)
                                G_middle_touch_cnt++;
                             else {
                                G_middle_touch_cnt = 0;
                                 REMOVE_FROM_MASK(G_pressed_button, MIDDLE_TOUCH_MASK);
                                //REMOVE_FROM_MASK(G_pressed_button, SOLO_BTN_MASK);
                             }
                            break;
                        case SDL_SCANCODE_N:
                             if(G_down_touch_cnt < 255)
                                G_down_touch_cnt++;
                             else {
                                G_down_touch_cnt = 0;
                                 REMOVE_FROM_MASK(G_pressed_button, DOWN_TOUCH_MASK);
                                //REMOVE_FROM_MASK(G_pressed_button, SOLO_BTN_MASK);
                             }
                            break;
                        default:
                            G_left_button_cnt = G_right_button_cnt = G_up_button_cnt = G_down_button_cnt = G_button_cnt = 0;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if(sdl_cxt->event.key.keysym.scancode == SDL_MOUSEBUTTONUP)
                        printf("Mouse button up\n");
                    break;
                case SDL_MOUSEWHEEL:
                    if(sdl_cxt->event.wheel.y == 1){
                        if(G_touch_pct < 99)
                            G_touch_pct++;
                    }
                    else if(sdl_cxt->event.wheel.y == -1){
                        if(G_touch_pct > 0)
                            G_touch_pct--;
                    }
                    printf("Touch PCT: %d\n", (int8_t)G_touch_pct);
                    break;
                    //printf("%d, %d" , (sdl_cxt->event.wheel.x, sdl_cxt->event.wheel.y));
                default:
                    //G_left_button_cnt = G_right_button_cnt = G_up_button_cnt = G_down_button_cnt = 0;
                    //printf("No event\n");
                    break;

            }
        }

    }
    return 0;
}