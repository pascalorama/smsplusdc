all: stream.drv aica_fw.h

aica_fw.h: stream.drv
	$(NDC_BASE)/utils/bin2c/bin2c stream.drv aica_fw.h
	echo >> aica_fw.h

stream.drv: prog.elf
	$(DC_ARM7OBJCOPY) -O binary prog.elf stream.drv

prog.elf: crt0.o main.o aica.o aica_utils.o
	$(DC_ARM7CC) -Wl,-Ttext,0x00000000 -nostartfiles -nostdlib -e reset -o prog.elf crt0.o main.o aica.o aica_utils.o -lgcc 

%.o: %.c
	$(DC_ARM7CC) $(DC_ARM7CFLAGS) $(DC_ARM7INCS) -c $< -o $@

%.o: %.s
	$(DC_ARM7AS) $< -o $@

clean:
	-rm -f *.o *.srec *.elf 1ST_READ.BIN prog.bin *.bck stream.drv sound.bin


