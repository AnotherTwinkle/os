#include <kernel/util.h>

#define SCREEN_WIDTH   320
#define SCREEN_HEIGHT  200
#define SCREEN_SIZE    SCREEN_WIDTH*SCREEN_HEIGHT
#define SCREEN_ADDR    0xA0000

void SCREEN_INIT();
void screen_set_palette(u8 index, u8 r, u8 g, u8 b);
void screen_clear(u8 color);
void screen_putpixel(int x, int y, u8 color);

u8 rgb(u8 r, u8 g, u8 b);