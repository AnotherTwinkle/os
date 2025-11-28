//  Screen driver functionality

#include "screen.h"
#include "../kernel/io.h"
#define VIDEO_ADDRESS 0xb8000
#define MAXR 25
#define MAXC 80

#define WHITE_ON_BLACK 0x0f

#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

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
		col = 79; // go to end of current row
	} else if (c == '\r') {
		row -= 1, col = 79; // end of previous row
	} else {
		vidmem[offset] = c;
		vidmem[offset + 1] = attr;
	}
	
	offset = get_offset(row, col);
	offset += 2;
	set_cursor(offset);
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