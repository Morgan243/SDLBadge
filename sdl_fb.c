//
// Created by morgan on 4/20/17.
//


#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include "SDL2/SDL.h"
#include "sdl_fb.h"
#include "SDL2_gfxPrimitives.h"
#include "SDL2/SDL2_rotozoom.h"
#include "SDL2/SDL_image.h"
#include "colors.h"
#include "assetList.h"

#define uCHAR (unsigned char *)

void write_buffer_pixel(struct sdl_context *sdl_cxt,
                        unsigned int x, unsigned int y,
                        uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    unsigned int offset = 0;
    offset = BUFFER_INDEX((*sdl_cxt), x, y);

    sdl_cxt->buffer[ offset + 0 ] = r;
    sdl_cxt->buffer[ offset + 1 ] = g;
    sdl_cxt->buffer[ offset + 2 ] = b;
    //sdl_cxt->buffer[ offset + 3 ] = SDL_ALPHA_OPAQUE;    // a
    sdl_cxt->buffer[ offset + 3 ] = a;    // a
}

void write_buffer_pixel32(struct sdl_context *sdl_cxt,
                        uint32_t x, uint32_t y,
                        uint32_t color, uint8_t alpha)
                        //uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    uint32_t offset = BUFFER_INDEX((*sdl_cxt), x, y);

    sdl_cxt->buffer[ offset + 0 ] = (uint8_t) ((color & RED)>>24);
    sdl_cxt->buffer[ offset + 1 ] = (uint8_t) ((color & GREEN)>>16);//g;
    sdl_cxt->buffer[ offset + 2 ] = (uint8_t) ((color & BLUE)>>8);//b;
    sdl_cxt->buffer[ offset + 3 ] = alpha;    // a
}

void rand_pixels_stream(struct sdl_context *sdl_cxt,
                        unsigned int num_pixels,
                        uint8_t red_max,
                        uint8_t green_max,
                        uint8_t blue_max)
{
    unsigned int x = 0;
    unsigned int y = 0;
    unsigned int offset = 0;

    for(unsigned int i=0; i < num_pixels; i++)
    {
        x = rand() % sdl_cxt->width;
        y = rand() % sdl_cxt->height;
        write_buffer_pixel(sdl_cxt, x, y,
                           (uint8_t) (rand() % red_max),
                           (uint8_t) (rand() % green_max),
                           (uint8_t) (rand() % blue_max),
                           //SDL_ALPHA_OPAQUE
                           (uint8_t) (cos((float)x/(float)(1.0+y))*100.0)
                            );
    }
    // Does texture need locking?
    // SDL_LockTexture()
    SDL_UpdateTexture
            (
                    sdl_cxt->texture,
                    NULL,
                    &(sdl_cxt->buffer[0]),
                    sdl_cxt->width * sdl_cxt->color_depth
            );

    SDL_RenderCopy( sdl_cxt->renderer, sdl_cxt->texture, NULL, NULL );
}

int construct_sdl_context(struct sdl_context *sdl_cxt)
{
    int bpp = 32;
    //int bpp = 16;

    sdl_cxt->width = WIDTH;
    sdl_cxt->height = HEIGHT;

    //just init the sdl video stuff
    SDL_Init(SDL_INIT_VIDEO);

    sdl_cxt->window = SDL_CreateWindow("Basic Badge",
                                       SDL_WINDOWPOS_UNDEFINED,
                                       SDL_WINDOWPOS_UNDEFINED,
                                       sdl_cxt->width, sdl_cxt->height,
                                       SDL_WINDOW_SHOWN);

    //associate the main window with a new renderer
    sdl_cxt->renderer = SDL_CreateRenderer(sdl_cxt->window,
                                           -1, SDL_RENDERER_ACCELERATED);
    //create the primary streaming texture
    sdl_cxt->texture = SDL_CreateTexture(sdl_cxt->renderer,
                                         SDL_PIXELFORMAT_ARGB8888,
                                        //SDL_PIXELFORMAT_ABGR8888,
                                        SDL_TEXTUREACCESS_STREAMING,
                                        sdl_cxt->width, sdl_cxt->height);

    sdl_cxt->color_depth = 4;
    //create a local buffer
    sdl_cxt->buffer = malloc(sizeof(unsigned char) * (WIDTH * HEIGHT * sdl_cxt->color_depth));

    SDL_SetRenderDrawColor(sdl_cxt->renderer, 0, 0, 0, 255);
    SDL_RenderClear(sdl_cxt->renderer);
    SDL_RenderPresent(sdl_cxt->renderer);

    Uint32 r, g, b, a;
    if(!SDL_PixelFormatEnumToMasks(SDL_PIXELFORMAT_ARGB8888, &bpp,
                                   &r, &g, &b, &a))
    {
        printf("Pixel format thing failed!\n");
        return -1;
    }
    sdl_cxt->pos.x = 0;
    sdl_cxt->pos.y = 0;
    sdl_cxt->changed = 0;

    sdl_cxt->surface = SDL_CreateRGBSurface(0, sdl_cxt->width,
                                            sdl_cxt->height,
                                            bpp, r, g, b, a);

    SDL_SetTextureBlendMode(sdl_cxt->texture,
                            SDL_BLENDMODE_BLEND);

    sdl_cxt->status_lock = SDL_CreateMutex();

    if (SDL_LockMutex(sdl_cxt->status_lock) == 0) {
        sdl_cxt->status = 1;
        SDL_UnlockMutex(sdl_cxt->status_lock);
    } else {
        fprintf(stderr, "Couldn't lock mutex\n");
    }
    return 0;
}


int update_from_texture(struct sdl_context *sdl_cxt)
{
    SDL_UpdateTexture
        (
                sdl_cxt->texture,
                NULL,
                &(sdl_cxt->buffer[0]),
                sdl_cxt->width * sdl_cxt->color_depth
        );
}

//void set_global_sdl_cxt(struct sdl_context *sdl_cxt){

struct sdl_context *G_Fb_ptr;
///////////------
void FbMove(uint32_t x, uint32_t y)
{
    G_Fb.pos.x = x;
    G_Fb.pos.y = y;

    if (G_Fb.pos.x > LCD_XSIZE) G_Fb.pos.x = LCD_XSIZE-1;
    if (G_Fb.pos.y > LCD_YSIZE) G_Fb.pos.y = LCD_YSIZE-1;
}

void FbClear()
{
    G_Fb.pos.x = 0;
    G_Fb.pos.y = 0;
    G_Fb.changed = 1; //lol
    uint32_t x, y;

    for(x=0; x < G_Fb.width; x++){
        for(y=0; y < G_Fb.height; y++){
            write_buffer_pixel32(&G_Fb, x, y,
                                 G_Fb.BGcolor, SDL_ALPHA_OPAQUE);
        }
    }

    SDL_SetRenderDrawColor(G_Fb.renderer,
                          (uint8_t) ((G_Fb.BGcolor & RED)>>24),
                          (uint8_t) ((G_Fb.BGcolor & GREEN)>>16),
                          (uint8_t) ((G_Fb.BGcolor & BLUE)>>8),
                          SDL_ALPHA_OPAQUE);
    SDL_RenderClear(G_Fb.renderer);
}

void FbColor(uint32_t color)
{
    G_Fb.color = color;
}

void FbBackgroundColor(uint32_t color)
{
    G_Fb.BGcolor = color;
}

void FbTransparentIndex(unsigned short color)
{
    G_Fb.transIndex = color;
}

void FbImage8bit(unsigned char assetId, unsigned char seqNum)
{
    unsigned char y, yEnd, x;
    unsigned char *pixdata, pixbyte, ci, *cmap, r, g, b;
    unsigned short pixel;

    /* clip to end of LCD buffer */
    yEnd = G_Fb.pos.y + assetList[assetId].y;
    if (yEnd > LCD_YSIZE) yEnd = LCD_YSIZE-1;

    for (y = G_Fb.pos.y; y < yEnd; y++) {
        pixdata = uCHAR(&(assetList[assetId].pixdata[ (y - G_Fb.pos.y) * assetList[assetId].x + seqNum * assetList[assetId].x * assetList[assetId].y]));

        for (x = 0; x < assetList[assetId].x; x++) {
            if ((x + G_Fb.pos.x) >= LCD_XSIZE) continue; /* clip x */

            pixbyte = *pixdata; /* 1 pixel per byte */

            ci = pixbyte;
            if (ci != G_Fb.transIndex) { /* transparent? */
                cmap = uCHAR(&(assetList[assetId].data_cmap[ci * 3]));

                r = cmap[0];
                g = cmap[1];
                b = cmap[2];

                pixel = ((((r >> 3) & 0b11111) << 11 )
                         |  (((g >> 3) & 0b11111) <<  6 )
                         |  (((b >> 3) & 0b11111)       )) ;

                if (G_Fb.transMask > 0)
                    BUFFER(y * LCD_XSIZE + x + G_Fb.pos.x) = (BUFFER(x + G_Fb.pos.x) & (~G_Fb.transMask)) | (pixel & G_Fb.transMask);
                else
                    BUFFER(y * LCD_XSIZE + x + G_Fb.pos.x) = pixel;
            }
            pixdata++;
        }
    }
    G_Fb.changed = 1;
}

//#define REDSHIFT 11
//#define GREENSHIFT 5
#define REDSHIFT 16
#define GREENSHIFT 8

void FbImage4bit(unsigned char assetId, unsigned char seqNum)
{
    unsigned char y, yEnd, x;
    unsigned char *pixdata, pixbyte, ci, *cmap, r, g, b;
    unsigned short pixel;

    /* clip to end of LCD buffer */
    yEnd = G_Fb.pos.y + assetList[assetId].y;
    if (yEnd >= LCD_YSIZE) yEnd = LCD_YSIZE-1;

    for (y = G_Fb.pos.y; y < yEnd; y++) {
        pixdata = uCHAR(&(assetList[assetId].pixdata[ (y - G_Fb.pos.y) * (assetList[assetId].x >> 1) + seqNum * (assetList[assetId].x >> 1) * assetList[assetId].y]));

        for (x = 0; x < (assetList[assetId].x); /* manual inc */ ) {
            pixbyte = *pixdata++; /* 2 pixels per byte */

            /* 1st pixel */
            if ((x + G_Fb.pos.x) > (LCD_XSIZE-1)) continue; /* clip x */
            // Pull first byte
            ci = ((pixbyte >> 4) & 0xF);
            if (ci != G_Fb.transIndex) { /* transparent? */
                // Each CMAP entry is 3 colors, so index is ci * 3
                cmap = uCHAR(&(assetList[assetId].data_cmap[ci * 3]));

                r = cmap[0];
                g = cmap[1];
                b = cmap[2];

                pixel = ((((r >> 3) & 0b11111) << REDSHIFT )
                         |  (((g >> 3) & 0b11111) <<  GREENSHIFT )
                         |  (((b >> 3) & 0b11111)       )) ;

                /* G_Fb.pos.x == offset into scan buffer */
                if (G_Fb.transMask > 0)
                    BUFFER(y * LCD_XSIZE + x + G_Fb.pos.x) = (BUFFER(x + G_Fb.pos.x) & (~G_Fb.transMask)) | (pixel & G_Fb.transMask);
                else
                    BUFFER(y * LCD_XSIZE + x + G_Fb.pos.x) = pixel;
            }
            x++;

            /* 2nd pixel */
            if ((x + G_Fb.pos.x) > (LCD_XSIZE-1)) continue; /* clip x */

            ci = pixbyte & 0xF;
            if (ci != G_Fb.transIndex) { /* transparent? */
                cmap = uCHAR(&(assetList[assetId].data_cmap[ci * 3]));

                r = cmap[0];
                g = cmap[1];
                b = cmap[2];

                pixel = ((((r >> 3) & 0b11111) << REDSHIFT )
                         |  (((g >> 3) & 0b11111) <<  GREENSHIFT )
                         |  (((b >> 3) & 0b11111)       )) ;

                /* G_Fb.pos.x == offset into scan buffer */
                if (G_Fb.transMask > 0)
                    BUFFER(y * LCD_XSIZE + x + G_Fb.pos.x) = (BUFFER(x + G_Fb.pos.x) & (~G_Fb.transMask)) | (pixel & G_Fb.transMask);
                else
                    BUFFER(y * LCD_XSIZE + x + G_Fb.pos.x) = pixel;
            }
            x++;
        }
    }
    G_Fb.changed = 1;
}

void FbImage2bit(unsigned char assetId, unsigned char seqNum)
{
    unsigned char y, yEnd, x;
    unsigned char *pixdata, pixbyte, ci, *cmap, r, g, b;
    unsigned short pixel;

    /* clip to end of LCD buffer */
    yEnd = G_Fb.pos.y + assetList[assetId].y;
    if (yEnd > LCD_YSIZE) yEnd = LCD_YSIZE-1;

    for (y = G_Fb.pos.y; y < yEnd; y++) {
        pixdata = uCHAR(&(assetList[assetId].pixdata[ (y - G_Fb.pos.y) * (assetList[assetId].x >> 2) + seqNum * (assetList[assetId].x >> 2) * assetList[assetId].y]));

        for (x = 0; x < (assetList[assetId].x); /* manual inc */) {
            pixbyte = *pixdata++; /* 4 pixels per byte */

            /* ----------- 1st pixel ----------- */
            if ((x + G_Fb.pos.x) > (LCD_XSIZE-1)) continue; /* clip x */

            ci = ((pixbyte >> 6) & 0x3);
            if (ci != G_Fb.transIndex) { /* transparent? */
                cmap = uCHAR(&(assetList[assetId].data_cmap[ci * 3]));

                r = cmap[0];
                g = cmap[1];
                b = cmap[2];

                pixel = ((((r >> 3) & 0b11111) << 11 )
                         |  (((g >> 3) & 0b11111) <<  6 )
                         |  (((b >> 3) & 0b11111)       )) ;

                /* G_Fb.pos.x == offset into scan buffer */
                if (G_Fb.transMask > 0)
                    BUFFER(y * LCD_XSIZE + x + G_Fb.pos.x) = (BUFFER(x + G_Fb.pos.x) & (~G_Fb.transMask)) | (pixel & G_Fb.transMask);
                else
                    BUFFER(y * LCD_XSIZE + x + G_Fb.pos.x) = pixel;
            }
            x++;

            /* ----------- 2nd pixel ----------- */
            if ((x + G_Fb.pos.x) > (LCD_XSIZE-1)) continue; /* clip x */

            ci = ((pixbyte >> 4) & 0x3);
            if (ci != G_Fb.transIndex) { /* transparent? */
                cmap = uCHAR(&(assetList[assetId].data_cmap[ci * 3]));

                r = cmap[0];
                g = cmap[1];
                b = cmap[2];

                pixel = ((((r >> 3) & 0b11111) << 11 )
                         |  (((g >> 3) & 0b11111) <<  6 )
                         |  (((b >> 3) & 0b11111)       )) ;

                /* G_Fb.pos.x == offset into scan buffer */
                if (G_Fb.transMask > 0)
                    BUFFER(y * LCD_XSIZE + x + G_Fb.pos.x) = (BUFFER(x + G_Fb.pos.x) & (~G_Fb.transMask)) | (pixel & G_Fb.transMask);
                else
                    BUFFER(y * LCD_XSIZE + x + G_Fb.pos.x) = pixel;
            }
            x++;

            /* ----------- 3rd pixel ----------- */
            if ((x + G_Fb.pos.x) > (LCD_XSIZE-1)) continue; /* clip x */

            ci = ((pixbyte >> 2) & 0x3);
            if (ci != G_Fb.transIndex) { /* transparent? */
                cmap = uCHAR(&(assetList[assetId].data_cmap[ci * 3]));

                r = cmap[0];
                g = cmap[1];
                b = cmap[2];

                pixel = ((((r >> 3) & 0b11111) << 11 )
                         |  (((g >> 3) & 0b11111) <<  6 )
                         |  (((b >> 3) & 0b11111)       )) ;

                /* G_Fb.pos.x == offset into scan buffer */
                if (G_Fb.transMask > 0)
                    BUFFER(y * LCD_XSIZE + x + G_Fb.pos.x) = (BUFFER(x + G_Fb.pos.x) & (~G_Fb.transMask)) | (pixel & G_Fb.transMask);
                else
                    BUFFER(y * LCD_XSIZE + x + G_Fb.pos.x) = pixel;
            }
            x++;

            /* ----------- 4th pixel ----------- */
            if ((x + G_Fb.pos.x) > (LCD_XSIZE-1)) continue; /* clip x */

            ci = ((pixbyte) & 0x3);
            if (ci != G_Fb.transIndex) { /* transparent? */
                cmap = uCHAR(&(assetList[assetId].data_cmap[ci * 3]));

                r = cmap[0];
                g = cmap[1];
                b = cmap[2];

                pixel = ((((r >> 3) & 0b11111) << 11 )
                         |  (((g >> 3) & 0b11111) <<  6 )
                         |  (((b >> 3) & 0b11111)       )) ;

                /* G_Fb.pos.x == offset into scan buffer */
                if (G_Fb.transMask > 0)
                    BUFFER(y * LCD_XSIZE + x + G_Fb.pos.x) = (BUFFER(x + G_Fb.pos.x) & (~G_Fb.transMask)) | (pixel & G_Fb.transMask);
                else
                    BUFFER(y * LCD_XSIZE + x + G_Fb.pos.x) = pixel;
            }
            x++;
        }
    }
    G_Fb.changed = 1;
}

void FbImage1bit(unsigned char assetId, unsigned char seqNum)
{
    unsigned char y, yEnd, x;
    unsigned char *pixdata, pixbyte, ci;

    yEnd = G_Fb.pos.y + assetList[assetId].y;
    /* clip to end of LCD buffer */
    if (yEnd >= LCD_YSIZE) yEnd = LCD_YSIZE-1;

    for (y=G_Fb.pos.y; y < yEnd; y++) {
        pixdata = uCHAR(&(assetList[assetId].pixdata[ seqNum * (assetList[assetId].x >> 3) * assetList[assetId].y + (y - G_Fb.pos.y) * (assetList[assetId].x >> 3)]));

        for (x=0; x < (assetList[assetId].x); x += 8) {
            unsigned char bit;

            pixbyte = *pixdata++;

            for (bit=0; bit < 8; bit++) { /* 8 pixels per byte */
                if ((bit + G_Fb.pos.x) > (LCD_XSIZE-1)) continue; /* clip x */

                ci = ((pixbyte >> bit) & 0x1); /* ci = color index */
                if (ci != G_Fb.transIndex) { // transparent?
                    if (ci == 0) {
                        if (G_Fb.transMask > 0)
                            BUFFER(y * LCD_XSIZE + x + G_Fb.pos.x + bit) = (BUFFER(y * LCD_XSIZE + x + G_Fb.pos.x + bit) & (~G_Fb.transMask)) | (G_Fb.BGcolor & G_Fb.transMask);
                        else
                            BUFFER(y * LCD_XSIZE + x + G_Fb.pos.x + bit) = G_Fb.BGcolor;
                    } else {
                        if (G_Fb.transMask > 0)
                            BUFFER(y * LCD_XSIZE + x + G_Fb.pos.x + bit) = (BUFFER(y * LCD_XSIZE + x + G_Fb.pos.x + bit) & (~G_Fb.transMask)) | (G_Fb.color & G_Fb.transMask);
                        else
                            BUFFER(y * LCD_XSIZE + x + G_Fb.pos.x + bit) = G_Fb.color;
                    }
                }
            }
        }
    }
    G_Fb.changed = 1;
}



void FbImage(unsigned char assetId, unsigned char seqNum){
    //printf("FbImage not implemented in SDL Badge");
    switch (assetList[assetId].type) {
        case PICTURE1BIT:
            FbImage1bit(assetId, seqNum);
            break;

        case PICTURE2BIT:
            FbImage2bit(assetId, seqNum);
            break;

        case PICTURE4BIT:
            FbImage4bit(assetId, seqNum);
            break;

        case PICTURE8BIT:
            FbImage8bit(assetId, seqNum);
            break;

        default:
            break;
    }
}

void FbFilledRectangle(uint32_t width, uint32_t height)
{
    uint32_t y, x, endX, endY;

    endX = G_Fb.pos.x + width;
    if (endX >= LCD_XSIZE) endX = LCD_XSIZE;

    endY = G_Fb.pos.y + height;
    if (endY >= LCD_YSIZE) endY = LCD_YSIZE;

    for (y=G_Fb.pos.y; y < endY; y++) {
        for (x=G_Fb.pos.x; x < endX; x++) {
            printf("%u,%u\n", x, y);
            write_buffer_pixel32(&G_Fb, x, y,
                                 G_Fb.color, SDL_ALPHA_OPAQUE);
        }
    }
    FbMove(endX, endY);
    G_Fb.changed = 1;
}

void FbPoint(unsigned char x, unsigned char y)
{
    if (x >= LCD_XSIZE) x = LCD_XSIZE;
    if (y >= LCD_YSIZE) y = LCD_YSIZE;

    //BUFFER(y * LCD_XSIZE + x) = G_Fb.color;
    //BUFFER((BUFFER_INDEX(G_Fb, x, y))) = G_Fb.color;
    write_buffer_pixel32(&G_Fb, x, y,
                         G_Fb.color, SDL_ALPHA_OPAQUE);

    FbMove(x, y);
    G_Fb.changed = 1;
}

void FbHorizontalLine(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2)
{
    unsigned char x;

    FbMove(x1, y1);
    for (x=x1; x<x2; x++) FbPoint(x, y1);

    G_Fb.changed = 1;
}

void FbVerticalLine(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2)
{
    unsigned char y;

    FbMove(x1, y1);
    for (y=y1; y<y2; y++) FbPoint(x1, y);
    G_Fb.changed = 1;
}

void FbLine(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy)/2, e2;

    for(;;) {
        FbPoint(x0, y0); /* optimise this: join multiple y==y points into one segments */

        if (x0==x1 && y0==y1) break;

        e2 = err;
        if (e2 > -dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
    G_Fb.changed = 1;
}

void FbCharacter(unsigned char charin){
    printf("FbCharacter not implemented in SDL Badge\n");

}

void FbWriteLine(unsigned char *string)
{
    unsigned char j, x, y;

    //debug("FbWriteLine ");

    x = G_Fb.pos.x;
    y = G_Fb.pos.y;

    for(j=0; string[j] != 0; j++) {
        //FbMove(x + j * assetList[G_Fb.font].x, y);
        //FbCharacter(string[j]);
    }
    G_Fb.changed = 1;

}

void FbRectangle(unsigned char width, unsigned char height)
{
    uint32_t x, y;

    x = G_Fb.pos.x;
    y = G_Fb.pos.y;
    FbVerticalLine(x,	 y, x,	 y + height);
    FbVerticalLine(x + width, y, x + width, y + height);

    FbHorizontalLine(x, y,	  x + width, y	 );
    FbHorizontalLine(x, y + height, x + width, y + height);
    G_Fb.changed = 1;
}

/*
  doesn't currently swap buffers, just flushes ram->LCD
  could do double buffer with 4,2 or 1 bit per pixel
*/
void FbSwapBuffers()
{
    unsigned int i, j;

    //debug("FbWriteLine");

    if (G_Fb.changed == 0) return;

    update_from_texture(&G_Fb);

    SDL_RenderCopy( G_Fb.renderer, G_Fb.texture, NULL, NULL );

    SDL_RenderPresent(G_Fb.renderer);

    //S6B33_rect(0, 0, LCD_XSIZE-1, LCD_YSIZE-1);
    for (i=0; i<LCD_XSIZE; i++) {
        for(j=0; j<LCD_YSIZE; j++){
            BUFFER(BUFFER_INDEX(G_Fb, i, j)) = 0;
        }
    }
    //update_from_texture(&G_Fb);
    FbClear();
        //S6B33_pixel(BUFFER(i));
        //BUFFER(i) = G_Fb.BGcolor; /* clear buffer as we go */
    //}
    G_Fb.changed = 0;

    G_Fb.pos.x = 0;
    G_Fb.pos.y = 0;
}

void FbPushBuffer()
{
       unsigned int i, j;

    //debug("FbWriteLine");

    if (G_Fb.changed == 0) return;

    update_from_texture(&G_Fb);

    SDL_RenderCopy( G_Fb.renderer, G_Fb.texture, NULL, NULL );

    SDL_RenderPresent(G_Fb.renderer);

    //S6B33_rect(0, 0, LCD_XSIZE-1, LCD_YSIZE-1);
    //for (i=0; i<LCD_XSIZE; i++) {
    //    for(j=0; j<LCD_YSIZE; j++){
    //        BUFFER(BUFFER_INDEX(G_Fb, i, j)) = 0;
    //    }
    //}
    //update_from_texture(&G_Fb);
    FbClear();
        //S6B33_pixel(BUFFER(i));
        //BUFFER(i) = G_Fb.BGcolor; /* clear buffer as we go */
    //}
    G_Fb.changed = 0;

    G_Fb.pos.x = 0;
    G_Fb.pos.y = 0;

//    unsigned int i;
//
//    //debug("FbWriteLine");
//
//    if (G_Fb.changed == 0) return;
//
//    S6B33_rect(0, 0, LCD_XSIZE-1, LCD_YSIZE-1);
//    for (i=0; i<(LCD_XSIZE * LCD_YSIZE); i++) {
//        S6B33_pixel(BUFFER(i));
//        //BUFFER(i) = G_Fb.BGcolor; /* clear buffer as we go */
//    }
//    G_Fb.changed = 0;
//
//    G_Fb.pos.x = 0;
//    G_Fb.pos.y = 0;
}

void FbDrawVectors(short points[][2],
                   unsigned char n_points,
                   short center_x,
                   short center_y,
                   unsigned char connect_last_to_first)
{
    unsigned char n = 0;
    short x0, y0, x1, y1;
    for(n=0; n < (n_points-1); n++)
    {
        x0 = points[n][0] + center_x;
        y0 = points[n][1] + center_y;
        x1 = points[n+1][0] + center_x;
        y1 = points[n+1][1] + center_y;
        // Don't bother with the line if
        // either point is out of bounds
        if(!(
                (x0 < 1) || (x0 > 131)
                || (x1 < 1) || (x1 > 131)
                || (y0 < 1) || (y0 > 131)
                || (y1 < 1) || (y1 > 131)
        ))
            FbLine((unsigned char)x0, (unsigned char)y0,
                   (unsigned char)x1, (unsigned char)y1);
    }

    if(connect_last_to_first){
        x0 = points[n_points-1][0] + center_x;
        y0 = points[n_points-1][1] + center_y;
        x1 = points[0][0] + center_x;
        y1 = points[0][1] + center_y;

        if(!(
                (x0 < 0) || (x0 > 132)
                || (x1 < 0) || (x1 > 132)
                || (y0 < 0) || (y0 > 132)
                || (y1 < 0) || (y1 > 132)
        ))
            FbLine((unsigned char)x0, (unsigned char)y0,
                   (unsigned char)x1, (unsigned char)y1);
    }
}

void FbPolygonFromPoints(short points[][2],
                         unsigned char n_points,
                         short center_x,
                         short center_y)
{

    FbDrawVectors(points, n_points, center_x, center_y, 1);
}


