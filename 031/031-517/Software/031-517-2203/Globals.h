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

#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "CommonTypes.h"


// Constants

#define KeyBoard          1
#define TouchBoard        2
#define BoardType         KeyBoard
// Input pins

#define Switch1In         PC5
#define Switch2In         PC4
#define Switch3In         PC3
#define Switch4In         PC2
#define Switch5In         PC1
#define Switch6In         PC0
#define Switch1to6InPin   PINC
#define Switch1to6InPort  PORTC

#define Switch7In         PB4
#define Switch7InPin      PINB
#define Switch7InPort     PORTB

#define Switch8In         PD3
#define Switch8InPin      PIND
#define Switch8InPort     PORTD

#define MosLED            PB3
#define MosLEDPort        PORTB
// Output pins

#define LED4Line          PB0
#define LED4Port          PORTB
#define LED6Line          PB1
#define LED6Port          PORTB
#define LED7Line          PB5
#define LED7Port          PORTB
#define LED8Line          PB2
#define LED8Port          PORTB

#define LED1Line          PD5
#define LED1Port          PORTD
#define LED2Line          PD6
#define LED2Port          PORTD
#define LED3Line          PD7
#define LED3Port          PORTD
#define LED5Line          PD2
#define LED5Port          PORTD

#define RS485DE           PD4
#define RS485DEPort       PORTD


// Data direction registers)
#define PortDirB          ((1 << LED4Line) | (1 << LED6Line) | (1 << LED7Line) | (1 << MosLED) | (1 << LED8Line))
#define PortDirC          0
#define PortDirD          ((1 << LED1Line) | (1 << LED2Line) | (1 << LED3Line) | (1 << LED5Line) | (1 << RS485DE))

// Pull ups

#define PortPullUpB       (1 << Switch7In)
#define PortPullUpC       ((1 << Switch1In) | (1 << Switch2In) | (1 << Switch3In) | (1 << Switch4In) | (1 << Switch5In) | (1 << Switch6In))
#define PortPullUpD       (1 << Switch8In)


// Other constants


// Useful macros

//#define TestLow           ((LocInPort) &= (~(1<<LocIn)))
//#define TestHigh          ((LocInPort) |= (1<<LocIn))

#define NOP asm("nop");

// Type definitions



// Vars

#endif
