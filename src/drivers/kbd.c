#include "kbd.h"
#include "kernel/io.h"
#include "kernel/isr.h"
#include "kernel/idt.h"
#include "kernel/util.h"
#include "screen.h"
#include "kbdmap.h"

#define inb port_byte_in
#define outb port_byte_out

#define SUB_FREE 0xFFFFFFFF

char print_buf[12];

struct KeyboardState kbd_state;
struct KeyEventQueue kbd_queue;

// Helper function to find the minimum active subscriber head
u32 _kbd_min_subscriber_head(KeyEventQueue* q) {
    u32 min_head = q->head;
    int active_subs = 0;

    for (int i = 0; i < KBD_MAX_SUBSCRIBERS; i++) {
        if (q->subscriber_heads[i] != SUB_FREE) {
            if (!active_subs || q->subscriber_heads[i] < min_head) {
                min_head = q->subscriber_heads[i];
            }
            active_subs = 1;
        }
    }
    return active_subs ? min_head : q->head;
}

void kbd_queue_init(KeyEventQueue* q) {
	q->head = 0;
	q->tail = 0;
	for (int i = 0; i < KBD_MAX_SUBSCRIBERS; i++) {
		q->subscriber_heads[i] = SUB_FREE;
	}
}

u32 kbd_queue_subscribe(KeyEventQueue* q) {
	for (int i = 0; i < KBD_MAX_SUBSCRIBERS; i++) {
		if (q->subscriber_heads[i] == SUB_FREE) {
			q->subscriber_heads[i] = q->head;
			return i;
		}
	}

	return SUB_FREE; // Changed to SUB_FREE to match the definition
}

u32 kbd_queue_unsubscribe(KeyEventQueue* q, u32 id) {
	q->subscriber_heads[id] = SUB_FREE;
    q->tail = _kbd_min_subscriber_head(q);
    return 0;
}

void kbd_enqueue(KeyEventQueue* q, KeyEvent event) {
    u32 next_head = (q->head + 1) % KBD_MAX_EVENTS;

    // Check if we just wrapped and overwrote the oldest event
    if (next_head == q->tail) {
        
        // If overwrite is imminent, advance any subscriber head pointing to the
        // event at q->head (the one about to be overwritten).
        for (int i = 0; i < KBD_MAX_SUBSCRIBERS; i++) {
            if (q->subscriber_heads[i] != SUB_FREE) {
                if (q->subscriber_heads[i] == q->head) {
                    q->subscriber_heads[i] = next_head;
                }
            }
        }
        
        // Find the new tail: smallest subscriber head
        q->tail = _kbd_min_subscriber_head(q);
    }

    q->buffer[q->head] = event;
    q->head = next_head;
}

// returns 0 : nothing to fetch
u8 kbd_dequeue(KeyEventQueue* q, int id, KeyEvent* out) {
	u32 pos = q->subscriber_heads[id];
	// Nothing to read if subscriber head has caught up to head
	if (pos == q->head){
		return 0;
	}

	*out = q->buffer[pos];
	q->subscriber_heads[id] = (pos + 1) % KBD_MAX_EVENTS;

	// Optional: advance tail if all subscribers have consumed the oldest event
    q->tail = _kbd_min_subscriber_head(q);

	return 1;
}

/* 
Todo: Implement E0 prefixed keys 
Currently the E0 byte is ignored (although the state is updated)
but the next byte is treated as normal
*/

void callback(RegisterState r) {
	u8 code = inb(KBD_DATA_PORT);

	if (code == CH_ESCAPE && kbd_state.waitingForEscapeData) {
		/* We have recieved two back to back escapes, invalid state */
		kbd_state.waitingForEscapeData = 0;
		return;
	}

	if (code == CH_ESCAPE) {
		kbd_state.waitingForEscapeData = 1;
		return;
	}

	if (code == CH_EXTENDED) {
		kbd_state.waitingForSecondByte = 1;
		return;
	} 

	KeyEvent event;
	event.code = code;
	event.flags = 0;

	if (kbd_state.waitingForEscapeData) {
		kbd_state.waitingForEscapeData = 0;
	} else {
		event.flags |= 0x1;
	}

	if (kbd_state.waitingForSecondByte) {
		kbd_state.waitingForSecondByte = 1;
	}


	// Handle keys that alter keyboard state
	if (code == KEY_LCTRL || code == KEY_RCTRL) {
		kbd_state.ctrlPressed ^= 1;
		event.flags |= 0x2;
	} else if (code == KEY_LSHIFT || code == KEY_RSHIFT) {
		kbd_state.shiftPressed ^= 1;
		event.flags |= 0x2;
	} else if (code == KEY_LALT || code == KEY_RALT) {
		kbd_state.altPresssed ^= 1;
		event.flags |= 0x2;
	} else if (code == KEY_LSUPER || code == KEY_RSUPER) {
		kbd_state.superPressed ^= 1;
		event.flags |= 0x2;
	}

	// set numpad flag
	if (code >= 0x54 && code <= 0x64) {
		event.flags |= 0x4;
	}

	// set printable flag
	//            A              0               _space             /
	if ((code >= 0x4 && code <= 0x27) || (code >= 0x2c && code <= 0x38)) {
		event.flags |= 0x8;
	}

	// set control sequence flag
	if (code == KEY_RETURN || code == KEY_BACKSPACE || code == KEY_TAB) {
		event.flags |= 0x10;
	}

	kbd_enqueue(&kbd_queue, event);
}

void KBD_INIT() {
	__asm__ volatile ("cli");

	// Read configuration byte
	outb(KBD_CTRL_PORT, 0x20);
	u8 config_byte = inb(KBD_DATA_PORT);

	// clear bit 6 to disable scan code translation
	config_byte &= ~0x40;
	// clear bit 0 to disable IRQs
	config_byte &= ~0x01;

	// Write config byte
	outb(KBD_CTRL_PORT, 0x60);
	outb(KBD_DATA_PORT, config_byte);

	outb(KBD_DATA_PORT, 0xF0); // Get/upd set
	inb(KBD_DATA_PORT);
	outb(KBD_DATA_PORT, 0x02); // set 2
	inb(KBD_DATA_PORT);


	// Read config byte again
	outb(KBD_CTRL_PORT, 0x20);
	config_byte = inb(KBD_DATA_PORT);
	config_byte |= 0x01; // re enable irq

	outb(KBD_CTRL_PORT, 0x60);
	outb(KBD_DATA_PORT, config_byte);

	register_interrupt_handler(IRQ1, callback);

	kbd_queue_init(&kbd_queue);


	kbd_state.shiftPressed = 0;
	kbd_state.ctrlPressed = 0;
	kbd_state.altPresssed = 0;
	kbd_state.superPressed = 0;
	kbd_state.waitingForEscapeData= 0;
	kbd_state.waitingForSecondByte = 0;

	__asm__ volatile ("sti");
}
