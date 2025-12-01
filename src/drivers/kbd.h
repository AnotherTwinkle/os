#include "kernel/util.h"

#define KBD_DATA_PORT 0x60
#define KBD_CMD_PORT 0x60
#define KBD_CTRL_PORT 0x64

#define KBD_MAX_EVENTS 256 // Should be enough.. right?

/*
As long as we are single process mode, there will only ever be
one active key event subscriber.
*/
#define KBD_MAX_SUBSCRIBERS 16  

void KBD_INIT();

/*
Keyboard flags
bit 0 :
	set if this event is a MAKE
bit 1 :
	set if this event alters the keyboard control state
bit 2:
	set if this event is numpad press
bit 3:
	set if this event has a character mapping
bit 4:
	set if this event is a control sequence
*/

typedef struct KeyEvent {
	u8 code;
	u8 flags;
} KeyEvent;

typedef struct KeyEventQueue {
	KeyEvent buffer[KBD_MAX_EVENTS];
	u32 head;
	u32 tail;
	u32 subscriber_heads[KBD_MAX_SUBSCRIBERS];
} KeyEventQueue;

struct KeyboardState {
	u8 shiftPressed;
	u8 ctrlPressed;
	u8 altPresssed;
	u8 superPressed;
	u8 waitingForEscapeData;
	u8 waitingForSecondByte;
};

extern struct KeyboardState kbd_state;
extern struct KeyEventQueue kbd_queue;

void kbd_queue_init(KeyEventQueue* q);
u32 kbd_queue_subscribe(KeyEventQueue* q);
u32 kbd_queue_unsubscribe(KeyEventQueue* q, u32 id);
void kbd_enqueue(KeyEventQueue* q, KeyEvent event);
u8 kbd_dequeue(KeyEventQueue* q, int id, KeyEvent* out);
