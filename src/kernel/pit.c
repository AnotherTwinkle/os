#include "pit.h"
#include "isr.h"
#include "util.h"
#include "io.h"
u32 CLOCK_TICKS = 0;

static void callback(RegisterState r) {
	CLOCK_TICKS++;
	// print("Tick: ");
	// char* buffer[11];
	// itoh(CLOCK_TICKS, buffer);
	// print(buffer);
	// print("\n");
}

void PIT_INIT(u32 freq) {
	register_interrupt_handler(IRQ0, &callback);

	u32 divisor = 1193180 / freq;
                  // 0x43 = command port 
	outportb(0x43, 0x36);

	outportb(0x40, (u8)(divisor & 0xff));
	outportb(0x40, (u8)((divisor >> 8) & 0xff));

}

u32 get_ticks() {
	return CLOCK_TICKS;
}