#ifndef SN76496_H
#define SN76496_H

#define MAX_76496 1

//struct SN76496
typedef struct
{
	int Channel;
	int SampleRate;
	unsigned int UpdateStep;
	int VolTable[16];	/* volume table         */
	int Register[8];	/* registers */
	int LastRegister;	/* last register written */
	int Volume[4];		/* volume of voice 0-2 and noise */
	unsigned int RNG;		/* noise generator      */
	int NoiseFB;		/* noise feedback mask */
	int Period[4];
	int Count[4];
	int Output[4];
}SN76496;

struct SN76496interface
{
	int num;	/* total number of 76496 in the machine */
	int baseclock[MAX_76496];
	int volume[MAX_76496];
};

int SN76496_init(int clock,int volume,int sample_rate);
void SN76496Update(signed short *buffer,int length);
void SN76496Write(int data);

#endif
