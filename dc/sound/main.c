#include "aica_types.h"
#include "aica_utils.h"
#include "aica.h"

/****************** Timer *******************************************/

extern volatile int timer;

void
timer_wait(int jiffies) 
{
  int fin = timer + jiffies;
  while (timer <= fin)
    ;
}


/****************** Main Program ************************************/

#define ARM_OFFSET 0
static volatile uint32 *start  = (uint32*)(0x10000 + ARM_OFFSET);
static volatile uint32 *init_done = (uint32*)(0x10004 + ARM_OFFSET);
static volatile uint32 *play_start = (uint32*)(0x10008 + ARM_OFFSET);
static volatile uint32 *rate = (uint32*)(0x10010 + ARM_OFFSET);
static volatile uint32 *bits = (uint32*)(0x10014 + ARM_OFFSET);
static volatile uint32 *buf_len = (uint32*)(0x10018 + ARM_OFFSET);
static volatile uint32 *position  = (uint32*)(0x10020 + ARM_OFFSET);
#define BASE_ADDRESS 0x11000

extern int __bss_start, end;

int
arm_main() 
{
  uint8 *bss_start = (uint8 *)(&__bss_start);
  uint8 *bss_end = (uint8 *)(&end);
  int sound_bits;
  
  *start = 0;
  *init_done = 0;
  *play_start = 0;
  
  /* Clear out the BSS area */
  memset(bss_start, 0, bss_end - bss_start);
  
  /* Initialize the AICA part of the SPU */
  aica_init();
  
  /* Observe channel 0 */
  SNDREG8(0x280d) = 0;
  
  while (!*start) 
  {
    *init_done = 1;
    timer_wait (10);
  }
  
  switch (*bits)
  {
    case 8:
      sound_bits = SM_8BIT;
      break;
    case 16:
      sound_bits = SM_16BIT;
      break;
    default:
      /* ADPCM don't work now */
      return 0;
  }
  
  aica_play (0, BASE_ADDRESS, sound_bits, 0, *buf_len / (*bits / 8),*rate, 0xff, 128, 1);
  //aica_play (0, BASE_ADDRESS, 0, 0, *buf_len/2,*rate, 0xff, 128, 1);
  //aica_play (0, BASE_ADDRESS, 0, 0, 4410,*rate, 0xff, 128, 1);
	//aica_play(0, 0x10000,0, 0,1472,44100, 0xFF, 0x80, 1);  
	//aica_play (0, BASE_ADDRESS,0,0,736, 22050,0xFF, 128, 1);
  
  *play_start = 1;
  
  for (;;) 
  {
    *position = SNDREG32(0x2814);
    timer_wait (2);
  }
  
  return 0;
}

