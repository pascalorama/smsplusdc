# ------------------------------------------
# Title:			Default Makefile for use in VC++
# File:		      default.mak
# Revision:             20020210 8:30pm
# 
# Author:			Benjamin D. Hale
# Orignally by:         Jaap Suter
# Contributed to by:    Christer Andersson
# 
# Info:		      This file is contains the make-instructions
#				for the project. Replace DEVDIR with the appropriate
#                       base directory for devkitdc. After you make the 
#                       needed modifications here rename this file to
#                       <project_name>.mak so Visual C++ does not get
#                       confused.
# -------------------------------------------

# -------------------------------------------
# Project name definition makes this whole thing 
# a lot easier to use. Only two total modifications
# (other than what .o files will be needed)
# should be needed to get this working with your
# compiler and Visual C++. Replace "Default" with
# your project's name.
# -------------------------------------------

PROJECT = smsplusDC

# -------------------------------------------
# Define some directories;
# -------------------------------------------

# -------------------------------------------
# Base Directory for gcc arm-agb-elf replace with 
# wherever you put your gba devkitadv files.
# -------------------------------------------

DEVDIR  = C:\Dev-Cpp\sh-elf

# -------------------------------------------
# Source directory (where your project's source files are located.)
# -------------------------------------------

SRCDIR  = C:\Dev\smsplus

# -------------------------------------------
# Compiler Directories for binaries, includes and libs.
# -------------------------------------------

CMPDIR  = $(DEVDIR)\bin
LIBDIR  = $(DEVDIR)\lib
LIBDIR2 = $(DEVDIR)\sh-sega-dreamcast\lib
INCDIR  = $(DEVDIR)\include
INCDIR2 = $(DEVDIR)\kernel\arch\dreamcast\include

# -------------------------------------------
# END of directory defines
# -------------------------------------------

# -------------------------------------------
# Define what extensions we use;
# -------------------------------------------
.SUFFIXES : .cpp .c .s

# -------------------------------------------
# Define the flags for the compilers;
# -------------------------------------------
CFLAGS  = -I $(INCDIR) -I $(INCDIR2) -ml -m4-single-only -O1 -fno-builtin -fno-strict-aliasing -fomit-frame-pointer -nostdlib
SFLAGS  = -I $(INCDIR) -I $(INCDIR2) -little
LDFLAGS = -L $(LIBDIR) -L $(LIBDIR2) -lgl -ldcutils -lkallisti -lgcc -Ttext=0x8c010000 

# -------------------------------------------
# Define the list of all O files;
# Just follow the syntax shown to add any 
# other objects your project may need to
# compile properly. You will need to add 
# files to this part to make it work with 
# your project add a \ to the end of all o
# files except the last one. Like below.
# -------------------------------------------

O_FILES	= \
romdisk.o \
startup.o \
smsplusDC.c

# -------------------------------------------
# There should be no need to modify anything
# below here.
# -------------------------------------------

# -------------------------------------------
# Define the final dependecy;
# -------------------------------------------
all : $(PROJECT).bin

# -------------------------------------------
# Define the copy from .elf to .bin file
# -------------------------------------------
$(PROJECT).bin : $(PROJECT).elf
	$(CMPDIR)\objcopy -v -O binary $(PROJECT).elf $(PROJECT).bin
	-@echo ------------------------------------------ 
	-@echo Done 
	-@echo ------------------------------------------ 
	
# -------------------------------------------
# Define the linker instruction;
# -------------------------------------------
$(PROJECT).elf : $(O_FILES)
	$(CMPDIR)\gcc -o $(PROJECT).elf $(O_FILES) $(CFLAGS) $(LDFLAGS) 
	-@echo ------------------------------------------ 
	-@echo Compiling and Linking Done
	-@echo ------------------------------------------ 

# -------------------------------------------
# Any problems with getting this working email
# questions to whatzdat_pimp@hotmail.com .
# This was tested on devkitadv r4 and Visual C++ 6
# on a p3 450/512mb/Windows XP Pro.
# -------------------------------------------
# EOF;

