#include "util.h"
#include "isr.h"
#include "io.h"

ISR interrupt_handlers[256];

char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void isr_handler(RegisterState r) {
	return;
}

void register_interrupt_handler(u32 i, ISR handler) {
	interrupt_handlers[i] = handler;
}

void irq_handler(RegisterState r) {
	// Send end of interrupt to PICs
	// If it comes from slave, gotta send to master and slave

	if (r.int_no >= 40) outportb(0xA0, 0x20);
	outportb(0x20, 0x20);

	// If 0 then we have no handlers defined
	if (interrupt_handlers[r.int_no] != 0) {
		ISR handler = interrupt_handlers[r.int_no];
		handler(r);
	}
}