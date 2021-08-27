/*****************************************************************
							  dsp_ap.c
 *****************************************************************/

		/*		We include the header dsp_ap. because
				is the header of this file.			  */
#include "dsp_ap.h"

		/* 		We include the header fir.h in order
				to implement the fir block            */
#include "pll.h"

					// PLL struct states declared
pll_state_def *PLL_Left;
pll_state_def *PLL_Right;

/* Global Declarations.  Add as needed. */
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

		//		Values needed for the PLL COMPONENTS

	int k = 1;					  // Gain
	int D = 1;					  // Damping Factor
	float f = 0.1; 				  // Frequency
	float w_o = 2 * PI * f * 0.1; // Corner Frequency
	int T = 1;					  // Sample Period
	int S = 1024;				  // # of values in the sine table

	// Initialize the Left PLL block for the left channel
	PLL_Left = pll_init(k,D,f,w_o,T,S);

	//Checks if it was initialized correctly
	if (PLL_Left == 0)
	{
		/* Error */
		return 1;
	}

	// Initialize the Left PLL block for the right channel
	PLL_Right = pll_init(k,D,f,w_o,T);

	//Checks if it was initialized correctly
	if(PLL_Right == 0)
	{
		/* Error */
		return 1;
	}
	
    /* Success */
	return(0);
}



void dsp_process(
	const float inL[],
	const float inR[],
	float outL[],
	float outR[])
{
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

	/* 	Implements the left PLL block to the left input 
		array, and stores it in the left out array    */
	pll(PLL_Left, inL[], outL[]);

	/* 	Implements the right PLL block to the right input 
		array, and stores it in the right out array    */
	pll(PLL_Right, inR[], outR[]);

}


