all: BuildBootloader BuildKernel32 Wrapup

BuildBootloader: 
	@echo [*] Building Bootloader
	make -C bootloader
	@echo [+] Building Bootloader Complete

BuildKernel32:
	@echo [*] Building Kernel32
	make -C kernel32
	@echo [*] Building Kernel32 Complete

Wrapup: bootloader/bootloader.bin kernel32/kernel32.bin
	@echo [*] Wrapup
	@echo [*] Building disk image
	cat $^ > disk.img

	@echo [+] Building disk image Complete
	@echo [+] Wrapup Complete

clean:
	make -C bootloader clean
	make -C kernel32 clean
	rm -f disk.img

