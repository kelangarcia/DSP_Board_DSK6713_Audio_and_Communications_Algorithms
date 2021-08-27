/*****************************************************************************
 * delay.h
 *      Header defines for implementing a delay block.
 *****************************************************************************/

#ifndef _delay_h_
    #define _delay_h_

    /*-- Defines ---------------------------------------------------------------*/

    /* Size of buffer (samples).  Controls maximum delay. */
    #define DELAY_BUFFER_SIZE       16384

    /* Mask.  Used to implment circular buffer */
    #define DELAY_BUFFER_CMASK      (DELAY_BUFFER_SIZE-1)

    /* Which memory segment the data should get stored in */
    //#define DELAY_SEG_ID            0               /* IDRAM - fastest, but smallest */
    #define DELAY_SEG_ID            1               /* SRAM  - a bit slower, but bigger */

    /* Allows alignment of buffer on specified boundary. */
    #define DELAY_BUFFER_ALIGN            128             

    /* Samples required for 1MS of Delay */
    #define DSP_SAMPLES_PER_SEC				8000
    #define DELAY_SAMPLES_1MS               (DSP_SAMPLES_PER_SEC/1000)

    /*-- Structures ------------------------------------------------------------*/
    typedef struct
    {
        float buffer[DELAY_BUFFER_SIZE];
        unsigned int del;
        unsigned int h, t;
    } delay_state_def;

    /*-- Function Prototypes ---------------------------------------------------*/

    /* Initializes the delay block */
    delay_state_def *delay_init();

    /* Change delay parameters */
    void delay_modify(delay_state_def *s, unsigned int new_delay);

    /* Processes a buffer of samples for the delay block */
    void delay(delay_state_def *s, const float x_in[], float y_out[]);

#endif /* _delay_h_ */