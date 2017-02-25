#ifndef __DC_PCM_H__
#define	__DC_PCM_H__


#define NTSC_SAMPLE_RATE			31440
#define PAL_SAMPLE_RATE				31200
#define NTSC_ENHANCED_SAMPLE_RATE	30000

#define SAMPLE_RATE (sms_tv==TV_NTSC ? NTSC_SAMPLE_RATE : sms_tv==TV_PAL ? PAL_SAMPLE_RATE : NTSC_ENHANCED_SAMPLE_RATE)

#define SAMPLE_BITS 16
#define SOUND_BUF_LEN ((snd.bufsize)*2)
#define SOUND_BUF_CNT 16

extern	void pcm_init();
extern	void pcm_close();
extern	void render_pcm_pal();
extern	void render_pcm_ntsc();

extern	void sndstart(void);
extern	void pcm_clear_buffer();

#endif