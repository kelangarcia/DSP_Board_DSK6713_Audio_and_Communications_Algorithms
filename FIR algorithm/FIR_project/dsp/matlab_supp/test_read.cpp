#include "windows.h"
#include <iostream.h>
#import "c:\CCStudio_v3.1\cc\bin\rtdxint.dll"

using namespace RTDXINTLib;

#define MAX_INSTANCES   256

#define Success 0
/*
#define Failure -1
#define EEndOfLogFile -2 
*/
#define ENoDataAvailable -2147287010

IRtdxExpPtr rtdx;

int main(int argc, char *argv[])
{
    HRESULT hr;
    int data;
    
    ::CoInitialize(NULL);

    hr = rtdx.CreateInstance(L"RTDX");
    if (FAILED(hr))
    {
        printf("Unable to create instance.");
        exit(1);
    }
    
    if ((status = rtdx->Open(name, "R")) != Success)
    {
        printf("Unable to open input channel named \"%s\".", name);
        exit(1);
    }

    for (i=0; i<100; )
    {
        status = rtdx->ReadI4(&data);
        switch(status)
        {
            case Success:
                /* Got the word */
                got_data = 1;
                printf("%d: read %d.\n", i+1, data);
                i++;
                break;
                
            case ENoDataAvailable:
                printf("No data.\n");
                break;
                
            default:
                mexPrintf("status = %d\n", status);
                break;
        }
        Sleep(100);
    }

    rtdx->Close();
    rtdx.Release();

    return(0);
}

