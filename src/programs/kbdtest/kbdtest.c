#include "drivers/screen.h"
#include "drivers/kbd.h"
#include "drivers/kbdmap.h"
#include "kernel/pit.h"
#include "kernel/util.h"
#include "drivers/ttylegacy.h"

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
			tty_print_at(-1, -1, kbd_map.cchar[event.code]);
			continue;
		}

		if (kbd_state.shiftPressed) {
				tty_print_char(kbd_map.shift[event.code], -1, -1, 0);
		} else {
				tty_print_char(kbd_map.base[event.code], -1, -1, 0);
		}
	}
}

void _debug_event_data(KeyEvent* event) {
	tty_print("Code: ");
	tty_print(itoh(event->code));
	tty_print(" ");
	tty_print(itoh(event->flags));
	tty_print((event->flags & KBD_FLAG_MAKE) ? " MAKE" : " BREAK");
	tty_print((event->flags & KBD_FLAG_ALTER_STATE) ? " ASTATE: YES" : " ASTATE: NO");
	tty_print((event->flags & KBD_FLAG_NUMPAD) ? " MAPPED: YES" : " MAPPED: NO");
	tty_print((event->flags & KBD_FLAG_CCHAR) ? " CSEQ: YES" : " CSEQ: NO");
	tty_print((event->flags & KBD_FLAG_EXTENDED) ? " E0 : YES " : " E0 : NO ");
	
	if (kbd_state.shiftPressed) {
			tty_print_char(kbd_map.shift[event->code], -1, -1, 0);
	} else {
			tty_print_char(kbd_map.base[event->code], -1, -1, 0);
	}
	tty_print("\n");
}