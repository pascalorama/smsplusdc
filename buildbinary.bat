C:\Dev-Cpp\sh-elf\bin\sh-elf-gcc.exe C:/Dev-Cpp/sh-elf/startup.o  obj/render.o obj/sms.o obj/sn76496.o obj/system.o obj/vdp.o obj/dc_adjust.o obj/dc_config.o obj/dc_emu.o obj/dc_font.o obj/dc_gui.o obj/dc_main.o obj/dc_pcm.o obj/dc_save.o obj/dc_snap.o obj/dc_sound.o obj/dc_title.o obj/dc_vmu.o obj/z80.o obj/dc_bookmark.o  -o "smsplus2.elf" -L"C:/Dev-Cpp/dclib" -ml -m4-single-only -nostartfiles -nostdlib -Wl,-Ttext=0x8c010000 romdisk.o -lpng -lz -lbz2 -lm -lkallisti -lgcc   
C:\Dev-Cpp\sh-elf\bin\sh-elf-strip smsplus2.elf
C:\Dev-Cpp\sh-elf\bin\sh-elf-objcopy -v -O binary smsplus2.elf smsplus.bin
del smsplus2.elf
scramble smsplus.bin 1st_read.bin