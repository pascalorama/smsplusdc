#include <shared.h>
#include <dc/g2bus.h>
#include "dc_sound.h"

#define ARM_OFFSET 0xa0800000
static volatile uint32 *start  = (uint32*)(0x10000 + ARM_OFFSET);
static volatile uint32 *init_done = (uint32*)(0x10004 + ARM_OFFSET);
static volatile uint32 *play_start = (uint32*)(0x10008 + ARM_OFFSET);
static volatile uint32 *rate = (uint32*)(0x10010 + ARM_OFFSET);
static volatile uint32 *bits = (uint32*)(0x10014 + ARM_OFFSET);
static volatile uint32 *buf_len = (uint32*)(0x10018 + ARM_OFFSET);
static volatile uint32 *position  = (uint32*)(0x10020 + ARM_OFFSET);

#define BASE_ADDRESS 0x11000

static const uint8 stream_driver[] = {
#include "sound/aica_fw.h"
};


void
dc_sound_init (int sample_rate, int sample_bits, int buffer_len)
{
  *init_done = 0; /* need before spu_memload */
  g2_fifo_wait ();
  
  spu_disable();
  spu_memload (0, stream_driver, sizeof(stream_driver));
  spu_enable ();
  
  *rate = sample_rate;
  *bits = sample_bits;
  *buf_len = buffer_len;
  g2_fifo_wait ();
  
  for (;;)
  {
    if (*init_done) break;
    timer_spin_sleep (100);
  }
  
  *start = true;
  g2_fifo_wait ();
  
  for (;;)
  {
    if (*play_start) break;
    timer_spin_sleep (100);
  }

}


void
dc_sound_shutdown (void)
{
  spu_disable();
}


uint32
dc_sound_get_baseaddr (void)
{
  return BASE_ADDRESS;
}


uint32 
dc_sound_get_position (void)
{
  uint32 pos = *position;
  g2_fifo_wait ();
  return pos;
}


