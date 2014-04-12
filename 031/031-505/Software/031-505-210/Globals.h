#ifndef globals_h
#define globals_h
/*---------------------------------------------------------------------------

  Globals module

  This module contains global CTVs for the application.

  Created by David Annett 13 May 2010

  Copyright Lynx Innovation 2010

---------------------------------------------------------------------------*/

// Defines that control includes


// Includes

#include "CommonTypes.h"
#include <stdbool.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>


// Constants

// Input pins

//#define VibSw             PB3
//#define VibSwPin          PINB
//#define VibSwPort         PORTB




// Output pins

#define Lamp              PB3
#define LampPort          PORTB

// Data direction registers

#define PortDirB          (1 << Lamp)

// Other constants


// Useful macros

//#define VibSwActive         (!GetBit(VibSwPin, VibSw))

#define NOP asm("nop");

// Type defintions



// Vars

#endif
