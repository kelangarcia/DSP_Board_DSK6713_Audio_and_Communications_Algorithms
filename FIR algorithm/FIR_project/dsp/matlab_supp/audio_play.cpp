/*============================================================================
 * audio_play.cpp
 *      Plays a sound sample.
 *==========================================================================*/

// #define INITGUID                1

#include "mex.h"
#include "malloc.h"

#include <iostream.h>

#include "audio.h"

/*----------------------------------------------------------------------------
 * makeMexScalar()
 *      Creates a double array containing a single scalar.
 *--------------------------------------------------------------------------*/
mxArray *makeMexScalar(double val)
{
    mxArray *a;
    double *p;
    
    a = mxCreateDoubleMatrix(1, 1, mxREAL);
    p = mxGetPr(a);
    *p = val;

    return(a);
}

/*----------------------------------------------------------------------------
 * mexFunction
 *      Dispatch routine for other functions.
 *--------------------------------------------------------------------------*/
void mexFunction(
		 int nlhs, mxArray *plhs[],
		 int nrhs, const mxArray *prhs[])
{
    unsigned command;
    unsigned handle;
    int status;
    int rows, cols;
    signed short *data;
    unsigned int rate;
    
    if (nrhs < 2)
    {
        mexErrMsgTxt("usage: [] = audio_play(samples, rate);\n");
        return;
    }

    /* Check format of the data */
    if ((mxGetClassID(prhs[0]) != mxINT16_CLASS) || (mxIsComplex(prhs[0])))
    {
        mexErrMsgTxt("Data must be REAL and of type INT16_CLASS.");
        return;
    }
    
    rows = mxGetM(prhs[0]);
    cols = mxGetN(prhs[0]);
    if (rows != 2)
    {
        mexErrMsgTxt("Invalid number of rows.\n");
        return;
    }
    data = (signed short *) mxGetData(prhs[0]);

    /* Get the sample rate */
    rate = (int)mxGetScalar(prhs[1]);

    /* Play it */
    status = audioPlay(data, cols, rate);
}

