#include "io.h"
#define byte unsigned char
#define word unsigned short

byte port_byte_in(word port) {
	byte result;
	__asm__ volatile (
			"in %%dx, %%al" // Address in dx, result in al
			: "=a"(result)
			: "d"(port)
		);

	return result;
}

void port_byte_out(word port, byte data) {
	__asm__ volatile (
			"out %%al, %%dx" // Address in dx, argument in al
			: 
			: "a"(data), "d"(port)
		);
}


word port_word_in(word port) {
	word result;
	__asm__ volatile (
			"in %%dx, %%ax" // Address in dx, result in al
			: "=a"(result)
			: "d"(port)
		);

	return result;
}

void port_word_out(word port, word data) {
	__asm__ volatile (
			"out %%ax, %%dx" // Address in dx, argument in al
			: 
			: "a"(data), "d"(port)
		);
}
