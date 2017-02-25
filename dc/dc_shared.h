#ifndef __DC_SHARED__
#define __DC_SHARED__

typedef struct
{	unsigned char w,h;
	unsigned char data[8];
}FONT_GLYPH ;


//-----------------------------------------------------------------------------
// Includes
#include <kos.h>

#include <zlib/zlib.h>
#include "pvr_internal.h"
#include <png/png.h>


#define ARGB1555(a,r,g,b) ((a&1)<<15) | ((r>>3)<<10) | ((g>>3)<<5) | ((b>>3))
#define RGB565(r,g,b) ( ((r>>3)<<11) | ((g>>3)<<6) | (b>>3))


#include "dc_vmu.h"
#include "dc_save.h"
#include "dc_config.h"
#include "dc_pcm.h"
#include "dc_gui.h"
#include "dc_emu.h"
#include "dc_font.h"
#include "dc_snap.h"
#include "dc_adjust.h"
#include "dc_bookmark.h"
#include "dc_title.h"

#include "sms_db.h"

#define	MAIN_SCREEN		0
#define	ROMS_SCREEN		1
#define	EMU_SCREEN		2
#define OPTIONS_SCREEN	3
#define ADJUST_SCREEN	4
#define CREDITS_SCREEN	5

#define	OUT_SCREEN		255



//-----------------------------------------------------------------------------
// Variables

extern unsigned char romdisk[];

extern	int rom_crc;
extern	int gui_state;

extern	int vertex_color_value; 
extern	float vertex_color; 


#endif
