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

#define HeadphoneDefault      BBY

#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include "CommonTypes.h"
#include "BoardComms.h"
#include <avr/wdt.h>

//====================== Constants ============================================

// Product information (used in EEPROM)

#define PILynxID          0x4C594E58L
#define PICustomer         31         // Don't use 031 as that is treated as octal by GCC
#define PIProject         517
#define PIPartNumber      202
#define PISettingsVersion  10

// Build control options

#define SWVerMajor        1
#define SWVerMinor        1
#define SWVerFix          1

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

// Keys and lamps

#define VolUp           1
#define PrevTrack       2
#define VolDown         3
#define NextTrack       4
#define PlayPause       5
#define LeftSelect      6
#define RightSelect     7
#define CenterSelect    8
#define MaxKey          8

#define DirectionLEDS 0b11111
#define LeftLEDs  (1 << (LeftSelect - 1))
#define RightLEDs (1 << (RightSelect - 1))

// Enumerated constants

// TODO: Redefine operational modes based on new spec when created.
enum TConfig {TwoBaysOneSource, OneBayTwoSources, TwoBaysTwoSources,OneBayOneSource};
enum TRamp {NoRamp, RampDown, RampDefault};
enum TInput {MP3In, LCDIn, LeftTablet, RightTablet};
enum TError {NoError, ErrorNoMP3, ErrorNoSource, ErrorNoBays};

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
  Uint8       LeftProduct;                // 42 Product type override for left bay.  If UnknownProduct use product reported by bay
  Uint8       CenterProduct;              // 43 Product type override for center bay.  If UnknownProduct use product reported by bay
  Uint8       RightProduct;               // 44 Product type override for right bay.  If UnknownProduct use product reported by bay
  Uint16      IdlePeriod;                 // 45 Time in 100mS units till idle from last key press
  Uint8       LeftIdleInput;              // 47 Idle input override for left bay.  If > 3 use coded logic
  Uint8       CenterIdleInput;            // 48 Idle input override for center bay.  If > 3 use coded logic
  Uint8       RightIdleInput;             // 49 Idle input override for right bay.  If > 3 use coded logic
  Uint8       DefaultBay;                 // 50 Default bay.  If > NoBay use coded logic
  Uint8       DefaultInput;               // 51 Default active input.  If > 3 use coded logic
  Uint8       LeftSelectKeyOveride;       // 52 Bit 7 activates override.  Bit 6 sets BayNotSourceButtons. Bits 0 to 3 remap key.
  Uint8       CenterSelectKeyOveride;     // 53 Bit 7 activates override.  Bit 6 sets BayNotSourceButtons. Bits 0 to 3 remap key.
  Uint8       RightSelectKeyOveride;      // 54 Bit 7 activates override.  Bit 6 sets BayNotSourceButtons. Bits 0 to 3 remap key.
  Uint8       BassLevel;                  // 55 Bass level in 1dB steps from 0 to 15dB, 0 = off
  Uint8       BassFreq;                   // 56 Bass frequency in 10Hz step from 2 to 15 (20 to 150Hz)
  Int8        TrebleLevel;                // 57 Treble level in 1.5dB steps from -8 to 7 (-12 to +11.5dB), 0 = off
  Uint8       TrebleFreq;                 // 58 Treble frequency in 1kHz steps from 1 to 15kHz
  bool        PauseKeyEnabled;            // 59 True Play/pause key functional, false if is ignored
  Uint8       LampBrightness[KBSettings]; // 60 See BCTLampBrightness, 255 is 'not used' value
  bool        InputChangeTrack1;          // 68 True if input change selection should restart playback on track 1
  Uint8       HeadPhoneChGain[HeadPhoneChMax]; // 69 Gain adjustment for each headphone
  Uint8       HeadphoneChMax[HeadPhoneChMax]; // 74 Maximum volume for each headphone
} TSettings;


//====================== Public vars ==========================================

// Settings vars, mirrored in EEPROM

extern       TSettings  Settings;
extern const TSettings  DefaultSettings PROGMEM;

/*
extern TProdDef   ProdDef[UnknownProduct+1];
#define           SettingIdlePeriod sizeof(ProdDef)
extern Uint16     IdlePeriod;
#define           SettingLeftProduct (SettingIdlePeriod + sizeof(IdlePeriod))
extern Uint8      LeftProduct;
#define           SettingRightProduct (SettingLeftProduct + sizeof(LeftProduct))
extern Uint8      RightProduct;

#define           MaxSetting  (SettingRightProduct + sizeof(RightProduct))
*/

// Comms buffers

extern Uint8      TxBuf[BCMsgSize];
extern Uint8      RxBuf[BCMsgSize];

extern bool       SlaveMode;

extern Uint8      LastError;

//====================== Functions ============================================


/*
  CheckForBoardMsg routine

  This routine will check if a new message is present, display it and do default
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
void SetMux(enum TInput Source, enum TBay SIBay);


/*
  ShowError routine

  Display and error number pattern on the lamps and reboot if second parameter true
*/
void ShowError(Uint8 ErrorCode, bool Reboot);

#endif
