// Have to do this define to get SwitchToThread()
#define _WIN32_WINNT            0x0400

#include "mex.h"
#include "malloc.h"
#include "Windows.h"
#include <iostream.h>
#import "c:\CCStudio_v3.1\cc\bin\rtdxint.dll"

using namespace RTDXINTLib;

#define MAX_INSTANCES   256

#define UNUSED_INSTANCE 0
#define WRITE_INSTANCE  1
#define READ_INSTANCE   2

#define Success 0
#define ENoDataAvailable -2147287010

/*
#define Failure -1
#define EEndOfLogFile -2 
*/

int init_done = 0;
IRtdxExpPtr rtdx[MAX_INSTANCES];
int rtdx_used[MAX_INSTANCES];

char str[256];

void uninit();

/*----------------------------------------------------------------------------
 * init()
 *      Checks to see if the initialization has happened.  If not, does
 *      proper initialization.
 *--------------------------------------------------------------------------*/
int init()
{
    if (!init_done)
    {
        mexPrintf("Initializing COM/RTDX system.\n");
        ::CoInitialize(NULL);

        memset(rtdx_used, 0, sizeof(rtdx_used));

        /* Register function to cleanup when MEX function exits. */
        mexAtExit(&uninit);

        init_done = 1;
    }

    return(0);
}

/*----------------------------------------------------------------------------
 * uninit()
 *      Uninitializes things.
 *--------------------------------------------------------------------------*/
void uninit()
{
    int i;
    
    /* Close any open interfaces. */
    for (i=0; i<MAX_INSTANCES; i++)
    {
        if (rtdx_used[i])
        {
            rtdx[i]->Close();
            rtdx[i].Release();
            rtdx_used[i] = 0;
        }
    }

    /* Shut down COM */
    ::CoUninitialize();
}

/*----------------------------------------------------------------------------
 * findFreeSlot
 *      Looks for free slot in the table of instances.
 *--------------------------------------------------------------------------*/
int findFreeSlot()
{
    int i;

    for (i=0; i<MAX_INSTANCES; i++)
    {
        if (rtdx_used[i] == 0)
        {
            return(i);
        }
    }

    return(-1);
}

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
 * cmd_open_write_channel
 *      Opens a channel for writing via RTDX to the target.
 *--------------------------------------------------------------------------*/
int cmd_open_write_channel(
                            int nlhs, mxArray *plhs[],
                            int nrhs, const mxArray *prhs[])
{
    int status;
    int ccode;
    char *name;
    int handle;
    HRESULT hr;
    
    if (nrhs < 2)
    {
        mexPrintf("Usage: rtdx(OPEN_WRITE, str_name)\n");
        ccode = -1;
        goto backout_base;
    }

    if ((handle = findFreeSlot()) < 0)
    {
        mexErrMsgTxt("All interface slots used.");
        ccode = -2;
        goto backout_base;
    }
    
    if ((name = mxArrayToString(prhs[1])) == 0)
    {
        mexErrMsgTxt("Write interface name string was invalid.");
        ccode = -3;
        goto backout_base;
    }

    hr = rtdx[handle].CreateInstance(L"RTDX");
    if (FAILED(hr))
    {
        mexErrMsgTxt("Unable to create instance.");
        ccode = -4;
        goto backout_base;
    }

    if ((status = rtdx[handle]->Open(name, "W")) != Success)
    {
        sprintf(str, "Unable to open output channel named \"%s\".", name);
        mexErrMsgTxt(str);
        
        ccode = -5;
        goto backout_instance;
    }

    /* Success */
    rtdx_used[handle] = WRITE_INSTANCE;
    plhs[0] = makeMexScalar(handle);
    return(0);
    
 backout_instance:
    rtdx[handle].Release();
    
 backout_base:
    return(ccode);

}

/*----------------------------------------------------------------------------
 * cmd_open_read_channel
 *      Opens a channel for reading via RTDX from the target.
 *--------------------------------------------------------------------------*/
int cmd_open_read_channel(
                            int nlhs, mxArray *plhs[],
                            int nrhs, const mxArray *prhs[])
{
    int status;
    int ccode;
    char *name;
    int handle;
    HRESULT hr;
    
    if (nrhs < 2)
    {
        mexPrintf("Usage: rtdx(OPEN_READ, str_name)\n");
        ccode = -1;
        goto backout_base;
    }

    if ((handle = findFreeSlot()) < 0)
    {
        mexErrMsgTxt("All interface names used.");
        ccode = -2;
        goto backout_base;
    }
    
    if ((name = mxArrayToString(prhs[1])) == 0)
    {
        mexErrMsgTxt("Write interface name string was invalid.");
        ccode = -3;
        goto backout_base;
    }

    hr = rtdx[handle].CreateInstance(L"RTDX");
    if (FAILED(hr))
    {
        mexErrMsgTxt("Unable to create instance.");
        ccode = -4;
        goto backout_base;
    }

    if ((status = rtdx[handle]->Open(name, "R")) != Success)
    {
        sprintf(str, "Unable to open input channel named \"%s\".", name);
        mexErrMsgTxt(str);
        
        ccode = -5;
        goto backout_instance;
    }

    /* Success */
    rtdx_used[handle] = READ_INSTANCE;
    plhs[0] = makeMexScalar(handle);
    return(0);
    
 backout_instance:
    rtdx[handle].Release();
    
 backout_base:
    return(ccode);

}

/*----------------------------------------------------------------------------
 * cmd_writeI4
 *      Writes specified number of 32-bit integers to the board.
 *--------------------------------------------------------------------------*/
int cmd_writeI4(
		 int nlhs, mxArray *plhs[],
		 int nrhs, const mxArray *prhs[])
{
    int ccode;
    int handle;
    int rows, cols;
    int i;
    int *p;
    long bufferstate;
    
    if (nrhs < 3)
    {
        mexPrintf("Usage: rtdx(WRITE_I4, handle, data)\n");
        ccode = -1;
        goto backout_base;
    }

    handle = (int)mxGetScalar(prhs[1]);
    if ((handle < 0) || (handle >= MAX_INSTANCES) || (rtdx_used[handle] != WRITE_INSTANCE))
    {
        mexErrMsgTxt("Invalid handle.");
        ccode = -2;
        goto backout_base;
    }

    rows = mxGetM(prhs[2]);
    cols = mxGetN(prhs[2]);
    p = (int *) mxGetData(prhs[2]);
    for (i=0; i<rows*cols; i++)
    {
        rtdx[handle]->WriteI4(p[i], &bufferstate);
    }

    return(0);
    
 backout_base:
    return(ccode);
}

/*----------------------------------------------------------------------------
 * cmd_readI4
 *      Reads specified number of 32-bit integers from the board.
 *--------------------------------------------------------------------------*/
int cmd_readI4(
		 int nlhs, mxArray *plhs[],
		 int nrhs, const mxArray *prhs[])
{
    int ccode;
    int handle;
    int size;
    int i, j;
    long *p, *p1;
    int dims[1];
    mxArray *data;
    mxArray *data1;    
    int status;
    int timeout;
    int got_timeout;
    int got_data;
    
    DWORD tick_start, tick_end;

    if (nrhs < 4)
    {
        mexPrintf("Usage: [data] = rtdx(READ_I4, handle, count, timeout)\n");
        ccode = -1;
        goto backout_base;
    }

    handle = (int)mxGetScalar(prhs[1]);
    if ((handle < 0) || (handle >= MAX_INSTANCES) || (rtdx_used[handle] != READ_INSTANCE))
    {
        mexErrMsgTxt("Invalid handle.");
        ccode = -2;
        goto backout_base;
    }

    if ((size = (int)mxGetScalar(prhs[2])) <= 0)
    {
        mexErrMsgTxt("Invalid size.");
        ccode = -3;
        goto backout_base;
    }
    dims[0] = size;

    if ((timeout = (int)mxGetScalar(prhs[3])) < 0.0)
    {
        mexErrMsgTxt("Invalid timeout.");
        ccode = -4;
        goto backout_base;
    }

    if ((data = mxCreateNumericArray(1, dims, mxINT32_CLASS, mxREAL)) == 0)
    {
        mexErrMsgTxt("Unable to create data array.");
        ccode = -5;
        goto backout_base;
    }

    /* Get starting clock */
    tick_start = GetTickCount();

    p = (long *) mxGetData(data);

    got_timeout = 0;
    for (i=0; i<size; i++)
    {
        got_data = 0;
        do
        {
            status = rtdx[handle]->ReadI4(p+i);
            switch(status)
            {
                case Success:
                    /* Got the word */
                    got_data = 1;
                    break;

                case ENoDataAvailable:
                    break;

                default:
                    mexPrintf("status = %d\n", status);
                    break;
                    
                    /*
                case Failure:
                default:
                    mexErrMsgTxt("Error reading data.");
                    ccode = -6;
                    goto backout_base;

                case EEndOfLogFile:
                    got_timeout = 1;
                    break;
                    */
                    
            }

            /* Check for a timeout */
            if ((timeout >= 0) && (GetTickCount() >= (tick_start + timeout)))
            {
                /* Timeout */
                got_timeout = 1;
                break;
            }

            /* Switch tasks as to not hog CPU */
            SwitchToThread();
        }
        while (!got_timeout && !got_data);
        
        if (got_timeout)
        {
            break;
        }
    }

    /* Return how many words we got */
    dims[0] = i;
    if ((data1 = mxCreateNumericArray(1, dims, mxINT32_CLASS, mxREAL)) == 0)
    {
        mexErrMsgTxt("Unable to create data array.");
        ccode = -6;
        goto backout_data;
    }
    p1 = (long *) mxGetData(data1);

    /* Create the output array */
    for (j=0; j<i; j++)
    {
        p1[j] = p[j];
    }

    /* Free the temporary array */
    mxDestroyArray(data);
    
    /* Success */
    plhs[0] = data1;
    return(0);
    
 backout_data:
    mxDestroyArray(data);

 backout_base:
    return(ccode);
}
    
/*----------------------------------------------------------------------------
 * cmd_close_channel
 *      Closes a read or write channel.
 *--------------------------------------------------------------------------*/
int cmd_close_channel(
		 int nlhs, mxArray *plhs[],
		 int nrhs, const mxArray *prhs[])
{
    int ccode;
    int handle;
    
    if (nrhs < 2)
    {
        mexPrintf("Usage: rtdx(CLOSE, handle)\n");
        ccode = -1;
        goto backout_base;
    }

    handle = (int)mxGetScalar(prhs[1]);
    if ((handle < 0) || (handle >= MAX_INSTANCES) || (rtdx_used[handle] == UNUSED_INSTANCE))
    {
        mexErrMsgTxt("Invalid handle.");
        ccode = -2;
        goto backout_base;
    }

    rtdx[handle]->Close();
    rtdx[handle].Release();
    rtdx_used[handle] = UNUSED_INSTANCE;
    
    return(0);
    
 backout_base:

    return(ccode);
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

    mxArray *data;

    if (init() != 0)
    {
        mexErrMsgTxt("Unable to initialize.\n");
        return;
    }
    
    if (nrhs < 1)
    {
        mexErrMsgTxt("usage: [status, ...] = rtdx(command, ...);\n");
        return;
    }
        
    /* Get inputs. */
    command = (unsigned)mxGetScalar(prhs[0]);

    /* Perform operation according to command. */
    switch(command)
    {
        case 0:
            /* NOP */
            break;

        case 1:
            /* Open write channel */
            cmd_open_write_channel(nlhs, plhs, nrhs, prhs);
            break;

        case 2:
            /* Write 32-bit integers */
            cmd_writeI4(nlhs, plhs, nrhs, prhs);
            break;

        case 10:
            /* Open read channel */
            cmd_open_read_channel(nlhs, plhs, nrhs, prhs);
            break;

        case 11:
            /* Read 32-bit integers */
            cmd_readI4(nlhs, plhs, nrhs, prhs);
            break;

        case 20:
            /* Close a channel */
            cmd_close_channel(nlhs, plhs, nrhs, prhs);
            break;

        case 6:
            /* Flush. */
            //cmd_flush(nlhs, plhs, nrhs, prhs);
            break;

        case 7:
            /* Status of write */
            //cmd_status_of_write(nlhs, plhs, nrhs, prhs);
            break;

        default:
            mexErrMsgTxt("Invalid command.\n");
            return;
    }
}

