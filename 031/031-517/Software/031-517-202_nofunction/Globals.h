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

#define VS10XX_FLAC           1
#define I2S                   1

#define Pos4Default           1
#define Pos5Default           2
#define Pos3Default           3

//3pos for 031-520 3headphone and pill, 4pos for 4headphone and pill ,5pos for 5headphone
#define HeadphonePosDefault   Pos5Default

#define TGT8                  1
#define TGT4                  2
#define BBY                   3
#define Device520             4
#define Others                5

#define HeadphoneDefault      Others

#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

//====================== Constants ============================================

// Product information (used in EEPROM)

#define PILynxID          0x4C594E58L
#define PICustomer         31         // Don't use 031 as that is treated as octal by GCC
#define PIProject         517
#define PIPartNumber      202
#define PISettingsVersion  11

// Build control options

#define SWVerMajor        1
#define SWVerMinor        1
#define SWVerFix          2

#define HWVer             5
#define HWKeyVer          1
#define RUN_TESTS         1
#define SecondUART        1
//#define DumpComms         1
//  PWM modes, slow is in software, fast is hardware assisted on specific port

#define NoPWM             1
#define SlowPWM           2
#define FastPWM           3
#define PWMMode           NoPWM

// 38400 with 24Mhz xtal
#define BaudRate          38

// SPI
#define DDR_SPI           DDRB
#define DD_SS             PB4
#define DD_MOSI           PB5
#define DD_MISO           PB6
#define DD_SCK            PB7
#define MUSTBeOutput      PB4

// SD
// pin to select/deselect card
#define SD_PORT PORTB
#define SD_SS PB0
// pin to detect card in the socket
#define SD_CD_PIN PIND
#define SD_CD PD4

// Ports for I2C

#define I2C_DDR     DDRC
#define I2C_PIN     PINC
#define I2C_PORT    PORTC

// Pins to be used in the bit banging

#define I2C_CLK 0
#define I2C_DAT 1

// Input pins

#define DREQ              PA1
#define DREQPin           PINA
#define DREQPort          PORTA

#define DIP1In            PC7
#define DIP1Pin           PINC
#define DIP1Port          PORTC

#define DIP2In            PA0
#define DIP2Pin           PINA
#define DIP2Port          PORTA

#define DIP3In            PA4
#define DIP3Pin           PINA
#define DIP3Port          PORTA

#define DIP4In            PA5
#define DIP4Pin           PINA
#define DIP4Port          PORTA

// Output pins

#define XDCS              PA2
#define XDCSPort          PORTA
#define XCS               PA3
#define XCSPort           PORTA

#define RS485DE           PA7
#define RS485DEPort       PORTA

#define CS                PB0
#define CSPort            PORTB
#define MP3Reset          PB3
#define MP3ResetPort      PORTB

#if HWVer >= 5
#define SD_PWR            PB2
#define SD_PWR_PORT       PORTB
#else
#define SD_PWR            PB4
#define SD_PWR_PORT       PORTB
#endif

#define Sel0A             PD6
#define Sel1A             PD7
#define SelAPort          PORTD

#define Sel0B             PC2
#define Sel1B             PC3
#define SelBPort          PORTC

#define Sel0C             PC4
#define Sel1C             PC5
#define SelCPort          PORTC

#define Mute              PC6
#define MutePort          PORTC

#define MP3CD             PD4
#define MP3CDPort         PORTD
#define MP3WP             PD5
#define MP3WPPort         PORTD

// Data direction registers

#define PortDirA          ((1 << XDCS) | (1 << XCS) | (1 << RS485DE))
#define PortDirB          ((1 << MP3Reset) | (1 << CS) | (1 << DD_SS) | (1 << SD_PWR) | (1 << MUSTBeOutput))
#define PortDirC          ((1 << Mute) | (1 << Sel0B) | (1 << Sel1B) | (1 << Sel0C) | (1 << Sel1C))
#define PortDirD          ((1 << MP3CD) | (1 << MP3WP) | (1 << Sel0A) | (1 << Sel1A))

// Pull ups

#define PortPullUpA       ((1 << DIP2In) | (1 << DIP3In) | (1 << DIP4In))
#define PortPullUpB       (1 << DD_MISO)
#define PortPullUpC       (1 << DIP1In)
#define PortPullUpD       0

// Other constants

#define KBSettings        8

// Useful macros

#define MP3ResetAssert    ((MP3ResetPort) &= (~(1<<MP3Reset)))
#define MP3ResetNegate    ((MP3ResetPort) |= (1<<MP3Reset))
#define XCSAssert         ((XCSPort) &= (~(1<<XCS)))
#define XCSNegate         ((XCSPort) |= (1<<XCS))
#define XDCSNegate        ((XDCSPort) &= (~(1<<XDCS)))
#define XDCSAssert        ((XDCSPort) |= (1<<XDCS))

#define SD_PowerOn()      SetBit(SD_PWR_PORT,SD_PWR)
#define SD_PowerOff()     ClearBit(SD_PWR_PORT,SD_PWR)

#define NOP asm("nop");

#define HeadPhoneChMax    5

#endif
