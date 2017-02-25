
#ifndef __PCM_H__
#define __PCM_H__


struct pcm
{
	int hz, len;
	int stereo;
	signed short *buf;
	int pos;
};

extern struct pcm pcm;


#endif


