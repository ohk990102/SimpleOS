NASM32 = nasm
GCC32 = x86_64-pc-linux-gcc.exe -c -m32 -ffreestanding
LD32 = x86_64-pc-linux-ld.exe -melf_i386 -T ../elf_i386.x -nostdlib -e Main -Ttext 0x10200
OBJCOPY32 = x86_64-pc-linux-objcopy.exe -j .text -j .data -j .rodata -j .bss -S -O binary

OBJECTDIRECTORY = tmp
SOURCEDIRECTORY = src

all: prepare kernel32.bin

prepare:
	mkdir -p $(OBJECTDIRECTORY)

$(OBJECTDIRECTORY)/entrypoint.bin: $(SOURCEDIRECTORY)/entrypoint.s
	$(NASM32) -o $@ $<

dep:
	@echo [*] Making dependency file
	make -C $(OBJECTDIRECTORY) -f ../makefile internalDependency
	@echo [+] Making dependency file complete

executeInternalBuild: dep
	make -C $(OBJECTDIRECTORY) -f ../makefile kernel32.elf

$(OBJECTDIRECTORY)/kernel32.elf.bin: executeInternalBuild
	$(OBJCOPY32) $(OBJECTDIRECTORY)/kernel32.elf $@

kernel32.bin: $(OBJECTDIRECTORY)/entrypoint.bin $(OBJECTDIRECTORY)/kernel32.elf.bin
	cat $^ > $@

clean:
	rm -f *.bin
	rm -f $(OBJECTDIRECTORY)/*.*

CENTRYPOINTOBJECTFILE = main.o
CSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.c)
ASSEMBLYSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.asm)
COBJECTFILES = $(subst main.o, , $(notdir $(patsubst %.c,%.o,$(CSOURCEFILES))))
ASSEMBLYOBJECTFILES = $(notdir $(patsubst %.asm,%.o,$(ASSEMBLYSOURCEFILES)))

%.o: ../$(SOURCEDIRECTORY)/%.c
	$(GCC32) -c $<

%.o: ../$(SOURCEDIRECTORY)/%.asm
	$(NASM32) -f elf32 -o $@ $<

internalDependency:
	$(GCC32) -MM $(CSOURCEFILES) > dependency.dep

kernel32.elf: $(CENTRYPOINTOBJECTFILE) $(COBJECTFILES) $(ASSEMBLYOBJECTFILES)
	$(LD32) -o $@ $^

ifeq (dependency.dep, $(wildcard dependency.dep))
include dependency.dep
endif
