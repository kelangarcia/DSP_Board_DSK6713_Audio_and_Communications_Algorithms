/*****************************************************************
                             dsp_ap.h
 	Contains global definitions for your DSP application.
 	Here you can control the size and number of
 	audio buffers (if needed), the sample rate, and
 	the audio source.
 *****************************************************************/

#ifndef _dsp_ap_h_
    #define _dsp_ap_h_

    #include "math.h"
    #include "aic23.h"
    #include "dsk_registers.h"

    /* DSP_SOURCE
    * -----------
    * The following lines control whether Line_In or Mic_In is
    * the source of the audio samples to the DSP board.  Use Mic_In
    * if you want to use the headset, or Line_In if you want to use
    * the PC to generate signals.  Just uncomment one of the lines
    * below.
    */
    //#define  DSP_SOURCE   AIC23_REG4_LINEIN
    #define  DSP_SOURCE   AIC23_REG4_MIC

    /* DSP_SAMPLE_RATE
    * ----------------
    * The following lines control the sample rate of the DSP board.
    * Just uncomment one of the lines below to get sample rates from
    * 8000 Hz up to 96kHz.
    */
    #define DSP_SAMPLE_RATE     AIC23_REG8_8KHZ
    //#define DSP_SAMPLE_RATE     AIC23_REG8_32KHZ
    //#define DSP_SAMPLE_RATE     AIC23_REG8_48KHZ
    //#define DSP_SAMPLE_RATE     AIC23_REG8_96KHZ

    /***********************************************************/
    /* You can probably leave the stuff below this line alone. */
    /***********************************************************/

    // Number of samples in hardware buffers.  Must be a multiple of 32.
    #define BUFFER_SAMPLES		128

    // Number of buffers to allocate.  Need at least 2.
    #define NUM_BUFFERS     2

    // Scale used for FP<->Int conversions
    #define SCALE           16384

    int dsp_init();
    void dsp_process(const float inL[],	const float inR[],	float outL[], 	float outR[]);
#endif /* _dsp_ap_h_ */