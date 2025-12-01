#include "drivers/screen.h"
#include "idt.h"
#include "isr.h"
#include "util.h"
#include "pit.h"
#include "drivers/kbd.h"

#include "programs/gameoflife/gol.h"
#include "programs/kbdtest/kbdtest.h"

#define SEED 0xBADFACE
#define ON '*'
#define OFF ' '

static char itoa_buf[11];
static char itoh_buf[10];


void main() {
	SCREEN_INIT();
	IDT_INIT();

	__asm__ volatile("sti");

	PIT_INIT(1000);
	KBD_INIT();

	PROGRAM_KBDTEST_MAIN();

}


