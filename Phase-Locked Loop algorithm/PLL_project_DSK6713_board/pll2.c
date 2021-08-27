/*****************************************************************
            This pll.c generates double the input frequency 
 *****************************************************************/

// Libraries that were used in delay.c
#include <std.h>
#include <sys.h>
#include <dev.h>
#include <sio.h>


// Include the headers of the files needed
#include "pll.h"
#include "dsp_ap.h"
#include "math.h" // needed fot floor() and abs()

pll_state_def *pll_init(int k, int D, float f, float w_0, int T, int S)
{
    /*  pll_init()
    This function initializes a pll block.
    Inputs:
    k = Gain,           D = Damping Constant,
    f = frequency,      w_0 = Corner Frequency,
    T = Sample Period,  S = # of values in the sine table
    Returns:
    0       An error ocurred
    other   A pointer to a new PLL structure */

    // Creating a struct named s
    pll_state_def *s;

    // Allocating te struct in te memory segment PLL_SEG_ID
    s = (pll_state_def *)MEM_calloc(PLL_SEG_ID, sizeof(pll_state_def), PLL_BUFFER_ALIGN);
    
    //Checking if the struct was created correctly and sending a message of error if not    
    if (s == NULL)
    {
       SYS_error("Unable to create an input delay floating-point buffer.", SYS_EUSER, 0);
        return(0);
    }

    // Setting all variables inside the struct, so it can be returned in one variable.
    s->k = k;       // k = Gain
    s->f = f;       // f = frequency
    s->S = S;       // S = # of values in the sine table

    // All of the following values should be initialized in 0
    s->zp = 0;      // Multiplication between previous x_in and y_out
    s->zc = 0;      // Multiplication between current x_in and y_out
    s->vp = 0;      // Result of zp in the IIR filter
    s->vc = 0;      // Result of zc in the IIR filter
    s->y = 0;       // Output value
    s->accum2 = 0;   //Accumulator,used for the frequency in the sin table.

    //  This normalized with 1 at beginning
    s->amp_est = 1; // Value used to normalized the current input

    // tau1 and tau2 are helper values to calculate the time domain IIR filter.
    float tau1 = k / (w_0 * w_0);   
    float tau2 = ((2 * D) / w_0) - 1/k;

    //this coefficients are used later to calculate the v[n]
    s->a1 = -(T - 2*tau1)/(T + 2*tau1);
    s->b0 = (T + 2*tau2)/(T + 2*tau1);
    s->b1 = (T - 2*tau2)/(T + 2*tau1);


    // Sine Table calculation  depending on the position i
    /*      It will create a table of sine starting at
        sine(2 * PI 1/1024)  until sine(2 * PI 1023/1024)   */
    for(int i = 0; i < S; i++)
    {
        s->sine_table[i] = sin(2 * PI * i/S);
    }

    /* Success.  Return a pointer to the new state structure. */
    return s;
}

/* Processes a buffer of samples for the PLL block */
void pll(pll_state_def *s, const float x_in[], float y_out[])
{
    float amp = 0;  // Used for normalization on x_in
    for (int i = 0; i < BUFFER_SAMPLES; i++)
    {   
        //Normalize the amplitude of input signal x_in[]
        amp = amp + abs(x_in[i]);   //amp added to previous amp
        x_in[i] = x_in[i] / s->amp_est; //Normalizing x_in[]
        // First two actions of the PLL block
        s->zc = s->y * x_in[i]; // z[n] =  y[n] * x[n]
        s->vc = s->a1 * s->vp + s->b0 * s->zc + s->b1 * s->zp; // v[n] = z[n] * IIR Filter

        // calculation of accumulator for the sin table
        s->accum2 = s->accum2 + 2 * (s->f - (s->k * s->vc / (2 * PI))); //calculate double the frequency
        s->accum2 = s->accum2 - floor(s->accum2);
        
        // calculate output using sine table
        // objective is y_out[n + 1] = sin(2 * PI * acumm[n + 1])
        y_out[i] = s->sine_table[floor(s->accum2 * s->S)]; //Where S = 1024
        
        // update state variables
        s->y  = s->y;
        s->zp = s->zc;
        s->vp = s->vc;

    }
    //Actualize the amplitude value for next normalization.
    s->amp_est = amp/BUFFER_SAMPLES/(2/PI);
}
