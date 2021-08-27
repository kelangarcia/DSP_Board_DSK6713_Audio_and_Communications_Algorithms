/*============================================================================
 * dsk_registers.h
 *      Register definitions for the boards we have in class.
 *==========================================================================*/

#ifndef _dsk_registers_h_
#define _dsk_registers_h_

#ifdef TARGET_DSK6713

/* Register in CPLD on board for LEDs and DIP switches */
#define USER_REG (*((unsigned int *) 0x90080000))
#define USER_REG_LED_MASK       0x0000ffff

#define USER_REG_LED_INC                \
        USER_REG = (USER_REG & ~USER_REG_LED_MASK) | (((USER_REG & USER_REG_LED_MASK) + 1) & 0xf)

#endif

#endif






