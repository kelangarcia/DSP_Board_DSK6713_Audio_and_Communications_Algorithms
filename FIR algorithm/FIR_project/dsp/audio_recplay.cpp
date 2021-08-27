/*============================================================================
 * audio_recplay.cpp
 *      Records and plays at the same time.
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
    signed short *data_play, *data_rec;
    unsigned int n_samples, rate;
    mxArray *output;
    int dims[2];
    
    if (nrhs < 2)
    {
        mexErrMsgTxt("usage: [rec_samples] = audio_recplay(play_samples, rate);\n");
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
    n_samples = cols;
    data_play = (signed short *) mxGetData(prhs[0]);

    /* Get record rate */
    rate = (int)mxGetScalar(prhs[1]);

    /* Create a variable to hold the data */
    dims[0] = 2;
    dims[1] = n_samples;
    if ((output = mxCreateNumericArray(2, dims, mxINT16_CLASS, mxREAL)) == 0)
    {
        mexErrMsgTxt("Unable to create output array.");
        return;
    }
    data_rec = (signed short *) mxGetData(output);
    
    /* Play/Record the sound */
    if ((status = audioRecPlay(data_play, data_rec, n_samples, rate)) != 0)
    {
        mxDestroyArray(output);
        return;
    }

    plhs[0] = output;
}
    

