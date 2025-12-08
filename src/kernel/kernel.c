#include "idt.h"
#include "isr.h"
#include "util.h"
#include "pit.h"
#include "drivers/kbd.h"
#include "drivers/screen.h"

#include "programs/gameoflife/gol.h"
#include "programs/kbdtest/kbdtest.h"

#define SEED 0xBADFACE
#define ON '*'
#define OFF ' '

static char itoa_buf[11];
static char itoh_buf[10];

// u16 data[width][height] - Row major (y, x)
const u16 AtlasData[8][8] = {
  {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
  {0x0000, 0x0000, 0x0000, 0x00a0, 0x00a0, 0x00a0, 0x0000, 0x0000},
  {0x0000, 0x0000, 0x00a0, 0x0000, 0x0000, 0x0000, 0x00a0, 0x0000},
  {0x0000, 0x0000, 0x00a0, 0x0000, 0x0000, 0x0000, 0x00a0, 0x0000},
  {0x0000, 0x0000, 0x00a0, 0x0000, 0x0000, 0x0000, 0x00a0, 0x0000},
  {0x0000, 0x0000, 0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x0000},
  {0x0000, 0x0000, 0x00a0, 0x0000, 0x0000, 0x0000, 0x00a0, 0x0000},
  {0x0000, 0x0000, 0x00a0, 0x0000, 0x0000, 0x0000, 0x00a0, 0x0000},
};

void font_char(char c, u32 x, u32 y, u8 color) {
    const u16 *glyph = AtlasData;

    for (u32 yy = 0; yy < 8; yy++) {
        for (u32 xx = 0; xx < 8; xx++) {
        	screen_putpixel(x + xx, y + yy, AtlasData[yy][xx]);
        }
    }
}

void font_str(const char *s, u32 x, u32 y, u8 color) {
    char c;

    while ((c = *s++) != 0) {
        font_char(c, x, y, color);
        x += 8;
    }
}

void main() {
	SCREEN_INIT();
	IDT_INIT();

	__asm__ volatile("sti");

	PIT_INIT(1000);
	KBD_INIT();

	screen_clear(0);

	while(1) {

		for (int i = 0; i < 320; i++) {
			for (int j = 0; j < 200; j++) {
				screen_putpixel(i, j, rgb(16*((i*j + get_ticks()*10) % (320*200))/(320*200),0,0));
			}
		}
		sleep(1);
	}
}



