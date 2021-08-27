/*****************************************************************
                                fir.h
            Header defines for implementing an FIR block.
 *****************************************************************/
#ifndef _fir_h_
    #define _fir_h_

    /*-------------------- Defines -----------------------*/

    /* Size of buffer (samples).  Maximum filter length. */
    #define FIR_BUFFER_SIZE       512

    /* Mask.  Used to implment circular buffer */
    #define FIR_BUFFER_CMASK      (FIR_BUFFER_SIZE-1)

    // Which memory segment the data should get stored in
    //#define FIR_SEG_ID 0 // IDRAM - fastest, but smallest
    #define FIR_SEG_ID   1 // SRAM  - a bit slower, but bigger

    /* Allows alignment of buffer on specified boundary. */
    #define FIR_BUFFER_ALIGN            128             

    /*------------------- Structures -------------------*/
    typedef struct
    {
        float buffer[FIR_BUFFER_SIZE];
        float len;
        float *h;
        unsigned int t;
    } fir_state_def;

    /*---------------- Function Prototypes ----------------*/

    /* Initializes the fir block */
    fir_state_def *fir_init(int len, float *h);

    /* Processes a buffer of samples for the fir block */
    void fir(fir_state_def *s, const float x_in[], float y_out[]);

#endif /* _fir_h_ */