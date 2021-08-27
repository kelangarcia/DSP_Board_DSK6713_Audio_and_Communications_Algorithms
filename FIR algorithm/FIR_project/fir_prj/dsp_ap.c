/*****************************************************************
							  dsp_ap.c
 *****************************************************************/

		/*		We include the header dsp_ap. because
				is the header of this file.			  */
#include "dsp_ap.h"

		/* 		We include the header fir.h in order
				to implement the fir block            */
#include "fir.h"

		/*		We include the header rc1_taps.h in order
				to have acces to the rc1_taps array	  */
#include "rc1_taps.h" 

					// FIR struct states declared
fir_state_def *FIR_Left;
fir_state_def *FIR_Right;

				// Global Declarations.  Add as needed.
float mybuffer[BUFFER_SAMPLES];

int dsp_init(){

	/*
	 dsp_init
		This function will be called when the board first starts.
		In here you should allocate buffers or global things
	   	you will need later during actual processing.
	 Inputs:
		None
	 Outputs:
		0	Success
		1	Error
	*/

	// Initialize the Left FIR block for the left channel
	FIR_Left = fir_init(rc1_taps_LEN, rc1_taps);
	
	//Checks if it was initialized correctly
    if (FIR_Left == 0){
        /* Error */
        return(1);
    }

	// Initialize the Right FIR block for the right channel
	FIR_Right = fir_init(rc1_taps_LEN, rc1_taps);
	
	//Checks if it was initialized correctly
    if (FIR_Right == 0){
        /* Error */
        return(1);
    }

    /* Success */
    return(0)
}

void dsp_process(
	const float inL[],
	const float inR[],
	float outL[],
	float outR[]){

	/* 
	* dsp_process
	* This function is what actually processes input samples
	* and generates output samples .
	* Inputs :
	* inL ,inR Array of left and Right input samples .
	* outL , outR Array of left and Right output samples .
	*
	* Outputs :
	* 0 Success
	* 1 Error
	*/

	/* 	Implements the left FIR block to the left input 
		array, and stores it in the left out array    */
	fir(FIR_Left, inL[], outL[]);

	/* 	Implements the right FIR block to the right input 
		array, and stores it in the right out array    */
	fir(FIR_Right, inR[], outR[]);
}