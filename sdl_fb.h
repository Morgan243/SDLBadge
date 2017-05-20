//
// Created by morgan on 4/20/17.
//

#ifndef BASICSDLBADGE_SDL_FB_H_H
#define BASICSDLBADGE_SDL_FB_H_H

#include "SDL2/SDL.h"
struct vector32_t {
    uint32_t x;
    uint32_t y;
};


#define WIDTH 132
#define HEIGHT 132
#define LCD_XSIZE WIDTH
#define LCD_YSIZE HEIGHT

#ifdef SDL_BADGE
#define portTICK_PERIOD_MS 1
#define TickType_t uint32_t
#define vTaskDelay(tick_rate) SDL_Delay(tick_rate*15)
#endif
struct sdl_context
{
    unsigned int width, height;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Surface *surface;

    SDL_Event event;

    struct vector32_t pos;
    uint32_t color,
           BGcolor,
           transMask,
           transIndex;
    unsigned short changed;

    //unsigned char pixels[WIDTH * HEIGHT * 4] = {0};
    unsigned char *buffer;
    unsigned char color_depth;
    unsigned char status;
    SDL_mutex *status_lock;
};

#define SIN(x) sin(x)
#define COS(x) cos(x)
#define PI 3.141
#define TWO_PI 6.282


#define BUFFER_INDEX(CXT, X, Y)  ((CXT.width * CXT.color_depth * Y) + X * CXT.color_depth)
#define SET_GLOBAL_SDL_CXT(SDL_CXT) G_Fb_ptr = SDL_CXT;
#define G_Fb (*G_Fb_ptr)
#define BUFFER(i)  G_Fb.buffer[i]
extern struct sdl_context *G_Fb_ptr;

//offset = ( sdl_cxt->width * 4 * y ) + x * 4;

void rand_pixels(struct sdl_context *sdl_cxt,
                 unsigned int num_pixels);
void rand_pixels_stream(struct sdl_context *sdl_cxt,
                        unsigned int num_pixels,
                        uint8_t red_max,
                        uint8_t green_max,
                        uint8_t blue_max);

void write_buffer_pixel(struct sdl_context *sdl_cxt,
                        unsigned int x, unsigned int y,
                        uint8_t r, uint8_t g, uint8_t b, uint8_t a);

void write_buffer_pixel32(struct sdl_context *sdl_cxt,
                          unsigned int x, unsigned int y,
                          uint32_t color, uint8_t alpha);

int construct_sdl_context(struct sdl_context *sdl_cxt);


void equilateral_polygon_points(short point_arr[][2],
                                float radius,
                                unsigned int n_sides,
                                float rotate_rads);

/*
   Frame buffer
*/
enum {
    FB_UNUSED=0,		/* unused */
    FB_CLEAR,			/* reset LCD */
    FB_BACKGROUND_COLOR,	/* 16 bit background fill color */
    FB_COLOR,			/* 16 bit color */
    FB_TRANSPARENT_MASK,	/* 16 bit transparent/key/mask color */
    FB_MOVE, 			/* (x 8bit, y 8bit) where the next stuff goes */
};

struct vector8_t {
    uint8_t x;
    uint8_t y;
};



struct framebuffer_t {
    unsigned short *buffer;
    struct vector8_t pos;
    unsigned char font;
    unsigned char fontHeight;

    unsigned short color;
    unsigned short BGcolor;
    unsigned short transMask;
    unsigned short transIndex;
    unsigned short changed;
};

//extern struct framebuffer_t G_Fb;

void FbInit() ;
int FbFrameDone();
void FbMove(uint32_t x, uint32_t y);
void FbClear();
void FbColor(uint32_t color);
void FbBackgroundColor(uint32_t color);
void FbTransparency(unsigned short transparencyMask);
void FbSprite(unsigned char picId, unsigned char imageNo);
void FbCharacter(unsigned char charin);
void FbFilledRectangle(uint32_t width, uint32_t height);
void FbPoint(unsigned char x, unsigned char y);
void FbPrintChar(unsigned char charin, unsigned char x, unsigned char y);
void FbHorizontalLine(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2);
void FbVerticalLine(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2);
void FbLine(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1);
void FbWriteLine(unsigned char *string);
void FbRectangle(unsigned char width, unsigned char height);

//TODO: doesn't work!
void FbPushRegion(unsigned int x, unsigned int y,
                  unsigned int width,unsigned int height);

// Connects together a set of (x,y) points
void FbPolygonFromPoints(short points[][2],
                         unsigned char n_points,
                         short center_x,
                         short center_y);

void FbImage1bit(unsigned char assetId, unsigned char seqNum);
void FbImage2bit(unsigned char assetId, unsigned char seqNum);
void FbImage4bit(unsigned char assetId, unsigned char seqNum);
void FbImage8bit(unsigned char assetId, unsigned char seqNum);
void FbSwapBuffers();
void FbPushBuffer();
void LCDInitPins();
void LCDReset();
void LCDBars();
void FbTransparentIndex(unsigned short color);

#endif //BASICSDLBADGE_SDL_FB_H_H
