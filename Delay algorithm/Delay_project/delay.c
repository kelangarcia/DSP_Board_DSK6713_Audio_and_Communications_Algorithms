/***************************************************************************
 * delay.c
 *      Implements functions from delay block.
 ***************************************************************************/

#include <std.h>
#include <sys.h>
#include <dev.h>
#include <sio.h>

#include "delay.h"
#include "dsp_ap.h"

/*----------------------------------------------------------------------------
 * delay_init()
 *      This function initializes a delay block with a delay of 0.
 * Inputs:
 *      None.
 * Returns:
 *      0       An error ocurred
 *      other   A pointer to a new delay structure
 *--------------------------------------------------------------------------*/
delay_state_def *delay_init()
{
    delay_state_def *s;
    
    /* Allocate a new delay_state_def structure.  Holds state and parameters. */
    if ((s = (delay_state_def *)MEM_calloc(DELAY_SEG_ID, sizeof(delay_state_def), DELAY_BUFFER_ALIGN)) == NULL)
    {
        SYS_error("Unable to create an input delay floating-point buffer.", SYS_EUSER, 0);
        return(0);
    }
    
    /* Set initial delay to 0 */
    s->t = 0;
    s->h = 0;

    /* Success.  Return a pointer to the new state structure. */
    return(s);
}

/*----------------------------------------------------------------------------
 * delay_modify()
 *      Change operating parameters of the delay block.
 * Inputs:
 *      s               A pointer to the delay state structure
 *      new_delay       The new delay value
 *---------------------------------------------------------------------------*/
void delay_modify(delay_state_def *s, unsigned int new_delay)
{
    /* Check the requested delay */
    if (DELAY_BUFFER_SIZE < (new_delay + BUFFER_SAMPLES))
    {
        /* Make delay maximum */
        new_delay = DELAY_BUFFER_SIZE-BUFFER_SAMPLES;
    }
    
    /* Change the head of the buffer to obtain the requested delay.  Do circular. */
    s->t = (s->t + new_delay) & DELAY_BUFFER_CMASK;
}

/*----------------------------------------------------------------------------
 * delay()
 *      Process one buffer of samples with the delay block.
 *--------------------------------------------------------------------------*/
void delay(delay_state_def *s, const float x_in[], float y_out[])
{
    int i;

    /* Read all input samples into tail of buffer */
    for (i = 0; i < BUFFER_SAMPLES; i++)
    {
        s->buffer[s->t] = x_in[i];
        s->t++; s->t &= DELAY_BUFFER_CMASK;
    }

    /* Read all output samples from head of buffer */
    for (i=0; i<BUFFER_SAMPLES; i++)
    {
        y_out[i] = s->buffer[s->h];
        s->h = (s->h + 1) &  DELAY_BUFFER_CMASK;
    }
}