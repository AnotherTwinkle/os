#ifndef POMELO_H
#define POMELO_H

#include "kernel/util.h"

#define EMPTY_PIXEL 0x0 
#define BLACK 0x20

typedef struct SpriteSheet {
	u32 width;
	u32 height;
	u32 unit_width;
	u32 unit_height;
	u8 *data;
} SpriteSheet;

/*
This kind of sucks... this means we cannot ever have true black in sprites.
The "closest" black we can have is 0x20, which is kind of reddish.
*/

void pml_setbuffer(u8 *buff, u16 buff_width, u16 buff_height);
void pml_setpixel(int x, int y, u8 color);
u8 pml_getpixel(int x, int y);

void pml_draw_rect(int x, int y, int w, int h, u8 color);
void pml_draw_rect_ca(int x, int y, int w, int h, u8 color);

void pml_draw_sprite(SpriteSheet *sheet, int idx, int x, int y, int scale);
void pml_draw_sprite_ca(SpriteSheet *sheet, int idx, int x, int y, int scale);

void pml_draw_sprite_colored(SpriteSheet *sheet, int idx, int x, int y, int scale, u8 color);
void pml_draw_sprite_ca_colored(SpriteSheet *sheet, int idx, int x, int y, int scale, u8 color);
#endif