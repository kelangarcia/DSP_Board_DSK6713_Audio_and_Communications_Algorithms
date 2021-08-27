/*============================================================================
 * audio_record.cpp
 *      Records a sound sample.
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
    unsigned int n_samples, rate;
    mxArray *output;
    int dims[2];
    
    if (nrhs < 2)
    {
        mexErrMsgTxt("usage: [data] = audio_record(n_samples, rate);\n");
        return;
    }

    /* Check format of the data */
    n_samples = (int)mxGetScalar(prhs[0]);
    rate = (int)mxGetScalar(prhs[1]);

    /* Create a variable to hold the data */
    dims[0] = 2;
    dims[1] = n_samples;
    if ((output = mxCreateNumericArray(2, dims, mxINT16_CLASS, mxREAL)) == 0)
    {
        mexErrMsgTxt("Unable to create output array.");
        return;
    }

    data = (signed short *) mxGetData(output);
    
    /* Record the sound */
    if ((status = audioRecord(data, n_samples, rate)) != 0)
    {
        mxDestroyArray(output);
        return;
    }

    plhs[0] = output;
}
    

