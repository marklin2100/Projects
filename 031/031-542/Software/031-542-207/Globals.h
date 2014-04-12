#ifndef globals_h
#define globals_h
/*---------------------------------------------------------------------------

  Globals module

  This module contains global CTVs for the application.

  Created by David Annett 22 February 2011

  Copyright Lynx Innovation 2011

---------------------------------------------------------------------------*/

// Defines that control includes

#define HWPanelVer  3
#define UseUART 1

// ColourTable can be CTUser, CTBars, CTAlt
#define CTUser  1
#define CTBars  2
#define CTAlt   3
#define ColourTable     CTBars
//#define ColourTable     CTAlt


//#define MuteBetweenTracks 1

// Includes

#include <stdbool.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "CommonTypes.h"


// Constants

#define Channels  6


// Input pins

#define BCD1          PC5
#define BCD2          PC4
#define BCD4          PC3
#define BCD8          PC2
#define BCDInPin      PINC
#define BCDInPort     PORTC

// Output pins

#define LED_CTRL      PB1
#define LED_Port      PORTB
// Data direction registers

#define PortDirB          (1 << LED_CTRL)
#define PortDirC          0
#define PortDirD          0


// Pull ups

#define PortPullUpB       0
#define PortPullUpC       (1 << BCD1 | 1 << BCD2 | 1 << BCD4 | 1 << BCD8)
#define PortPullUpD       0



// Other constants


// Useful macros


#define NOP asm("nop");

// Type definitions



// Vars

#endif
