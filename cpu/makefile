#
# SMS Plus
# Copyright (C) 1998, 1999, 2000  Charles Mac Donald
#
# Define LSB_FIRST for little endian systems.
# Define ALIGN_DWORD if you need aligned 32-bit memory transfers.
#


# Executable and archive name
NAME	=	sp

# Target platform (dos, psx)
SYSTEM	=       dos
#SYSTEM	=	psx

# Debug flags
FLAGS	=	#-Wall -Werror

# DOS specific stuff
ifeq ($(SYSTEM), dos)
CC	=	gcc
AS	=	nasm -f coff
AR	=	ar
LD	=	ld
FLAGS	+=	-DLSB_FIRST -I./dos -DDOS -DX86_ASM -mpentium -O6 -fomit-frame-pointer
LDFLAGS	=	
OBJDIR	=	dos/obj
SYSOBJ	=	$(OBJDIR)/dos.o \
		$(OBJDIR)/sealintf.o \
		$(OBJDIR)/config.o \
		$(OBJDIR)/blit.o \
		$(OBJDIR)/blur.o \
		$(OBJDIR)/expand.o \
		$(OBJDIR)/unzip.o \
		$(OBJDIR)/loadzip.o
LIB	=	-lalleg -laudio -lz
EXE	=	$(NAME).exe
endif

# PSX specific stuff
ifeq ($(SYSTEM), psx)
CC	=	mipsgcc
AS	=	mipsas
AR	=	mipsar
LD	=	mipsld
LDFLAGS	=	-Xlinker -mpsx
FLAGS	+=	-DLSB_FIRST -DALIGN_DWORDS -I./psx -DPSX -O2
OBJDIR	=	psx/obj
SYSOBJ	=	$(OBJDIR)/psx.o
LIB	=	-lpsx
EXE	=	$(NAME).pxe
endif

# Shared data
FLAGS	+=	-I. -I./cpu
SPLIB	=	$(OBJDIR)/$(NAME).a $(OBJDIR)/z80.a

# Object files
OBJ	=       $(OBJDIR)/system.o \
		$(OBJDIR)/sms.o	\
		$(OBJDIR)/vdp.o	\
		$(OBJDIR)/render.o \
		$(OBJDIR)/sn76496.o \
		$(OBJDIR)/fmopl.o \
		$(OBJDIR)/ym2413.o
        
# Main target
all	:	$(EXE)

$(EXE)	:	$(SYSOBJ) $(SPLIB) $(OBJ)
		$(CC) -o $(EXE) $(LDFLAGS) $(SYSOBJ) $(SPLIB) $(LIB)

# Archive file for the mips tools commandline limitation
$(OBJDIR)/$(NAME).a : $(OBJ)		        

# Shared files
$(OBJDIR)/%.o :	%.c %.h
		$(CC) -c $< -o $@ $(FLAGS)
		$(AR) rs $(OBJDIR)/$(NAME).a $@					
	        
# System specific files
$(OBJDIR)/%.o : $(SYSTEM)/%.c $(SYSTEM)/%.h	        
		$(CC) -c $< -o $@ $(FLAGS)
	        
$(OBJDIR)/%.o : $(SYSTEM)/%.s
		$(AS) $< -o $@

# The CPU emulator
$(OBJDIR)/z80.a	: $(OBJDIR)/z80.oa

$(OBJDIR)/z80.oa : cpu/z80.c cpu/z80.h
ifeq ($(SYSTEM), psx)	        
		$(CC) -c cpu/z80.c -o $(OBJDIR)/z80.oa $(FLAGS) -O1
else
		$(CC) -c cpu/z80.c -o $(OBJDIR)/z80.oa $(FLAGS)
endif		
		$(AR) rs $(OBJDIR)/z80.a $(OBJDIR)/z80.oa

# Other targets	        

# Zip up the source code
.PHONY	: archive
ARCFN	= spsrc.zip
STDOP	= -add -max -silent \
	  -excl=*.?xe -excl=*.a -excl=*.o -excl=*.bak -excl=*.oa
archive	:
		@echo Archiving source ...
		@rm -f $(ARCFN)
		pk $(STDOP) $(ARCFN) *.*
		pk $(STDOP) -dir $(ARCFN) dos\*.*
		pk $(STDOP) -dir $(ARCFN) psx\*.*
		pk $(STDOP) -dir $(ARCFN) cpu\*.*

# Compress the executable 
.PHONY	: pack
pack	:
		@echo Packing $< ...
		@strip $(EXE) --strip-all
		@upx -1 $(EXE)	        

# Remove object and debug files
.PHONY	: clean
clean	:	        
		@rm -f $(OBJDIR)/*.o
		@rm -f $(OBJDIR)/$(NAME).a
		@rm -f $(EXE)
		@rm -f *.bak
	        
# Remove the CPU emulator 	        
.PHONY  : cleanlib
cleanlib :
		@rm -f $(OBJDIR)/z80.a
		@rm -f $(OBJDIR)/z80.oa

#
# end of makefile
#
