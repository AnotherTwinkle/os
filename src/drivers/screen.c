/*
VGA Screen Driver
=================

Setups up the screen pallete, And provides fundamental screen operations.

The driver assumes a 320x200x256 VGA screen. Each color is represented by a byte, giving us 256 possible colors.

Color mapping - RRRGGGBB
*/
#include <kernel/io.h>
#include "screen.h"

void SCREEN_INIT() {  
    outportb(0x3C6, 0xFF);
    outportb(0x3C8, 0);
    for (u8 i = 0; i < 255; i++) {
        outportb(0x3C9, (((i >> 5) & 0x7) * (256 / 8)) / 4);
        outportb(0x3C9, (((i >> 2) & 0x7) * (256 / 8)) / 4);
        outportb(0x3C9, (((i >> 0) & 0x3) * (256 / 4)) / 4);
    }

    // set color 255 = white
    outportb(0x3C9, 0x3F);
    outportb(0x3C9, 0x3F);
    outportb(0x3C9, 0x3F);
}

void screen_putpixel(int x, int y, u8 color) {
    if ((unsigned)x >= SCREEN_WIDTH || (unsigned)y >= SCREEN_HEIGHT)
        return;

    u8 *vga = (u8*)SCREEN_ADDR;
    vga[y * SCREEN_WIDTH + x] = color;
}

void screen_clear(u8 color) {
    u8 *vga = (u8*)SCREEN_ADDR;
    for (int i = 0; i < SCREEN_SIZE; i++)
        vga[i] = color;
}

void screen_flush(u8 *buff) {
    u8 *vga = (u8*)SCREEN_ADDR;
    for (int i = 0; i < SCREEN_SIZE; i++) {
        vga[i] = buff[i];
    }
}

u8 rgb(u8 r, u8 g, u8 b) {
    return ((r & 0x7) << 5) | ((g & 0x7) << 2) | (b & 0x3);
}
