/*
**
** File: fmopl.c -- software implementation of FM sound generator
**
** Copyright (C) 1999 Tatsuyuki Satoh , MultiArcadeMachineEmurator development
**
** Version 0.36f
**
*/

#include <math.h>

#define BUILD_YM3812 1

/*
	preliminary :
	Problem :
	note:
*/

#include "shared.h"

#ifndef PI
#define PI 3.14159265358979323846
#endif

/* -------------------- preliminary define section --------------------- */
/* attack/decay rate time rate */
#define OPL_ARRATE     141280  /* RATE 4 =  2826.24ms @ 3.6MHz */
#define OPL_DRRATE    1956000  /* RATE 4 = 39280.64ms @ 3.6MHz */

#define DELTAT_MIXING_LEVEL (1) /* DELTA-T ADPCM MIXING LEVEL */

#define FREQ_BITS 24			/* frequency turn          */

/* counter bits = 20 , octerve 7 */
#define FREQ_RATE   (1<<(FREQ_BITS-20))
#define TL_BITS    (FREQ_BITS+2)

/* final output shift , limit minimum and maximum */
#define OPL_OUTSB   (TL_BITS+3-16)		/* OPL output final shift 16bit */
#define OPL_MAXOUT (0x7fff<<OPL_OUTSB)
#define OPL_MINOUT (-0x8000<<OPL_OUTSB)

/* -------------------- quality selection --------------------- */

/* sinwave entries */
/* used static memory = SIN_ENT * 4 (byte) */
#define SIN_ENT 2048

/* output level entries (envelope,sinwave) */
/* envelope counter lower bits */
#define ENV_BITS 16
/* envelope output entries */
#define EG_ENT   4096
/* used dynamic memory = EG_ENT*4*4(byte)or EG_ENT*6*4(byte) */
/* used static  memory = EG_ENT*4 (byte)                     */

#define EG_OFF   ((2*EG_ENT)<<ENV_BITS)  /* OFF          */
#define EG_DED   EG_OFF
#define EG_DST   (EG_ENT<<ENV_BITS)      /* DECAY  START */
#define EG_AED   EG_DST
#define EG_AST   0                       /* ATTACK START */

#define EG_STEP (96.0/EG_ENT) /* OPL is 0.1875 dB step  */

/* LFO table entries */
#define VIB_ENT 512
#define VIB_SHIFT (32-9)
#define AMS_ENT 512
#define AMS_SHIFT (32-9)

#define VIB_RATE 256

/* -------------------- local defines , macros --------------------- */

/* register number to channel number , slot offset */
#define SLOT1 0
#define SLOT2 1

/* envelope phase */
#define ENV_MOD_RR  0x00
#define ENV_MOD_DR  0x01
#define ENV_MOD_AR  0x02

/* -------------------- tables --------------------- */
static const int slot_array[32]=
{
	 0, 2, 4, 1, 3, 5,-1,-1,
	 6, 8,10, 7, 9,11,-1,-1,
	12,14,16,13,15,17,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1
};

/* key scale level */
#define ML (0.1875*2/EG_STEP)
static const UINT32 KSL_TABLE[8*16]=
{
	/* OCT 0 */
	 0.000*ML, 0.000*ML, 0.000*ML, 0.000*ML,
	 0.000*ML, 0.000*ML, 0.000*ML, 0.000*ML,
	 0.000*ML, 0.000*ML, 0.000*ML, 0.000*ML,
	 0.000*ML, 0.000*ML, 0.000*ML, 0.000*ML,
	/* OCT 1 */
	 0.000*ML, 0.000*ML, 0.000*ML, 0.000*ML,
	 0.000*ML, 0.000*ML, 0.000*ML, 0.000*ML,
	 0.000*ML, 0.750*ML, 1.125*ML, 1.500*ML,
	 1.875*ML, 2.250*ML, 2.625*ML, 3.000*ML,
	/* OCT 2 */
	 0.000*ML, 0.000*ML, 0.000*ML, 0.000*ML,
	 0.000*ML, 1.125*ML, 1.875*ML, 2.625*ML,
	 3.000*ML, 3.750*ML, 4.125*ML, 4.500*ML,
	 4.875*ML, 5.250*ML, 5.625*ML, 6.000*ML,
	/* OCT 3 */
	 0.000*ML, 0.000*ML, 0.000*ML, 1.875*ML,
	 3.000*ML, 4.125*ML, 4.875*ML, 5.625*ML,
	 6.000*ML, 6.750*ML, 7.125*ML, 7.500*ML,
	 7.875*ML, 8.250*ML, 8.625*ML, 9.000*ML,
	/* OCT 4 */
	 0.000*ML, 0.000*ML, 3.000*ML, 4.875*ML,
	 6.000*ML, 7.125*ML, 7.875*ML, 8.625*ML,
	 9.000*ML, 9.750*ML,10.125*ML,10.500*ML,
	10.875*ML,11.250*ML,11.625*ML,12.000*ML,
	/* OCT 5 */
	 0.000*ML, 3.000*ML, 6.000*ML, 7.875*ML,
	 9.000*ML,10.125*ML,10.875*ML,11.625*ML,
	12.000*ML,12.750*ML,13.125*ML,13.500*ML,
	13.875*ML,14.250*ML,14.625*ML,15.000*ML,
	/* OCT 6 */
	 0.000*ML, 6.000*ML, 9.000*ML,10.875*ML,
	12.000*ML,13.125*ML,13.875*ML,14.625*ML,
	15.000*ML,15.750*ML,16.125*ML,16.500*ML,
	16.875*ML,17.250*ML,17.625*ML,18.000*ML,
	/* OCT 7 */
	 0.000*ML, 9.000*ML,12.000*ML,13.875*ML,
	15.000*ML,16.125*ML,16.875*ML,17.625*ML,
	18.000*ML,18.750*ML,19.125*ML,19.500*ML,
	19.875*ML,20.250*ML,20.625*ML,21.000*ML
};
#undef ML

/* sustain lebel table (3db per step) */
/* 0 - 15: 0, 3, 6, 9,12,15,18,21,24,27,30,33,36,39,42,93 (dB)*/
#define SC(db) (db*((3/EG_STEP)*(1<<ENV_BITS)))+EG_DST
static const INT32 SL_TABLE[16]={
 SC( 0),SC( 1),SC( 2),SC(3 ),SC(4 ),SC(5 ),SC(6 ),SC( 7),
 SC( 8),SC( 9),SC(10),SC(11),SC(12),SC(13),SC(14),SC(31)
};
#undef SC

#define TL_MAX (EG_ENT*2) /* limit(tl + ksr + envelope) + sinwave */
/* TotalLevel : 48 24 12  6  3 1.5 0.75 (dB) */
/* TL_TABLE[ 0      to TL_MAX          ] : plus  section */
/* TL_TABLE[ TL_MAX to TL_MAX+TL_MAX-1 ] : minus section */
static INT32 *TL_TABLE;

/* pointers to TL_TABLE with sinwave output offset */
static INT32 **SIN_TABLE;

/* LFO table */
static INT32 *AMS_TABLE;
static INT32 *VIB_TABLE;

/* envelope output curve table */
/* attack + decay + OFF */
static INT32 ENV_CURVE[2*EG_ENT+1];

/* multiple table */
#define ML 2
static const UINT32 MUL_TABLE[16]= {
/* 1/2, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15 */
   0.50*ML, 1.00*ML, 2.00*ML, 3.00*ML, 4.00*ML, 5.00*ML, 6.00*ML, 7.00*ML,
   8.00*ML, 9.00*ML,10.00*ML,10.00*ML,12.00*ML,12.00*ML,15.00*ML,15.00*ML
};
#undef ML

/* dummy attack / decay rate ( when rate == 0 ) */
static INT32 RATE_0[16]=
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

/* -------------------- static state --------------------- */

/* lock level of common table */
static int num_lock = 0;

/* work table */
static void *cur_chip = NULL;	/* current chip point */
/* currenct chip state */
/* static FMSAMPLE  *bufL,*bufR; */
static OPL_CH *S_CH;
static OPL_CH *E_CH;
OPL_SLOT *SLOT7_1,*SLOT7_2,*SLOT8_1,*SLOT8_2;

static INT32 outd[1];
static INT32 ams;
static INT32 vib;
INT32  *ams_table;
INT32  *vib_table;
static INT32 amsIncr;
static INT32 vibIncr;
static INT32 feedback2;		/* connect for SLOT 2 */

/* log output level */
#define LOG_ERR  3      /* ERROR       */
#define LOG_WAR  2      /* WARNING     */
#define LOG_INF  1      /* INFORMATION */

#define LOG_LEVEL LOG_INF

static void Log(int level,char *format,...)
{
}

/* --------------------- subroutines  --------------------- */

static __inline__ int Limit( int val, int max, int min ) {
	return val;
}

/* status set and IRQ handling */
static __inline__ void OPL_STATUS_SET(FM_OPL *OPL,int flag)
{
}

/* status reset and IRQ handling */
static __inline__ void OPL_STATUS_RESET(FM_OPL *OPL,int flag)
{
}

/* IRQ mask set */
static __inline__ void OPL_STATUSMASK_SET(FM_OPL *OPL,int flag)
{
}

/* ----- key on  ----- */
static __inline__ void OPL_KEYON(OPL_SLOT *SLOT)
{
}
/* ----- key off ----- */
static __inline__ void OPL_KEYOFF(OPL_SLOT *SLOT)
{
}

/* ---------- calcrate Envelope Generator & Phase Generator ---------- */
/* return : envelope output */
static __inline__ UINT32 OPL_CALC_SLOT( OPL_SLOT *SLOT )
{
        return 0;
}

/* set algorythm connection */
static void set_algorythm( OPL_CH *CH)
{
}

/* ---------- frequency counter for operater update ---------- */
static __inline__ void CALC_FCSLOT(OPL_CH *CH,OPL_SLOT *SLOT)
{
}

/* set multi,am,vib,EG-TYP,KSR,mul */
static __inline__ void set_mul(FM_OPL *OPL,int slot,int v)
{
}

/* set ksl & tl */
static __inline__ void set_ksl_tl(FM_OPL *OPL,int slot,int v)
{
}

/* set attack rate & decay rate  */
static __inline__ void set_ar_dr(FM_OPL *OPL,int slot,int v)
{
}

/* set sustain level & release rate */
static __inline__ void set_sl_rr(FM_OPL *OPL,int slot,int v)
{
}

/* operator output calcrator */
#define OP_OUT(slot,env,con)   slot->wavetable[((slot->Cnt+con)/(0x1000000/SIN_ENT))&(SIN_ENT-1)][env]
/* ---------- calcrate one of channel ---------- */
static __inline__ void OPL_CALC_CH( OPL_CH *CH )
{
}

/* ---------- calcrate rythm block ---------- */
#define WHITE_NOISE_db 6.0
static __inline__ void OPL_CALC_RH( OPL_CH *CH )
{
}

/* ----------- initialize time tabls ----------- */
static void init_timetables( FM_OPL *OPL , int ARRATE , int DRRATE )
{
}

/* ---------- generic table initialize ---------- */
static int OPLOpenTable( void )
{
        return 0;
}


static void OPLCloseTable( void )
{
}

/* CSM Key Controll */
static __inline__ void CSMKeyControll(OPL_CH *CH)
{
}

/* ---------- opl initialize ---------- */
static void OPL_initalize(FM_OPL *OPL)
{
}

/* ---------- write a OPL registers ---------- */
void OPLWriteReg(FM_OPL *OPL, int r, int v)
{
}

/* lock/unlock for common table */
static int OPL_LockTable(void)
{
	return 0;
}

static void OPL_UnLockTable(void)
{
}

#if (BUILD_YM3812 || BUILD_YM3526)
/*******************************************************************************/
/*		YM3812 local section                                                   */
/*******************************************************************************/

/* ---------- update one of chip ----------- */
void YM3812UpdateOne(FM_OPL *OPL, INT16 *buffer, int length)
{
}
#endif /* (BUILD_YM3812 || BUILD_YM3526) */

#if BUILD_Y8950

void Y8950UpdateOne(FM_OPL *OPL, INT16 *buffer, int length)
{
}
#endif

/* ---------- reset one of chip ---------- */
void OPLResetChip(FM_OPL *OPL)
{
}

/* ----------  Create one of vietual YM3812 ----------       */
/* 'rate'  is sampling rate and 'bufsiz' is the size of the  */
FM_OPL *OPLCreate(int type, int clock, int rate)
{
        return NULL;
}

/* ----------  Destroy one of vietual YM3812 ----------       */
void OPLDestroy(FM_OPL *OPL)
{
}

/* ----------  Option handlers ----------       */

void OPLSetTimerHandler(FM_OPL *OPL,OPL_TIMERHANDLER TimerHandler,int channelOffset)
{
}
void OPLSetIRQHandler(FM_OPL *OPL,OPL_IRQHANDLER IRQHandler,int param)
{
}
void OPLSetUpdateHandler(FM_OPL *OPL,OPL_UPDATEHANDLER UpdateHandler,int param)
{
}
#if BUILD_Y8950
void OPLSetPortHandler(FM_OPL *OPL,OPL_PORTHANDLER_W PortHandler_w,OPL_PORTHANDLER_R PortHandler_r,int param)
{
}

void OPLSetKeyboardHandler(FM_OPL *OPL,OPL_PORTHANDLER_W KeyboardHandler_w,OPL_PORTHANDLER_R KeyboardHandler_r,int param)
{
}
#endif
/* ---------- YM3812 I/O interface ---------- */
int OPLWrite(FM_OPL *OPL,int a,int v)
{
        return 0;
}

unsigned char OPLRead(FM_OPL *OPL,int a)
{
	return 0;
}

int OPLTimerOver(FM_OPL *OPL,int c)
{
        return 0;
}
