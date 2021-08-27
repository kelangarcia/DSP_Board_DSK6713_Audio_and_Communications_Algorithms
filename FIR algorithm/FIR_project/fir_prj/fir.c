/*****************************************************************
                                fir.c
 *****************************************************************/

// Libraries that were used in delay.c
#include <std.h>
#include <sys.h>
#include <dev.h>
#include <sio.h>


// Include the headers of the files needed
#include "fir.h"
#include "dsp_ap.h"



fir_state_def *fir_init(int len, float *h){
    /*  fir_init()
        This function initializes a fir block.
        Inputs:
        len = length of the filter
        *h = pointer to coefficients
        Returns:
        0       An error ocurred
        other   A pointer to a new FIR structure */

    // Creating a struct named s
    fir_state_def *s;

    // Allocating te struct in te memory segment FIR_SEG_ID
    s = (fir_state_def *)MEM_calloc(FIR_SEG_ID, sizeof(fir_state_def), FIR_BUFFER_ALIGN);

    //Checking if the struct was created correctly and sending a message of error if not
    if(s == NULL){
        SYS_error("Unable to create an input delay floating-point buffer.", SYS_EUSER, 0);
        return(0);     
    }

    // Setting all variables inside the struct, so it can be returned in one variable.
    s->len = len; //Length of the filter
    s->h = h;  //coefficients
    s->t = 0; // tail

    /* Success.  Return a pointer to the new state structure. */
    return(s);
}

void fir(fir_state_def *s, const float x_in[], float y_out[]){
    /*  fir_init()
        This Function was explained in task 5
        it initializes a fir block.
        Inputs:
        len = length of the filter
        *h = pointer to coefficients
        Returns:
        0       An error ocurred
        other   A pointer to a new delay structure */

    /* This Function was explained in task 5
         This loop walks between y_out array */
    for(int i = 0; i < BUFFER_SAMPLES; i++)
    {
        x_in[s->t] = s->h[i]; //Coefficients into buffer

        //updating tail in a circular way
        s->t = (s->t + 1) & FIR_BUFFER_CMASK;

        //updating ptr in a circular way
        int ptr = (s->t + FIR_BUFFER_SIZE - 1) & FIR_BUFFER_CMASK;
        float sum = 0;// sum variable needed

        /* This loop does the convolution*/
        for (int j = 0; j < s->len; j++)
        {
            sum = sum + s->h[j] * x_in[ptr]; //convolution of h&x
            //updating ptr in a circular way
            ptr = (ptr + FIR_BUFFER_SIZE - 1) & FIR_BUFFER_CMASK;
        }
        y_out[i] = sum; //FINAL RESULT in y_out[n]
    }
}