/*
VGA 80x25 TTY Screen Driver
===========================

Kept for legacy purposes.

*/

#include "ttylegacy.h"
#include "kernel/util.h"
#include "kernel/io.h"

void TTY_SCREEN_INIT() {
	set_tty_cursor(0);
	tty_clear_screen();
	return;
}

int get_tty_offset(int row, int col) {
	return 2*(row*TTY_MAXC + col);
}

int get_tty_cursor() {
	// reg 14 : high byte, reg 15 : low byte
	int offset;
	outportb(REG_TTY_SCREEN_CTRL, 14);
	offset =  inportb(REG_TTY_SCREEN_DATA) << 8;

	outportb(REG_TTY_SCREEN_CTRL, 15);
	offset |= inportb(REG_TTY_SCREEN_DATA);
	return offset * 2;
}

void set_tty_cursor(int offset) {
	offset /= 2;
	outportb(REG_TTY_SCREEN_CTRL, 14);
	outportb(REG_TTY_SCREEN_DATA, (unsigned char)(offset >> 8));

	outportb(REG_TTY_SCREEN_CTRL, 15);

	offset &= 0x0000ffff;
	outportb(REG_TTY_SCREEN_DATA, (unsigned char)offset); 
}

	
void tty_print_char(char c, int row, int col, char attr) {
	unsigned char *vidmem = (unsigned char*) TTY_VIDEO_ADDRESS;
	int offset;

	if (attr == 0) {
		attr = TTY_WHITE_ON_BLACK;
	}

	if (row < 0 || col < 0) {
		offset = get_tty_cursor();
	} else {
		offset = get_tty_offset(row, col);
	}

	// Recompute if not provided
	row = offset / (2*TTY_MAXC);
	col =  (offset - 2*row*TTY_MAXC) / 2;

	// Control characters
	if (c == '\n') {
		col = TTY_MAXC - 1; // go to end of current row
	} else if (c == '\r') {
		row -= 1, col = TTY_MAXC - 1; // end of previous row
	} else if (c == '\b') {
		if (row == 0 && col == 0) return;

		// Our cursor is now at the position of the  next write

		// Get the previous position
		int prow = row;
		int pcol = col;

		if (col == 0) {
			// Go to the end of preivous row
			prow--;
			pcol = TTY_MAXC - 1;

		} else {
			// Row remains unchanged
			pcol--;
		}

		int poffset = get_tty_offset(prow, pcol);
		// Change the character there
		vidmem[poffset] = 0x0;
		while (poffset >= 0 && vidmem[poffset] == 0x0) poffset -= 2;
		// println(itoa(poffset));
		poffset += 2;
		set_tty_cursor(tty_handle_scroll(poffset));
		return; 

		// This is terrible backspace handling and will be fixed
		// when I plan out a seperate text output system
		// for the pixel display driver

	} else {
		vidmem[offset] = c;
		vidmem[offset + 1] = attr;
	}

	offset = get_tty_offset(row, col);
	offset += 2;
	
	// will handle cursor position
	set_tty_cursor(tty_handle_scroll(offset));
}

void tty_print_at(int row, int col, char* msg) {
	// Terminate if empty string 
	if (msg[0] == '\0') return;

	// Print first character at [row, col]. This also sets up the cursor
	tty_print_char(msg[0], row, col, 0);
	for (int i = 1; msg[i] != '\0'; i++) {
		tty_print_char(msg[i], -1, -1, 0);
	}
}

void tty_print(char *msg) {
	tty_print_at(-1, -1, msg);
}

void tty_println(char* msg) {
	tty_print(msg);
	tty_print("\n");
}

void tty_clear_screen() {
	for (int r = 0; r < TTY_MAXR; r++) {
		for (int c = 0; c < TTY_MAXC; c++) {
			tty_print_char(' ', r, c, TTY_WHITE_ON_BLACK);
		}
	}

	set_tty_cursor(get_tty_offset(0, 0));
}

void tty_scroll_up() {
	// get current row from cursor offset
	int offset = get_tty_cursor();
	int row = offset / (2*TTY_MAXC);

	// copy memory region from line 1 to the end of current row
	// to line 0
	int start_addr = TTY_VIDEO_ADDRESS + 2*TTY_MAXC;
	int end_addr = TTY_VIDEO_ADDRESS + get_tty_offset(row, TTY_MAXC - 1);

	if (start_addr < end_addr) {
		memcpy(
			(char*)TTY_VIDEO_ADDRESS, 
			(char*)start_addr,
			end_addr - start_addr + 1
			);
	}
	// now clear the current row
	memset((char*)TTY_VIDEO_ADDRESS + get_tty_offset(row, 0), 0, 2*TTY_MAXC);
}

int tty_handle_scroll(int offset) {
	if (offset < 2 * TTY_MAXC * TTY_MAXR) {
		return offset;
	}

	tty_scroll_up();
	offset -= 2*TTY_MAXC;
	return offset;
}

void tty_flush(char *buff) { // Note : accepts flattened array
	unsigned char *vidmem = (unsigned char*) TTY_VIDEO_ADDRESS;

	// Flush screen without cursor shenanighans
	for (int r = 0; r < TTY_MAXR; r++) {
		for (int c = 0; c < TTY_MAXC; c++) {
			int offset = get_tty_offset(r, c);
			char c = buff[offset/2];
			vidmem[offset] = c;
			vidmem[offset + 1] = TTY_WHITE_ON_BLACK;
		}
	}
}