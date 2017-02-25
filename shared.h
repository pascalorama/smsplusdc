
#ifndef _SHARED_H_
#define _SHARED_H_

#define VERSION     "0.9.3"

/* Data types */
typedef unsigned long int dword;
typedef unsigned short int word;
typedef unsigned char byte;


#ifndef true
#define true (1)
#endif
#ifndef false
#define false (0)
#endif
#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

/* To keep the MAME code happy */
#define HAS_YM3812  1
typedef signed short int FMSAMPLE;

/* Nasty hacks so the PSX version will compile */
#if PSX
#include "psx/syscall.h"    /* for NULL, malloc(), etc. */
#define fread               /* No real filesystem :) */
#define fwrite
#define sin                 /* No equivelant */
#define pow
#define log10
#endif


#if DOS
#include <stdio.h>
#endif

//#if DC
#include <kos.h>
#include <dc/dc_shared.h>
//#endif

#include "cpu/Z80.H"
#include "sms.h"
#include "vdp.h"
#include "render.h"
#include "sn76496.h"

#ifdef FMCHIP
#include "fmopl.h"
#include "ym2413.h"
#endif

#include "system.h"


#endif /* _SHARED_H_ */


