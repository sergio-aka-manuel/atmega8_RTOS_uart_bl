/*
 * globals.h
 *
 *	Тут описываются все инклюды, общие для всего проекта. 
 *	Чтобы их каждый раз не определять в каждом файле.
 *
 */

#pragma once

#ifndef __AVR_ATmega8__//__AVR_ATmega168__//__AVR_ATmega8U2__//__AVR_ATmega328P__
#define __AVR_ATmega8__
#endif//__AVR_ATmega8__

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include <util/delay.h>
#include <util/atomic.h>

#include <stdio.h>
#include <stddef.h>         /* for 'NULL' */
#include <stdint.h>         /* for 'int8_t' to 'uint64_t' */
#include <stdbool.h>        /* for 'true', 'false' */
#include <string.h>

#include "avrlibtypes.h"
#include "avrlibdefs.h"


#define HI(x) 			((x)>>8)
#define LO(x) 			((x)&0xFF)

#define CONFIGURE_AS_INPUT(x)   _XSI(x)
#define CONFIGURE_AS_OUTPUT(x)  _XSO(x)
#define GET(x)                  _XG(x)
#define SET(x)                  _XS(x)
#define RESET(x)                _XR(x)
#define TOGGLE(x)               _XT(x)

#define _XSI(p, b)           	DDR ##  p &= ~(1<<b); _XR(p,b)
#define _XSO(p, b)           	DDR ##  p |=  (1<<b); _XR(p,b)
#define _XG(p, b)            	((PIN ##  p>>b) & 0x01)
#define _XS(p, b)            	PORT ## p |=  (1<<b)
#define _XR(p, b)            	PORT ## p &= ~(1<<b)
#define _XT(p, b)            	PORT ## p ^=  (1<<b)

#define GET_BIT(d,b)		((d>>b) & 0x01)
#define SET_BIT(d,b)		(d |= (1<<b))
#define RESET_BIT(d,b)		(d &= ~(1<<b))


#define         ARDUINO_LED_D13                         B,5
//#define	GPIO_LED1				B,0
//#define	GPIO_LED2				B,1
//#define	GPIO_BTN1				D,4
//#define	GPIO_BTN2				D,5
//#define	GPIO_BTN3				D,6

//#define DEBUG
//#define DBG_STR_LEN				64

//#ifdef  DEBUG
//	void debugPring( const char *str, uint32_t val );
//	void debugPrintCanMessage( canMessage_t *msg );
//	#define DBG(str, val)  debugPring(str, val)
//#else
//	#define DBG(str, val)
//#endif //DEBUG
