#ifndef globals_h
#define globals_h
/*---------------------------------------------------------------------------

  Globals module

  This module contains global CTVs for the application.

  Created by David Annett 22 February 2011

  Copyright Lynx Innovation 2011

---------------------------------------------------------------------------*/

// Defines that control includes

// Includes

#include <stdbool.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "CommonTypes.h"


// Constants

// Input pins

//#define LocIn             PC4
//#define LocInPin          PINC
//#define LocInPort         PORTC

// Output pins

#define LED1Line          PC5
#define LED2Line          PC4
#define LED3Line          PC3
#define LED4Line          PC2
#define LED5Line          PC1
#define LED6Line          PC0
#define LEDPort           PORTC

#define RS485DE           PD4
#define RS485DEPort       PORTD


// Data direction registers

#define PortDirB          0
#define PortDirC          ((1 << LED1Line) | (1 << LED2Line) | (1 << LED3Line) | (1 << LED4Line) | (1 << LED5Line) | (1 << LED6Line))
#define PortDirD          ((1 << RS485DE))

// Pull ups

#define PortPullUpB       0
#define PortPullUpC       0
#define PortPullUpD       0


// Other constants


// Useful macros

//#define Mute              ((MuteLinePort) &= (~(1<<MuteLine)))
//#define UnMute            ((MuteLinePort) |= (1<<MuteLine))

#define NOP asm("nop");

// Type definitions



// Vars

#endif
