PROGRAM_SOURCES := $(wildcard src/programs/*/*.c)
PROGRAM_HEADERS := $(wildcard src/programs/*/*.h)

C_SOURCES = $(wildcard src/kernel/*.c src/drivers/*.c) $(PROGRAM_SOURCES)
HEADERS = $(wildcard src/kernel/*.h src/drivers/*.h) $(PROGRAM_HEADERS)

OBJ = ${C_SOURCES:.c=.o} src/kernel/interrupt.o

# --- DEBUGGING LINES ---
$(info C_SOURCES: $(C_SOURCES))
$(info PROGRAM_SOURCES: $(PROGRAM_SOURCES))
$(info OBJ list: $(OBJ))
# -----------------------

all : os.img

run : all
	qemu-system-x86_64 os.img

os.img : src/boot/boot.bin kernel.bin
	cat $^ > $@
	truncate -s 32768 $@

kernel.bin: src/boot/kernel_entry.o ${OBJ}
	ld -m elf_i386 -o $@ -Ttext 0x1000 $^ --oformat binary

%.o : %.c ${HEADERS}
	gcc -I src -m32 -ffreestanding -fno-pie -c $< -o $@

%.o : %.asm
	nasm $< -f elf32 -o $@

%.bin : %.asm
	nasm $< -f bin -o $@

clean:
	rm -f *.bin *.o *.dis *.img *.map
	rm -rf src/kernel/*.o src/boot/*.bin src/drivers/*.o programs/*/*.o