/************************************************************
* dsp_ap.c
* 		You should edit this file to contain your DSP code
*		and calls to functions in other files.
*************************************************************/
#include "dsp_ap.h"
#include "delay.h"

/* Global Declarations.  Add as needed. */
delay_state_def *delay_left;
delay_state_def *delay_right;

/*
 * State of DIP switches.   Set initial value to force
 * update of delay state.
 */
unsigned int switch_state = 0xff;

float mybuffer[BUFFER_SAMPLES];
/*-----------------------------------------------------------
* dsp_init
*	This function will be called when the board first starts.
*	In here you should allocate buffers or global things
*   you will need later during actual processing.
* Inputs:
*	None
* Outputs:
*	0	Success
*	1	Error
*----------------------------------------------------------*/
int dsp_init()
{
	/* Initialize the delay block */
    if ((delay_left = delay_init()) == 0)
    {
        /* Error */
        return(1);
    }

    /* Initialize the delay block */
    if ((delay_right = delay_init()) == 0)
    {
        /* Error */
        return(1);
    }
    
    /* Success */
    return(0);
}
/*-----------------------------------------------------------
* dsp_process
*	This function is what actually processes input samples
*	and generates output samples.  The samples are passed
*	in using the arrays inL and inR, corresponding to the
*	left and right channels, respectively.  You
*	can read these and then write to the arrays outL
*	and outR.  After processing the arrays, you should exit.
* Inputs:
*	inL		Array of left input samples.  Indices on this
*			and the other arrays go from 0 to BUFFER_SAMPLES.
*
* Outputs:
*	0	Success
*	1	Error
*----------------------------------------------------------*/
void dsp_process(
	const float inL[],
	const float inR[],
	float outL[],
	float outR[])
{

	unsigned int switch_state_new;
	unsigned int delay_mult;
    
    /*
     * Check if the state of the DIP switches changed.  DIP switches are upper
     * 4 bits of USER_REG.  We use the 3 least sig. bits to indicate delay in
     * powers of 2.
     */
    switch_state_new = (USER_REG >> 4) & 0x7;
    if (switch_state_new != switch_state)
    {
        /* State of switches changed.  Update delay block. */
        switch_state = switch_state_new;

        /*
         * Compute new delay according to switch state
         *   Do in powers of 2 according to lower 3 DIP switches.  Allows us to
         *   try a wide range of delays.
         *    000 = 0 => 1
         *    001 = 1 => 2
         *    010 = 2 => 4
         *    011 = 3 => 8 ...
         *    111 = 7 => 128
         */

        delay_mult = 1 << switch_state;
        
        /* Update delay blocks */
        delay_modify(delay_left,  10*DELAY_SAMPLES_1MS*delay_mult);
        delay_modify(delay_right, 10*DELAY_SAMPLES_1MS*delay_mult);        
    }
	    /* Run the samples through the delay block. */
	delay(delay_left, inL, outL);
	delay(delay_right, inR, outR);
}