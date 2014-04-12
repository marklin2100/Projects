#ifndef globals_h
#define globals_h
/*---------------------------------------------------------------------------

  Globals module

  This module contains global CTVs for the application.

  Created by David Annett 22 February 2011

  Copyright Lynx Innovation 2011

---------------------------------------------------------------------------*/

// Defines that control includes

//#define DebugLamps    1

// Includes

#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "CommonTypes.h"


// Constants
#define HWVer             2
// Input pins

#define Switch1In         PC5
#define Switch2In         PC4
#define Switch3In         PC3
#define Switch4In         PC2
#define Switch5In         PC1
#define Switch6In         PC0
#define Switch1to6InPin   PINC
#define Switch1to6InPort  PORTC

#define Switch7In         PD3
#define Switch7InPin      PIND
#define Switch7InPort     PORTD

// Output pins

#define LED4              PB0
#define LED6              PB1
#define LED7              PB2
#define LED4to7Port       PORTB

#define LED1              PD5
#define LED2              PD6
#define LED3              PD7
#define LED1to3Port       PORTD

#define LEDLogo           PB1
#define LEDLogoPort       PORTB

#define RS485DE           PD4
#define RS485DEPort       PORTD


// Data direction registers

#define PortDirB          ((1 << LED4) | (1 << LED6) | (1 << LED7))
#define PortDirC          0
#define PortDirD          ((1 << LED1) | (1 << LED2) | (1 << LED3) | (1 << LEDLogo) | (1 << RS485DE))

// Pull ups

#define PortPullUpB       0
#define PortPullUpC       ((1 << Switch1In) | (1 << Switch2In) | (1 << Switch3In) | (1 << Switch4In) | (1 << Switch5In) | (1 << Switch6In))
#define PortPullUpD       (1 << Switch7In)


// Other constants


// Useful macros

//#define TestLow           ((LocInPort) &= (~(1<<LocIn)))
//#define TestHigh          ((LocInPort) |= (1<<LocIn))

#define NOP asm("nop");

// Type definitions



// Vars

#endif
