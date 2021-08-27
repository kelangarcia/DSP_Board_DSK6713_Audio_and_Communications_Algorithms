/*============================================================================
 * dsp_top.c
 *      This is the top-level file in your DSP program.  You should
 *		not edit it unless you have a need to.  You should be able
 *		to do everything you need by changing dsp_ap.c and dsp_ap.h.
 *==========================================================================*/

#define FP		float

/* DSP/BIOS includes */
#include <std.h>
#include <sys.h>
#include <dev.h>
#include <sio.h>
#include <swi.h>
//#include <log.h>
//#include <mem.h>

/* DSP/BIOS configuration */
#include "dsp_apcfg.h"

/* Register definitions for the DSK board */
#include "dsk_registers.h"

/* User defines */
#include "dsp_ap.h"

#include "dsk6713.h"
#include "dsk6713_led.h"

/*
 * Aligning buffers on 128 byte boundary allows them to work optimally
 * with the L2 cache on the C6x.
 */
#define BUFFER_SIZE     (BUFFER_SAMPLES*4)
#define BUFFER_ALIGN	128

/* Controlls where memory gets allocated.  SEG_ID=1 uses SDRAM */
#define SEG_ID          1

/*
 * Input and output streams handles which are used with the
 * DSP/BIOS API to handle our buffers for us.
 */
SIO_Handle inStream, outStream;

/* Seems like this should be in the .cfg header, but it's not. */
extern SWI_Obj swiEcho;

/* Codec definitions */
#include "aic23.h"

#ifdef TARGET_DSK6713

/*--------------------------------------------------------------------------
 * aic23param
 *      Here we set up the default values that are actually used by
 *      the device driver.  This structure gets pointed to by the device
 *      driver entry in the DSP/BIOS settings.  See aic23.h for more information
 *      on this structure.
 *--------------------------------------------------------------------------*/
DSK6416_EDMA_AIC23_DevParams aic23param =
{
    0xab01,
    TRUE,
    8,
    {AIC23_REG01_0dB,
     AIC23_REG01_0dB,
     AIC23_REG23(-10),
     AIC23_REG23(-10),
	DSP_SOURCE,
     AIC23_REG5,
     AIC23_REG6,
     AIC23_REG7,
	 DSP_SAMPLE_RATE,
     AIC23_REG9
    },
    1,
    1
};

#endif

void DSK6713_waitusec(unsigned int delay);

/*----------------------------------------------------------------------------
 * flash_error()
 *		Do a flashing light to show error.
 *--------------------------------------------------------------------------*/
void flash_error(int cycles)
{
	for ( ; ; )
	{
		USER_REG &= ~0xf;
		DSK6713_waitusec(1000*cycles/2);
		USER_REG |= 0xf;
		DSK6713_waitusec(1000*cycles/2);
	}
}

/*----------------------------------------------------------------------------
 * init()
 *      Perform initialization.
 *--------------------------------------------------------------------------*/
void init()
{
    /* Structures for setting up callback */
    SIO_Attrs   attr;
    DEV_Callback callback;

    Ptr p;
    Int i;
    
    /*
     * Set up callbacks to be used when a full receive buffer is
     * ready.  We need to use a callback, because the SIO routines
     * can block, which is not allowed on an interrupt.
     */
    callback.fxn = (Fxn)SWI_andnHook;   /* This is a BIOS routine that
                                         * fields calls for us. */
    callback.arg0 = (Arg)&swiEcho;      /* Our echo routine */
    callback.arg1 = (Arg)0x1;           /* IOM_INPUT mode */

    /* Attributes for our streams. */
    attr =       SIO_ATTRS;  /* Default values? */
    attr.nbufs = NUM_BUFFERS;
    attr.align = BUFFER_ALIGN;
    attr.model = SIO_ISSUERECLAIM;
    attr.callback = (DEV_Callback *)&callback;
    attr.segid = SEG_ID;
    
    /* Create the input stream */
    if ((inStream = SIO_create("/dioCodec", SIO_INPUT, BUFFER_SIZE, &attr)) == NULL)
    {

        LOG_printf(&LOG_system, "Creation of input stream failed.");
		flash_error(100);
    }

    /* Create the output stream. */
    callback.arg1 = (Arg)0x2;   /* IOM_OUTPUT mode */
    if ((outStream = SIO_create("/dioCodec", SIO_OUTPUT, BUFFER_SIZE, &attr)) == NULL)
    {
        LOG_printf(&LOG_system, "Creation of output stream failed.");
		flash_error(100);
    }

    /* Issue buffers to the output streams */
    for (i=0; i<NUM_BUFFERS; i++)
    {
        if ((p = (Ptr)MEM_calloc(SEG_ID, BUFFER_SIZE, BUFFER_ALIGN)) == NULL)
        {
            SYS_abort("Unable to create an output buffer.");
			flash_error(100);
        }

        /* Hand it off to the stream */
        if (SIO_issue(outStream, p, SIO_bufsize(outStream), NULL) != SYS_OK)
        {
            LOG_printf(&LOG_system, "Unable to issue buffer to output stream.");
	 		flash_error(100);
       }
    }
    
    /* Issue buffers to the input streams */
    for (i=0; i<NUM_BUFFERS; i++)
    {
        if ((p = (Ptr)MEM_calloc(SEG_ID, BUFFER_SIZE, BUFFER_ALIGN)) == NULL)
        {
            LOG_printf(&LOG_system, "Unable to create an input buffer.");
			flash_error(100);
        }

        /* Hand it off to the stream */
        if (SIO_issue(inStream, p, SIO_bufsize(inStream), NULL) != SYS_OK)
        {
           LOG_printf(&LOG_system, "Unable to issue buffer to input stream.");
			flash_error(100);
        }
    }

	/* Call the user routine */
	if (dsp_init())
	{
		flash_error(200);
	}
}

/*----------------------------------------------------------------------------
 * echo
 *      This function just gets a full buffer from the input stream and
 *      copies its contents verbatim to the output stream.
 *--------------------------------------------------------------------------*/
void io()
{
    /* For 6713, stereo codec, so we (should) have 32 bit words */
    unsigned int       *inbuf, *outbuf;
    signed short       *p;
    Int numBytes;
    Int i;
	static float inL[BUFFER_SAMPLES], inR[BUFFER_SAMPLES];
	static float outL[BUFFER_SAMPLES], outR[BUFFER_SAMPLES];
	static int buff_count = 0;

    /* Get a full buffer from the input stram */
    if ((numBytes = SIO_reclaim(inStream, (Ptr *)&inbuf, NULL)) < 0)
    {
        LOG_printf(&LOG_system, "Error reclaiming buffer from input stream.");
		flash_error(100);
    }

    /* Get an empty buffer from the output stream to write data to */
    if (SIO_reclaim(outStream, (Ptr *)&outbuf, NULL) < 0)
    {
        LOG_printf(&LOG_system, "Error reclaiming buffer from output stream.");
		flash_error(100);
    }
    
    /* Copy data to input buffer. */
    p = (signed short *) inbuf;
    for (i=0; i<BUFFER_SAMPLES; i++)
    {
        
        inL[i] = *(p++)/(FP)SCALE;         /* Just take samples from left channel */
        inR[i] = *(p++)/(FP)SCALE; 
    }

	/* Check for loop-back mode */
	if ((USER_REG >> 4) & (0x8))
	{
		/* Copy input to output. */
		for (i=0; i<BUFFER_SAMPLES; i++)
		{
			outL[i] = inL[i];
			outR[i] = inR[i];
		}
	}
	else
	{
		/* Call the user's process routine. */
		dsp_process(inL, inR, outL, outR);
	}

    /* Convert floating point output samples back to integers */
    p = (signed short *) outbuf;
    for (i=0; i<BUFFER_SAMPLES; i++)
    {
        /* Put output sample to both left and right channel */
        *(p++) = (signed short)(outL[i]*(FP)SCALE);
        *(p++) = (signed short)(outR[i]*(FP)SCALE);
    }	

    /* Send our loaded buffer back to the output stream */
    if (SIO_issue(outStream, outbuf, numBytes, NULL) != SYS_OK)
    {
        LOG_printf(&LOG_system, "Error issuing full buffer to output stream.");
		flash_error(100);
    }
    
    /* Now, give the empty buffer back to the input stream */
    if (SIO_issue(inStream, inbuf, SIO_bufsize(inStream), NULL) != SYS_OK)
    {
        LOG_printf(&LOG_system, "Error issuing free buffer to input stream.");
		flash_error(100);
    }

	/* Do flashing to show progress */
	buff_count++;
	if (buff_count == 100)
	{
		USER_REG = USER_REG ^ 0x8;
		buff_count = 0;
	}

}

/*----------------------------------------------------------------------------
 * main()
 *      Perform initialization of streams and buffers.  Then returns
 *      to the system idle loop.
 *--------------------------------------------------------------------------*/
void main()
{
    init();
    
    /*
     * Now just passthrough to the idle loop.  Everything else that
     * happens occurs via software interrupt, and our echo routine is
     * called through a callback.
     */
}
