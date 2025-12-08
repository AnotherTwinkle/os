#include "gol.h"
#include "drivers/ttylegacy.h"
#include "kernel/util.h"
#include "kernel/pit.h"

#define SEED 0xBADFACE
#define ON '*'
#define OFF ' '

static char ibuf[TTY_MAXR][TTY_MAXC];
static char next[TTY_MAXR][TTY_MAXC];

static char int_print_buffer[12];

void PROGRAM_GOL_MAIN() {
	srand(SEED + get_ticks());

	init_board();

	u32 INIT_TIME = get_ticks();
	u32 CUR_TIME;
	
	tty_flush((char*) ibuf);

	while(1) {
		sleep(500);
		conway_step();
		CUR_TIME = get_ticks();

		if ((CUR_TIME - INIT_TIME) >= 3*60*1000) {
			// guess it's not too interesting after 3 minutes
			INIT_TIME = CUR_TIME = get_ticks();
			init_board();		
		}
		tty_flush((char*) ibuf);
	}
}

void init_board() {
	for (int r = 0; r < TTY_MAXR; r++) {
		for (int c = 0; c < TTY_MAXC; c++) {
			ibuf[r][c] = (rand() % 100 < 30) ? ON : OFF;
		}
	}
}

int ia(int r, int c) {
	return r >= 0 && r < TTY_MAXR && c >= 0 && c < TTY_MAXC 
	       && ibuf[r][c] == ON;
}

void conway_step() {
	memcpy((char*)next, (char*)ibuf, TTY_MAXR*TTY_MAXC);

	for (int r = 0; r < TTY_MAXR; r++) {
	    for (int c = 0; c < TTY_MAXC; c++) {
	        int neigh = ia(r + 1, c - 1) + ia(r + 1, c) + ia(r + 1, c + 1)
	                  + ia(r, c - 1) + ia(r, c + 1)
	                  + ia(r - 1, c - 1) + ia(r - 1, c) + ia(r - 1, c + 1);

	        if (ibuf[r][c] == OFF) {
	            if (neigh == 3) {
	                next[r][c] = ON;
	            } else {
	                next[r][c] = OFF;
	            }
	        } else {
	            if (neigh < 2 || neigh > 3) {
	                next[r][c] = OFF;
	            } else {
	                next[r][c] = ON;
	            }
	        }
	    }
	}
	memcpy((char*)ibuf, (char*)next,  TTY_MAXR*TTY_MAXC);
}