#ifndef globals_h
#define globals_h
/*---------------------------------------------------------------------------

  Globals module

  This module contains global CTVs for the application.

  Created by David Annett 22 February 2011

  Copyright Lynx Innovation 2011

---------------------------------------------------------------------------*/

// Defines that control includes

#define VS10XX_FLAC  1

// Includes

#include <stdbool.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "CommonTypes.h"


// Constants

#define RUN_TESTS         1

// SPI
#define DDR_SPI           DDRB
#define DD_SS             PB4
#define DD_MOSI           PB5
#define DD_MISO           PB6
#define DD_SCK            PB7

// SD
// pin to select/deselect card
#define SD_PORT PORTB
#define SD_SS PB0
// pin to detect card in the socket
#define SD_CD_PIN PIND
#define SD_CD PD4

// Input pins

#define DREQ              PA1
#define DREQPin           PINA
#define DREQPort          PORTA

#define Touch0In          PC0
#define Touch1In          PC1
#define Touch2In          PC2
#define Touch3In          PC3
#define TouchInPin        PINC
#define TouchInPort       PORTC

#define SWIn              PC7
#define SWInPin           PINC
#define SWInPort          PORTC

// Output pins

#define XDCS              PA2
#define XDCSPort          PORTA
#define XCS               PA3
#define XCSPort           PORTA
#define LampSR_Data       PA4
#define LampST_CP         PA5
#define LampSH_CP         PA6
#define LampPort          PORTA

#define CS                PB0
#define CSPort            PORTB
#define MP3Reset          PB3
#define MP3ResetPort      PORTB
#define IRDrive           PB4
#define IRDrivePort       PORTB

#define Out1              PC4
#define Out2              PC5
#define Out3              PC6
#define OutPort           PORTC

#define MP3CD             PD4
#define MP3CDPort         PORTD
#define MP3WP             PD5
#define MP3WPPort         PORTD

// Data direction registers

#define PortDirA          ((1 << XDCS) | (1 << XCS) | (1 << LampSR_Data) | (1 << LampSH_CP) | (1 << LampST_CP))
#define PortDirB          ((1 << IRDrive) | (1 << MP3Reset) | (1 << CS))
#define PortDirC          ((1 << Out1) | (1 << Out2) | (1 << Out3))
#define PortDirD          ((1 << MP3CD) | (1 << MP3WP))

// Other constants


// Useful macros

#define HP1Off            ((OutPort)   &= (~(1<<Out1)))
#define HP1On             ((OutPort)   |= (1<<Out1))
#define HP2Off            ((OutPort)   &= (~(1<<Out2)))
#define HP2On             ((OutPort)   |= (1<<Out2))
#define SpkOff            ((OutPort)   &= (~(1<<Out3)))
#define SpkOn             ((OutPort)   |= (1<<Out3))
#define MP3ResetAssert    ((MP3ResetPort) &= (~(1<<MP3Reset)))
#define MP3ResetNegate    ((MP3ResetPort) |= (1<<MP3Reset))
#define XCSAssert         ((XCSPort) &= (~(1<<XCS)))
#define XCSNegate         ((XCSPort) |= (1<<XCS))
#define XDCSNegate        ((XDCSPort) &= (~(1<<XDCS)))
#define XDCSAssert        ((XDCSPort) |= (1<<XDCS))

#define NOP asm("nop");

// Type definitions



// Vars

#endif
