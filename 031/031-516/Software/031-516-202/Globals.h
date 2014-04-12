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

#define VS10XX_FLAC   1
#define I2S           1

#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include "CommonTypes.h"
#include "BoardComms.h"

#define  START_ADDRESS   0XE000
//====================== Constants ============================================

//for two sdcards
//BothPowerCs       1
//EachPowerCs       2
//BothPowerEachCs   2
#define SDPowerCs   1



// Product information (used in EEPROM)

#define PILynxID          0x4C594E58L
#define PICustomer         31         // Don't use 031 as that is treated as octal by GCC
#define PIProject         516
#define PIPartNumber      202
#define PISettingsVersion  17


#define SWVerMajor        1
#define SWVerMinor        1
#define SWVerFix          2
// Build control options

#define HWVer             1
#define HWKeyVer          1
#define RUN_TESTS         1
#define SecondUART        1
//#define DumpComms         1



// 38400 with 24Mhz xtal
#define BaudRate          38

// SPI
#define DD_MOSI           PB5
#define DD_MISO           PB6
#define DD_SCK            PB7
#define DDR_SPI           DDRB

//SD power
#define SD_POWER0         PB2
#define SD_POWER1         PC2
#define SD_POWER_PORT0    PORTB
#define SD_POWER_PORT1    PORTC
#define SD_PowerOnx(x)    SetBit(SD_POWER_PORT##x,SD_POWER##x)
#define SD_PowerOffx(x)   ClearBit(SD_POWER_PORT##x,SD_POWER##x)
// pin to select/deselect card
#define SD_PORT PORTB
#define SD_SS0  PB1
#define SD_SS1  PB0
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

// Output pins

#define XDCS              PA2
#define XDCSPort          PORTA
#define XCS               PA3
#define XCSPort           PORTA

#define RS485DE2          PA6
#define RS485DE           PA7
#define RS485DEPort       PORTA


#define MP3Reset          PB3
#define MP3ResetPort      PORTB

#define Sel0A             PD6
#define Sel1A             PD7
#define SelAPort          PORTD

#define MP3CD             PD4
#define MP3CDPort         PORTD
#define MP3WP             PD5
#define MP3WPPort         PORTD

// Data direction registers

#define PortDirA          ((1 << XDCS) | (1 << XCS) | (1 << RS485DE) | (1 << RS485DE2))
#define PortDirB          ((1 << MP3Reset)  | (1 << SD_POWER0) | (1 << SD_POWER1) | (1 << SD_SS1) | (1 << SD_SS0))
#define PortDirC          0
#define PortDirD          ((1 << MP3CD) | (1 << MP3WP) | (1 << Sel0A) | (1 << Sel1A))

// Pull ups

#define PortPullUpA       0
#define PortPullUpB       0
#define PortPullUpC       0
#define PortPullUpD       0

// Other constants

// Useful macros

#define MP3ResetAssert    ((MP3ResetPort) &= (~(1<<MP3Reset)))
#define MP3ResetNegate    ((MP3ResetPort) |= (1<<MP3Reset))
#define XCSAssert         ((XCSPort) &= (~(1<<XCS)))
#define XCSNegate         ((XCSPort) |= (1<<XCS))
#define XDCSNegate        ((XDCSPort) &= (~(1<<XDCS)))
#define XDCSAssert        ((XDCSPort) |= (1<<XDCS))

#define NOP asm("nop");

// Enumerated constants

enum TRamp {NoRamp, RampDown, RampDefault};
enum TInput {MP3In, LCDIn};
enum TError {NoError, ErrorNoMP3, ErrorNoOutPut};

//====================== Types ================================================

typedef struct {
  Uint8 IdleVolume;
  Uint8 MinimumVolume;
  Uint8 DefaultVolume;
  Uint8 MaximumVolume;
} TProdDef;


// EEPROM usage


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
                                          // vv
  TProdDef    ProdDef[UnknownProduct+1];  //  0 Product specific settings
  Uint8       VolumeStep;                 // 40 Amount to step volume up and down by on key press
  Uint8       VolumeRampStep;             // 41 Amount to step volume up and down by on volume ramping
  Uint8       ForceProduct;               // 42 Product type override.  If UnknownProduct use product reported by bay
  Uint16      IdlePeriod;                 // 43 Time in 10mS units till idle from last key press
  Uint8       IdleInput;                  // 45 Idle input override.  If > 3 use coded logic
  Uint8       DefaultInput;               // 46 Default active input.  If > 3 use coded logic
  Uint8       BassLevel;                  // 47 Bass level in 1dB steps from 0 to 15dB, 0 = off
  Uint8       BassFreq;                   // 48 Bass frequency in 10Hz step from 2 to 15 (20 to 150Hz)
  Int8        TrebleLevel;                // 49 Treble level in 1.5dB steps from -8 to 7 (-12 to +11.5dB), 0 = off
  Uint8       TrebleFreq;                 // 50 Treble frequency in 1kHz steps from 1 to 15kHz
} TSettings;


//====================== Public vars ==========================================

// Settings vars, mirrored in EEPROM

extern       TSettings  Settings;
extern const TSettings  DefaultSettings PROGMEM;

// Comms buffers

extern Uint8      TxBuf[BCMsgSize];
extern Uint8      RxBuf[BCMsgSize];

extern bool       SlaveMode;

//====================== Functions ============================================


/*
  CheckForBoardMsg routine

  This routine will check is new message is present, display it and do default
  processing if it is.
*/
void CheckForBoardMsg(void);


/*
  ProcessBoardMsg routine

  This routine processes an incoming message
*/
void ProcessBoardMsg(void);


/*
  ExchangeBoardMsg routine

  This routine sends a message to a bay and waits for an answer
 */
bool ExchangeBoardMsg(Uint8 Address, enum TBCT BCTSend, Uint8 Param1, Uint8 Param2, enum TBCT BCTExpect);


/*
  SetMux routine

  This routine sets the input muxes for a bay.  No checks are made, use
  SetInput for safe version.
 */
void SetMux(enum TInput Source);

void ShowError(Uint8 ErrorCode, bool Reboot);

#endif
