/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DDK 1.11.00.00 11-04-03 (ddk-b13)" */
/* 
 *  ======== dsk6x11_edma_ad535.h ========
 */

#ifndef DSK6X11_EDMA_AD535_
#define DSK6X11_EDMA_AD535_

//#include <iom.h>
#include <csl_edma.h>


#ifdef __cplusplus
extern "C" {
#endif

/* Version number definition */
#define DSK6X11_EDMA_AD535_VERSION_1 0xAB01

#define DSK6X11_EDMA_AD535_NUMREGS 7

#define DSK6X11_EDMA_AD535_INTR_MASK_DEFAULT 1


/*
 * Perform voice channel initialisation of TLC320AD535 Codec.
 * AD535 has 2 serial port channels: data and voice.
 * The data channel is controlled by registers 1,2 (Reg 0 = NOP).
 * The voice channel is controlled by Registers 3,4,5,6.
 * Only the voice channel is used on the 6x11.
 */
#define DSK6X11_AD535_DEFAULTPARAMS {                                    \
    0x0000, /* AD535 control register 0 default -- dummy read/write */   \
    0x0000, /* AD535 control register 1 default -- data chan not used */ \
    0x0000, /* AD535 control register 2 default -- data chan not used */ \
    0x0006, /* AD535 control register 3 default dB */                    \
    0x0000, /* AD535 control register 4 default */                       \
    0x0002, /* AD535 control register 5 default -- input gain 0 dB */    \
    0x0000  /* AD535 control register 6 default -- output gain 0 dB */   \
}

/* Default IRQ number for EDMA is 8 */          
#define DSK6X11_EDMA_AD535_EDMA_ID_DEFAULT  8

#define DSK6X11_EDMA_AD535_DEFAULT_DEVPARAMS {                           \
    DSK6X11_EDMA_AD535_VERSION_1,     /* version number */               \
    TRUE,       /* Buffers are in external memory by default */          \
    DSK6X11_EDMA_AD535_EDMA_ID_DEFAULT,                                  \
    DSK6X11_AD535_DEFAULTPARAMS,                                         \
    DSK6X11_EDMA_AD535_INTR_MASK_DEFAULT,                                \
    EDMA_OPT_PRI_HIGH                                                    \
}

/* Driver function table to be used by applications. */
extern far IOM_Fxns DSK6X11_EDMA_AD535_FXNS;

/* Device setup parameters */
typedef struct DSK6X11_EDMA_AD535_DevParams {
    Int versionId;   /* Set to the version number used by the application */
    Bool cacheCalls; /* Set to TRUE if buffers are in external memory */
    Int irqId;       /* IRQ number to use for EDMA interrupt */
    Int reg[DSK6X11_EDMA_AD535_NUMREGS]; /* AD535 registers */
    Uns intrMask;    /* Interrupt mask, set while executing EDMA int ISR */
    Int edmaPriority;/* priority of EDMA transfers */
} DSK6X11_EDMA_AD535_DevParams;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* DSK6X11_EDMA_AD535_ */


