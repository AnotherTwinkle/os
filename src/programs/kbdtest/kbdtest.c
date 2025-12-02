#include "drivers/screen.h"
#include "drivers/kbd.h"
#include "drivers/kbdmap.h"
#include "kernel/pit.h"
#include "kernel/util.h"

static int KBD_SUB_ID;

void PROGRAM_KBDTEST_MAIN() {
	KBD_SUB_ID = kbd_queue_subscribe(&kbd_queue);

	KeyEvent event;
	while (1) {
		u8 result = kbd_dequeue(&kbd_queue, KBD_SUB_ID, &event);
		if (result == 0) continue;

		// _debug_event_data(&event);

		if (!(event.flags & KBD_FLAG_MAKE)) continue;
		if (!(event.flags & KBD_FLAG_MAPPED)) continue;
		
		if (event.flags & (KBD_FLAG_CCHAR)) {
			print_at(-1, -1, kbd_map.cchar[event.code]);
			continue;
		}

		if (kbd_state.shiftPressed) {
				print_char(kbd_map.shift[event.code], -1, -1, 0);
		} else {
				print_char(kbd_map.base[event.code], -1, -1, 0);
		}
	}
}

void _debug_event_data(KeyEvent* event) {
	print("Code: ");
	print(itoh(event->code));
	print(" ");
	print(itoh(event->flags));
	print((event->flags & KBD_FLAG_MAKE) ? " MAKE" : " BREAK");
	print((event->flags & KBD_FLAG_ALTER_STATE) ? " ASTATE: YES" : " ASTATE: NO");
	print((event->flags & KBD_FLAG_NUMPAD) ? " MAPPED: YES" : " MAPPED: NO");
	print((event->flags & KBD_FLAG_CCHAR) ? " CSEQ: YES" : " CSEQ: NO");
	print((event->flags & KBD_FLAG_EXTENDED) ? " E0 : YES " : " E0 : NO ");
	
	if (kbd_state.shiftPressed) {
			print_char(kbd_map.shift[event->code], -1, -1, 0);
	} else {
			print_char(kbd_map.base[event->code], -1, -1, 0);
	}
	print("\n");
}