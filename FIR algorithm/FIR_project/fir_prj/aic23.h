/*
 *  ======== aic23.h ========
 *
 *  Header file for the AIC23 codec driver implementation specific to the 
 *  Spectrum Digital EVM5509 board. It contains
 *  control word bit-definition macros and declaration of public functions.
 */

#ifndef AIC23_
#define AIC23_

#include <std.h>

//#include <csl.h>

/*
 *  AIC23 Control registers
 *  
 *  There are 10 control registers (+the reset reg.) on AIC23, each 9 bits wide.
 *  The address of a control register is 7 bits wide: together,
 *  the address of a register and its content form a 16-bit control word,
 *  the address occupying the uppermost 7 bits and the content lowermost 9.
 */
#define AIC23_NUMREGS 10
#define AIC23_RESET   15

/*
 *  Codec registers are kept in an object of the structure below.
 */
typedef struct AIC23_Params {
    Uns regs[ AIC23_NUMREGS ];
} AIC23_Params;

/*----------------------------------------------------------------------------
 * AIC23 codec device parameters
 *--------------------------------------------------------------------------*/

typedef struct DSK6416_EDMA_AIC23_DevParams {
    Int versionId;
    Bool cacheCalls;
    Int irqId;
    AIC23_Params aic23;
    Uns intrMask;
    Uns edmaPriority;
} DSK6416_EDMA_AIC23_DevParams;

/*----------------------------------------------------------------------------
 * AIC23 Parameters
 *      Here you can change the setting of volume, input source, sample
 *      rate, etc.
 *--------------------------------------------------------------------------*/

/*
 *  A macro that let us define 9-bit register values as array of zeroes/ones
 */
#define AIC23_9BITWORD( b8, b7, b6, b5, b4, b3, b2, b1, b0 )              \
    ((Uns)( 1*(b0) +  2*(b1) +  4*(b2) +   8*(b3) +                       \
           16*(b4) + 32*(b5) + 64*(b6) + 128*(b7) + 256*(b8) ))

/*-- REG0/1: Left/Right Input Volume -----------------------------------------------*/

#define AIC23_REG01_0dB                                                \
    AIC23_9BITWORD(         /* REG 0: left input ch. volume control */    \
        0,                  /* simultaneous left/right volume: disabled*/ \
        0,                  /* left line input mute: disabled */          \
        0,0,                /* reserved */                                \
        1,0,1,1,1           /* left line input volume: 0 dB */            \
    )                                                                     

/*-- REG2/3: Left/Right Headphone Volume ------------------------------------*/

#define AIC23_REG23(vol_db)  \
    (AIC23_9BITWORD(         /* REG 2/3: left/right ch. headphone volume control*/ \
        1,                  /* simultaneous left/right volume: enabled */ \
        1,                  /* left channel zero-cross detect: enabled */ \
        0,0,0,0,0,0,0) | (127 - (6 - (vol_db))))
                            
#define AIC23_REG23_0dB      \
    AIC23_9BITWORD(         /* REG 2/3: left/right ch. headphone volume control*/ \
        1,                  /* simultaneous left/right volume: enabled */ \
        1,                  /* left channel zero-cross detect: enabled */ \
        1,1,1,1,0,0,1       /* left headphone volume: 0 dB */             \
    )                                                                     

/*-- REG4: Input Source ----------------------------------------------------*/

/* LINE IN */

#define AIC23_REG4_LINEIN                                                 \
    AIC23_9BITWORD(         /* REG 4: analog audio path control */        \
        0,                  /* reserved */                                \
        0,0,                /* sidetone attenuation: 6 dB */              \
        0,                  /* sidetone: disabled */                      \
        1,                  /* DAC: selected */                           \
        0,                  /* bypass: off */                             \
        0,                  /* input select for ADC: line */              \
        0,                  /* microphone mute: disabled */               \
        1                   /* microphone boost: enabled */               \
    )                                                                     

/* MIC IN */

#define AIC23_REG4_MIC                                                    \
    AIC23_9BITWORD(         /* REG 4: analog audio path control */        \
        0,                  /* reserved */                                \
        0,0,                /* sidetone attenuation: 6 dB */              \
        0,                  /* sidetone: disabled */                      \
        1,                  /* DAC: selected */                           \
        0,                  /* bypass: off */                             \
        1,                  /* input select for ADC: mic */               \
        0,                  /* microphone mute: disabled */               \
        1                   /* microphone boost: enabled */               \
    )                                                                     

/*-- REG5: Digital Audio Path Control --------------------------------------*/

#define AIC23_REG5                                                \
    AIC23_9BITWORD(         /* REG 5: digital audio path control */       \
        0,0,0,0,0,          /* reserved */                                \
        0,                  /* DAC soft mute: disabled */                 \
        0,0,                /* deemphasis control: 48khz */             \
        0                   /* ADC high-pass filter: disabled */          \
    )                                                                     

/*-- REG6: Power Down Control ----------------------------------------------*/

#define AIC23_REG6                                                \
    AIC23_9BITWORD(         /* REG 6: power down control */               \
        0,                  /* reserved */                                \
        0,                  /* device power: on (i.e. not off) */         \
        0,                  /* clock: on */                               \
        0,                  /* oscillator: on */                          \
        0,                  /* outputs: on */                             \
        0,                  /* DAC: on */                                 \
        0,                  /* ADC: on */                                 \
        0,                  /* microphone: on */                          \
        0                   /* line input: on */                          \
    )                                                                     

/*-- REG7: Digital Audio Interface Control ---------------------------------*/

#define AIC23_REG7                                                \
    AIC23_9BITWORD(         /* REG 7: digital audio interf. format ctrl*/ \
        0,0,                /* reserved */                                \
        1,                  /* master/slave mode: master */               \
        0,                  /* DEC left/right swap: disabled */           \
        0,                  /* no delay before transmitting MSB */        \
        0,0,                /* input bit length: 16 bit */                \
        1,1                 /* data format: DSP format */                 \
    )                                                                     

/*-- REG8: Sample Rate -----------------------------------------------------*/

#define AIC23_REG8_8KHZ                                                   \
    AIC23_9BITWORD(         /* REG 8: sample rate control */              \
        0,                  /* reserved */                                \
        0,                  /* clock output divider: 1 (MCLK) */        \
        0,                  /* clock input  divider: 1 (MCLK) */        \
        0,0,1,1,0,          /* sampling rate: ADC 8kHz DAC 8kHz */        \
        1                   /* clock mode select (USB/normal): USB */     \
    )                                                                     \

#define AIC23_REG8_32KHZ                                                  \
    AIC23_9BITWORD(         /* REG 8: sample rate control */              \
        0,                  /* reserved */                                \
        0,                  /* clock output divider: 1 (MCLK) */        \
        0,                  /* clock input  divider: 1 (MCLK) */        \
        0,1,1,0,0,          /* sampling rate: ADC 32kHz DAC 32kHz */      \
        1                   /* clock mode select (USB/normal): USB */     \
    )                                                                     

#define AIC23_REG8_44_1KHZ                                                \
    AIC23_9BITWORD(         /* REG 8: sample rate control */              \
        0,                  /* reserved */                                \
        0,                  /* clock output divider: 1 (MCLK) */        \
        0,                  /* clock input  divider: 1 (MCLK) */        \
        1,0,0,0,1,          /* sampling rate: ADC 44.1kHz DAC 44.1kHz */  \
        1                   /* clock mode select (USB/normal): USB */     \
    )                                                                     

#define AIC23_REG8_48KHZ                                                  \
    AIC23_9BITWORD(         /* REG 8: sample rate control */              \
        0,                  /* reserved */                                \
        0,                  /* clock output divider: 1 (MCLK) */        \
        0,                  /* clock input  divider: 1 (MCLK) */        \
        0,0,0,0,0,          /* sampling rate: ADC 48kHz DAC 48kHz */      \
        1                   /* clock mode select (USB/normal): USB */     \
    )                                                                     

#define AIC23_REG8_96KHZ                                                  \
    AIC23_9BITWORD(         /* REG 8: sample rate control */              \
        0,                  /* reserved */                                \
        0,                  /* clock output divider: 1 (MCLK) */        \
        0,                  /* clock input  divider: 1 (MCLK) */        \
        0,1,1,1,0,          /* sampling rate: ADC 96kHz DAC 96kHz */      \
        1                   /* clock mode select (USB/normal): USB */     \
    )                                                                     

/*-- REG9: Digital Interface Activation -----------------------------------*/

#define AIC23_REG9                                               \
    AIC23_9BITWORD(         /* REG 9: digital interface activation */     \
        0,0,0,0,0,0,0,0,    /* reserved */                                \
        1                   /* active */                                  \
    )                                                                     

#endif 

