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
#include <stddef.h>
#define HWVer 1

#if HWVer == 1
#if !defined (__AVR_ATmega48__)
#error "CPU type must be ATmega48"
#endif


// Constants

// Input pins


#define RS485DE           PB2
#define RS485DEPort       PORTB

#define Test1             PB3
#define Test2             PB4
#define Test3             PB5
#define TestPort          PORTB

#define IRDrive           PD3
#define IRDrivePort       PORTD

// Data direction registers

#define PortDirB          ((1 << RS485DE) | (1 << Test1) | (1 << Test2) | (1 << Test3))
#define PortDirC          0
// TODO: LocIn made output test line, REMOVE later
//#define PortDirC          ((1 << MuteLine) | (1<<LocIn))
#define PortDirD          ((1 << IRDrive))

// Pull ups

#define PortPullUpB       0
#define PortPullUpC       0
#define PortPullUpD       0


// Other constants


// Useful macros

#define Mute              ((MuteLinePort) &= (~(1<<MuteLine)))
#define UnMute            ((MuteLinePort) |= (1<<MuteLine))

#define TestLow           ((LocInPort) &= (~(1<<LocIn)))
#define TestHigh          ((LocInPort) |= (1<<LocIn))



// Type definitions



// Vars

#endif

#define NOP asm("nop");

#endif

