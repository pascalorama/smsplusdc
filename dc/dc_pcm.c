#include "dc_sound.h"
#include <dc/g2bus.h>


#include "shared.h"
#include "dc_shared.h"

/* SOUND_BUF_LEN = 2 * (rate * buffer_length_in_frames / 60.0) * (bit / 8) */
/* (format "%x" ((lambda (x) (* 2 (/ (* 44100 x) 60) (/ 16 8))) 3)) */
/* NOTE: AICA require 32-bit (or higher) alignment. */

int pcm_initialized=0;
int sound_init=0;
uint32 last_pos = 0;

#define ARM_OFFSET 0xa0800000
static volatile uint32 *start  = (uint32*)(0x10000 + ARM_OFFSET);
static volatile uint32 *init_done = (uint32*)(0x10004 + ARM_OFFSET);
static volatile uint32 *play_start = (uint32*)(0x10008 + ARM_OFFSET);
static volatile uint32 *rate = (uint32*)(0x10010 + ARM_OFFSET);
static volatile uint32 *bits = (uint32*)(0x10014 + ARM_OFFSET);
static volatile uint32 *buf_len = (uint32*)(0x10018 + ARM_OFFSET);
static volatile uint32 *position  = (uint32*)(0x10020 + ARM_OFFSET);


void sndstart(void);

void pcm_clear_buffer()
{	
	memset(&snd.psg_buffer[0][0],0,snd.bufsize);
	
	spu_memset (dc_sound_get_baseaddr (), 0, SOUND_BUF_LEN * SOUND_BUF_CNT);}


void pcm_init()
{
	dc_sound_init (SAMPLE_RATE, SAMPLE_BITS, (SOUND_BUF_LEN) * SOUND_BUF_CNT);
	*start=false;	
	//last_pos = dc_sound_get_position () / snd.bufsize;	
	last_pos = 0;	
	*start=true;
		
}

//-----------------------------------------------------------------------------
// Sound Buffers Rendering

void pcm_close()
{
	dc_sound_shutdown ();	
}

//-----------------------------------------------------------------------------
// Sound Buffers Rendering



void render_pcm_pal()
{
	/*
	int count=0;
	int pcm_pos=0;
	uint32 cur_pos = dc_sound_get_position () / snd.bufsize;
				
	cur_pos = dc_sound_get_position () / snd.bufsize;
	while(cur_pos==last_pos && pcm_initialized)
	{	cur_pos = dc_sound_get_position () / snd.bufsize;}
		
	spu_memload (dc_sound_get_baseaddr () + SOUND_BUF_LEN * last_pos,&snd.psg_buffer[0][0],SOUND_BUF_LEN);		
			
	last_pos++;
	last_pos&=0xf;				
	*/

	uint32 cur_pos;			
					
	cur_pos = dc_sound_get_position () / snd.bufsize;
	while(cur_pos==last_pos)
	{	cur_pos = dc_sound_get_position () / snd.bufsize;
	}
	
	spu_memload (dc_sound_get_baseaddr () + SOUND_BUF_LEN * last_pos,&snd.psg_buffer[0][0],SOUND_BUF_LEN);		
	last_pos=cur_pos;
}



void render_pcm_ntsc()
{

	if( abs(dc_sound_get_position () - (last_pos * snd.bufsize))<(snd.bufsize-10) )
	{	
		last_pos++;
		last_pos&=0xf;
	}
	
	spu_memload (dc_sound_get_baseaddr () + SOUND_BUF_LEN * last_pos,&snd.psg_buffer[0][0],SOUND_BUF_LEN);		
			
	last_pos++;	
	last_pos&=0xf;
}

//-----------------------------------------------------------------------------


void * snd_callback(int req, int * ret)
{
	SN76496Update(&snd.psg_buffer[0][0],req>>1);

    *ret = req;

	return &snd.psg_buffer[0][0];
}

void sndstart(void)
{
 		snd_stream_init (snd_callback);
		snd_stream_start(44100, 1);
}

void sndstop(void)
{
		snd_stream_stop();
}