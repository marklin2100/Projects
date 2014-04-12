#ifndef globals_h
#define globals_h
/*---------------------------------------------------------------------------

  Globals module

  This module contains global CTVs for the application.

  Created by David Annett 18 December 2012

  Copyright Lynx Innovation 2012

---------------------------------------------------------------------------*/

//====================== Includes =============================================

// Defines that control includes

//#define Headphone4PosDefault  1

#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include "CommonTypes.h"


//====================== Constants ============================================
// Build control options

#define SecondUART        1
//#define DumpComms         1

#define UseUART           1



// Ports for I2C

#define I2C_DDR     DDRC
#define I2C_PIN     PINC
#define I2C_PORT    PORTC

// Pins to be used in the bit banging

#define I2C_CLK 0
#define I2C_DAT 1


// Output pins

#define USBLED_516  PC3
#define USBLEDPort  PORTC

#define USBLED_517  PC6


// Data direction registers

#define PortDirA          0
#define PortDirB          0
#define PortDirC          ((1 << USBLED_516) | (1 << USBLED_517))
#define PortDirD          0

// Pull ups

#define PortPullUpA       0
#define PortPullUpB       0
#define PortPullUpC       0
#define PortPullUpD       0

// Other constants

#define KBSettings        8

#define Debug             0

// Useful macros

typedef struct {
  // Standard header
  Uint32      LynxID;                   // Should be 0x4C594E58L
  Uint16      Customer;
  Uint16      Project;
  Uint16      PartNumber;
  Uint8       Version;
  Uint8       Spare1;
  Uint8       Spare2;
  Uint8       Spare3;
  Uint8       Spare4;
  Uint8       Checksum;

  // Board specific                          Setting number
} TSettings;

enum TBootStatus{NoAT24C,AT24Err,AT24NoData,AT24DataErr,AT24VerLow,AT24VerSame,AT24VerHigth,NoStatus};

#define NOP asm("nop");


#endif
