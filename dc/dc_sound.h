#ifndef _DC_SOUND_H
#define _DC_SOUND_H

#ifdef __cplusplus
extern "C" {
#endif

#if 0
}
#endif

#include <kos.h>

void dc_sound_init (int sample_rate, int sample_bits, int buffer_len);
void dc_sound_shutdown (void);
uint32 dc_sound_get_baseaddr (void);
uint32 dc_sound_get_position (void);

#ifdef __cplusplus
}
#endif

#endif


