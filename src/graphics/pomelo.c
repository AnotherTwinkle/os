/*
PomeloGL, Graphics Library for Bakshobondi
Started on : 08/12/2025
*/

#include "pomelo.h"
#include "drivers/screen.h"

u8 *CUR_BUFFER;
u16 BUFFER_WIDTH;
u16 BUFFER_HEIGHT;

void pml_setbuffer(u8 *buff, u16 buff_width, u16 buff_height) {
	CUR_BUFFER = buff;
	BUFFER_WIDTH = buff_width;
	BUFFER_HEIGHT = buff_height;
}


void pml_setpixel(int x, int y, u8 color) {
    if (x >= BUFFER_WIDTH || y >= BUFFER_HEIGHT)
        return;

    if (x < 0 || y < 0) {
    	return;
    }

    if (color == EMPTY_PIXEL) {
    	return;
    }

    CUR_BUFFER[y * BUFFER_WIDTH + x] = color;
}

u8 pml_getpixel(int x, int y) {
	if (x > BUFFER_WIDTH || y > BUFFER_HEIGHT) {
		return 0x0;
	}

	if (x < 0 || y < 0) {
		return 0x0;
	}

	return CUR_BUFFER[y * BUFFER_WIDTH + x];
}

void pml_draw_rect(int x, int y, int w, int h, u8 color) {
	for (int xx = x; xx < (x + w); xx++) {
		for (int yy = y; yy < (y + h); yy++) {
			pml_setpixel(xx, yy, color);
		}
	}
}

void pml_draw_rect_ca(int x, int y, int w, int h, u8 color) {
	int adj_x = x - w/2;
	int adj_y = y - h/2;

	pml_draw_rect(adj_x, adj_y, w, h, color);
}

void pml_draw_sprite(SpriteSheet *sheet, int idx, int x, int y, int scale) {
	int width = sheet->width;
	int height = sheet->height;
	int unit_width = sheet->unit_width;
	int unit_height = sheet->unit_height;

	int sprite_x = (idx % (width/unit_width)) * unit_width;
	int sprite_y = (idx / (width/unit_width)) * unit_height;

	int sx = sprite_x;
	for (int xx = x; xx < (x + unit_width * scale); xx += scale, sx++) {
		for (int yy = y, sy = sprite_y; yy < (y + unit_height * scale); yy += scale, sy++) {
			u8 color = sheet->data[sy * width + sx];
			pml_draw_rect(xx, yy, scale, scale, color);

			
			// if (xx == (x + (unit_width-1) * scale) || yy == (y + (unit_height-1) * scale) ||
			// 	xx == x || yy == y) pml_draw_rect(xx, yy, scale, scale, BLACK);
		}
	}
}

void pml_draw_sprite_ca(SpriteSheet *sheet, int idx, int x, int y, int scale) {
    int unit_width = sheet->unit_width;
    int unit_height = sheet->unit_height;

    // Adjust x and y to the top-left corner for centering
    int adj_x = x - (unit_width * scale) / 2;
    int adj_y = y - (unit_height * scale) / 2;

    pml_draw_sprite(sheet, idx, adj_x, adj_y, scale);
}
