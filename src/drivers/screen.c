//  Screen driver functionality

#include "screen.h"
#include "kernel/util.h"
#include "kernel/io.h"

void SCREEN_INIT() {
	set_cursor(0);
	return;
}

int get_offset(int row, int col) {
	return 2*(row*MAXC + col);
}

int get_cursor() {
	// reg 14 : high byte, reg 15 : low byte
	int offset;
	port_byte_out(REG_SCREEN_CTRL, 14);
	offset =  port_byte_in(REG_SCREEN_DATA) << 8;

	port_byte_out(REG_SCREEN_CTRL, 15);
	offset |= port_byte_in(REG_SCREEN_DATA);
	return offset * 2;
}

void set_cursor(int offset) {
	offset /= 2;
	port_byte_out(REG_SCREEN_CTRL, 14);
	port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset >> 8));

	port_byte_out(REG_SCREEN_CTRL, 15);

	offset &= 0x0000ffff;
	port_byte_out(REG_SCREEN_DATA, (unsigned char)offset); 
}

	
void print_char(char c, int row, int col, char attr) {
	unsigned char *vidmem = (unsigned char*) VIDEO_ADDRESS;
	int offset;

	if (attr == 0) {
		attr = WHITE_ON_BLACK;
	}

	if (row < 0 || col < 0) {
		offset = get_cursor();
	} else {
		offset = get_offset(row, col);
	}

	// Recompute if not provided
	row = offset / (2*MAXC);
	col =  (offset - 2*row*MAXC) / 2;

	// Control characters
	if (c == '\n') {
		col = MAXC - 1; // go to end of current row
	} else if (c == '\r') {
		row -= 1, col = MAXC - 1; // end of previous row
	} else if (c == '\b') {
		if (row == 0 && col == 0) return;

		// Our cursor is now at the position of the  next write

		// Get the previous position
		int prow = row;
		int pcol = col;

		if (col == 0) {
			// Go to the end of preivous row
			prow--;
			pcol = MAXC - 1;

		} else {
			// Row remains unchanged
			pcol--;
		}

		int poffset = get_offset(prow, pcol);
		// Change the character there
		vidmem[poffset] = 0x0;
		while (poffset >= 0 && vidmem[poffset] == 0x0) poffset -= 2;
		// println(itoa(poffset));
		poffset += 2;
		set_cursor(handle_scroll(poffset));
		return; 

		// This is terrible backspace handling and will be fixed
		// when I plan out a seperate text output system
		// for the pixel display driver

	} else {
		vidmem[offset] = c;
		vidmem[offset + 1] = attr;
	}

	offset = get_offset(row, col);
	offset += 2;
	
	// will handle cursor position
	set_cursor(handle_scroll(offset));
}

void print_at(int row, int col, char* msg) {
	// Terminate if empty string 
	if (msg[0] == '\0') return;

	// Print first character at [row, col]. This also sets up the cursor
	print_char(msg[0], row, col, 0);
	for (int i = 1; msg[i] != '\0'; i++) {
		print_char(msg[i], -1, -1, 0);
	}
}

void print(char *msg) {
	print_at(-1, -1, msg);
}

void println(char* msg) {
	print(msg);
	print("\n");
}

void clear_screen() {
	for (int r = 0; r < MAXR; r++) {
		for (int c = 0; c < MAXC; c++) {
			print_char(' ', r, c, WHITE_ON_BLACK);
		}
	}

	set_cursor(get_offset(0, 0));
}

void scroll_up() {
	// get current row from cursor offset
	int offset = get_cursor();
	int row = offset / (2*MAXC);

	// copy memory region from line 1 to the end of current row
	// to line 0
	int start_addr = VIDEO_ADDRESS + 2*MAXC;
	int end_addr = VIDEO_ADDRESS + get_offset(row, MAXC - 1);

	if (start_addr < end_addr) {
		memcpy(
			(char*)VIDEO_ADDRESS, 
			(char*)start_addr,
			end_addr - start_addr + 1
			);
	}
	// now clear the current row
	memset((char*)VIDEO_ADDRESS + get_offset(row, 0), 0, 2*MAXC);
}

int handle_scroll(int offset) {
	if (offset < 2 * MAXC * MAXR) {
		return offset;
	}

	scroll_up();
	offset -= 2*MAXC;
	return offset;
}

void flush(char *buff) { // Note : accepts flattened array
	unsigned char *vidmem = (unsigned char*) VIDEO_ADDRESS;

	// Flush screen without cursor shenanighans
	for (int r = 0; r < MAXR; r++) {
		for (int c = 0; c < MAXC; c++) {
			int offset = get_offset(r, c);
			char c = buff[offset/2];
			vidmem[offset] = c;
			vidmem[offset + 1] = WHITE_ON_BLACK;
		}
	}
}