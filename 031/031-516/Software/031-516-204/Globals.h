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
#include <stddef.h>
#include "CommonTypes.h"
#include "BoardComms.h"

#define HWVer 2

#if !defined (__AVR_ATmega164P__)
#error "CPU type must be ATmega164P"
#endif

// Build control

#define SecondUART        1
#define DebugVerbosity    1
//#define Analog            1
  //FiveHeadphones,     // 0 Five headphones
  //Pill,               // 1 Pill speaker
  //Beatbox,            // 2 Beat Box speaker
  //Defiant device  205/207 = headphones; 204/208 = pill; 210 = beat box

  //2:out put hex file select like 031-517-204/205/207/208/210
#define ProductID        1
#define Position         0

// Input pins

#define ID0In             PA0
#define ID1In             PA1
#define ID2In             PA2
#define ID3In             PA3
#define IDInPin           PINA
#define IDInPort          PORTA


// IIC Port
#define IIC_SCL1          PC0
#define IIC_SDA1          PC1
#define IIC_Pin1          PINC
#define IIC_DDR1          DDRC
#define IIC_Port1         PORTC

#define IIC_SCL2          PC2
#define IIC_SDA2          PC3
#define IIC_Pin2          PINC
#define IIC_DDR2          DDRC
#define IIC_Port2         PORTC

#define IIC_SCL3          PC4
#define IIC_SDA3          PC5
#define IIC_Pin3          PINC
#define IIC_DDR3          DDRC
#define IIC_Port3         PORTC

#define IIC_SCL4          PB0
#define IIC_SDA4          PB1
#define IIC_Pin4          PINB
#define IIC_DDR4          DDRB
#define IIC_Port4         PORTB

#define IIC_SCL5          PC6
#define IIC_SDA5          PC7
#define IIC_Pin5          PINC
#define IIC_DDR5          DDRC
#define IIC_Port5         PORTC



// Output pins
#define HC245DIR          PD5
#define HC245OE           PD6
#define HC245Port         PORTD

#define RS485DE           PA4
#define RS485DEPort       PORTA

//led
#define LED_GRAPICAL      PB1
#define LED1_Pin          PB3
#define LED2_Pin          PB2
#define LED_GRAPICAL_DDR  DDRB
#define LED_GRAPICAL_PORT PORTB
#define LED1_DDR          DDRB
#define LED2_DDR          DDRB
#define LED1_PORT         PORTB
#define LED2_PORT         PORTB
// Data direction registers
#define PortDirA          ((1 << RS485DE))
#define PortDirB          ((1 << LED1_Pin) | (1 << LED2_Pin) | (1 << LED_GRAPICAL))
#define PortDirC          0
#define PortDirD          ((1 << HC245DIR) | (1 << HC245OE))

// Pull ups
#define PortPullUpA       ((1 << ID0In) | (1 << ID1In) | (1 << ID2In) | (1 << ID3In))
#define PortPullUpB       0
#define PortPullUpC       0
#define PortPullUpD       0

// Other constants


// Useful macros


#define NOP asm("nop");

#endif

