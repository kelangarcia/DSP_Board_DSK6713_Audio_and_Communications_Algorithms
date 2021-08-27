/*============================================================================
 * audio.h
 *      Global definitions for audio.cpp.
 *==========================================================================*/

#ifndef _audio_h_
#define _audio_h_

int audioPlay(signed short *data, unsigned int samples, unsigned int sampleRate);
int audioRecord(signed short *data, unsigned int samples, unsigned int sampleRate);
int audioRecPlay(signed short *data_play, signed short *data_rec, unsigned int samples, unsigned int sampleRate);

#endif
