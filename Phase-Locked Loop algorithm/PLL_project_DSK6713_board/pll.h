/*****************************************************************
                                pll.h
            Header defines for implementing an PLL block.
 *****************************************************************/
#ifndef _pll_h_
    #define _pll_h_

    /*-------------------- Defines -----------------------*/

    // Which memory segment the data should get stored in
    //#define FIR_SEG_ID 0 // IDRAM - fastest, but smallest
    #define PLL_SEG_ID   1 // SRAM  - a bit slower, but bigger

    /* Allows alignment of buffer on specified boundary. */
    #define PLL_BUFFER_ALIGN            128             

    /*------------------- Structures -------------------*/
    typedef struct
    {
        // 		Values needed for the PLL COMPONENTS
        int k;      // Gain
        float f;    // Frequency
        int S;      // # of values in the sine table


        //      Coefficients of the IIR Filter
        float a1;
        float b0;
        float b1;
        
        //      Values of the functions during the PLL process
        float zp; // Multiplication between previous x_in and y_out
        float zc; // Multiplication between current x_in and y_out
        float vp; // Result of zp in the IIR filter 
        float vc; // Result of zc in the IIR filter 
        float y;  // Output value

        // needed variables
        float accum;//Accumulator,used for the frequency in the sin table.
        float amp_est;// Value used to normalizedb the current input
        float sine_table[1024];//Table with different values of sine.
    } pll_state_def;

    /*---------------- Function Prototypes ----------------*/

    /* Initializes the PLL block */
    pll_state_def *pll_init(int k, int  D, float f, float w_o, int T, int S);

    /* Processes a buffer of samples for the PLL block */
    void pll(pll_state_def *s, const float x_in[], float y_out[]);
    

#endif /* _pll_h_ */