PROGRAM_SOURCES := $(wildcard src/programs/*/*.c)
PROGRAM_HEADERS := $(wildcard src/programs/*/*.h)

C_SOURCES = $(wildcard src/kernel/*.c src/drivers/*.c src/graphics/*.c) $(PROGRAM_SOURCES)
HEADERS = $(wildcard src/kernel/*.h src/drivers/*.h src/graphics/*.h) $(PROGRAM_HEADERS)

OBJ = ${C_SOURCES:.c=.o} src/kernel/interrupt.o

$(info C_SOURCES: $(C_SOURCES))
$(info PROGRAM_SOURCES: $(PROGRAM_SOURCES))
$(info OBJ list: $(OBJ))

all : os.img

run : all
	qemu-system-x86_64 -drive format=raw,file=os.img

scripts/poke_into_img.sh:
	python3.12 scripts/gen_sprite_data.py
	python3.12 scripts/gen_level_data.py

os.img : src/boot/boot.bin src/boot/setup.bin kernel.bin scripts/poke_into_img.sh
	cat $^ > $@
	truncate -s 10485701 $@
	./scripts/poke_into_img.sh

kernel.bin: src/boot/kernel_entry.o ${OBJ}
	ld -m elf_i386 -o $@ -Ttext 0xa000 $^ --oformat binary

%.o : %.c ${HEADERS} scripts/poke_into_img.sh
	gcc -O0 -I src -m32 -ffreestanding -fno-pie -c $< -o $@

%.o : %.asm
	nasm $< -f elf32 -o $@

src/boot/setup.bin: src/boot/setup.asm
	nasm $< -f  bin -o $@
	truncate -s 4096 $@  # Pad to exactly 8 sectors (4096 bytes)

%.bin : %.asm
	nasm $< -f bin -o $@

clean:
	rm -f *.bin *.o *.dis *.img *.map
	rm -rf src/kernel/*.o src/boot/*.bin src/drivers/*.o src/programs/*/*.o src/graphics/*o src/programs/*/*/*.bin
	rm -f scripts/poke_into_img.sh