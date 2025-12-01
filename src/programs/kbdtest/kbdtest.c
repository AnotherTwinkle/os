#include "drivers/screen.h"
#include "drivers/kbd.h"
#include "drivers/kbdmap.h"
#include "kernel/pit.h"
#include "kernel/util.h"

static int KBD_SUB_ID;
char printbuffer[11];

void PROGRAM_KBDTEST_MAIN() {
	KBD_SUB_ID = kbd_queue_subscribe(&kbd_queue);

	KeyEvent event;
	while (1) {
		u8 result = kbd_dequeue(&kbd_queue, KBD_SUB_ID, &event);

		if (result == 0) continue;
		print("Code: ");
		print(itoh(event.code, printbuffer));
		print(" ");
		print(itoh(event.flags, printbuffer));
		print((event.flags & 0x1) ? " MAKE" : " BREAK");
		print((event.flags & 0x2) ? " ASTATE: YES" : " ASTATE: NO");
		print((event.flags & 0x8) ? " MAPPED: YES" : " MAPPED: NO");
		print((event.flags & 0x10) ? " CSEQ: YES " : " CSEQ: NO ");
		
		if (event.flags & 0x8) {
			if (kbd_state.shiftPressed) {
				print_char(kbd_map.base[event.code],-1,-1,0);
			} else {
				print_char(kbd_map.shift[event.code],-1,-1,0);
			}
		}
		print("\n");
	}
}