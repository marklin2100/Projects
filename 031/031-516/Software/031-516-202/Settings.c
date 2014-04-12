/*****************************************************************************

    Settings module

    This module implements settings stored in the AVR's EEPROM and updated
    from an I2C connected EEPROM or serial port command.
    See http://wiki.lynxinnovation.com/lynxwiki/pmwiki.php?n=Main.AVREEPROM

    David Annett, david@lynxinnovation.com, 18 February 2013

 ****************************************************************************/

//====================== Includes =============================================

#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "CommonTypes.h"
#include "Globals.h"
#include "I2C.h"
#include "UART.h"
#include "Settings.h"

//====================== Constants ============================================

#define EELocID         0x0000
#define EELocCustomer   0x0004
#define EELocProject    0x0006
#define EELocPartNumber 0x0008
#define EELocVersion    0x000A
#define EELocData       0x0010



//====================== Types ================================================

enum TSerState {SerCmd, Set1, Set2, Set3, Val1, Val2, Val3};

//====================== Public vars ==========================================


//====================== Private vars =========================================

Uint8   Setting;
Uint8   Value;
Uint8   SerState = SerCmd;

//====================== Private functions ====================================

/*
  ReadSetting routine

  This routine reads a setting
 */
Uint8 ReadSetting(Uint8 Number)
{
  Uint8 *Ptr;


  Ptr = (Uint8 *)&Settings;
  if (Number < sizeof(Settings))
    return Ptr[Number + EELocData];

  return 0;
}



/*
  WriteSetting routine

  This routine writes a setting.  It will save to the EEPROM.
 */
void WriteSetting(Uint8 Number, Uint8 Value)
{
  Uint8    *Ptr;


  // Check in valid range

  Ptr = (Uint8 *)&Settings;
  if (Number >= sizeof(Settings))
    return;

  Ptr[Number + EELocData] = Value;
  SaveEEPROMSetting();    // Save it for the future
}



/*
  PrintSettings routine

  This routine prints a settings.
 */
void PrintSetting(Uint8 Number)
{
  UART_TxChar('[');
  UART_TxNum(Number, 3);
  UART_TxStr("]=");
  UART_TxNum(ReadSetting(Number), 3);
  UART_TxNewLine();
}



/*
  SetSettingsChecksum routine

  This routine sets the correct checksum value for the current Settings contents.
 */
void SetSettingsChecksum(void)
{
  Uint8 Pos;
  Uint8 ChkSum;
  Uint8  *Ptr;


  Ptr = (Uint8 *)&Settings;
  Settings.Checksum = 0;
  ChkSum = 0;
  for (Pos = 0; Pos < sizeof(Settings); Pos++)
    ChkSum += Ptr[Pos];
  Settings.Checksum = ~ChkSum + 1;
}



/*
  LoadExternalEEPROMBlock routine

  This routine attempts to read a config settings block from an external EEPROM.
  If primary is true it tries to read the primary block, otherwise it tries
  the backup block.
  It will return true if the Settings was updated.
 */
bool LoadExternalEEPROMBlock(bool Primary) {
  Uint8       Pos;
  Uint8       ChkSum;
  Uint8       Offset;
  Uint8       Header[0x10];
  TSettings  *HeaderPtr;


  UART_TxStr("Load from external EEPROM ");

  // Determine block

  if (Primary) {
    Offset = 0x00;
    UART_TxStr("primary");
  } else {
    Offset = 0x80;
    UART_TxStr("backup");
  }
  UART_TxStr(" block, ");

  // Check it's check sum

  ChkSum = 0;
  for (Pos = 0; Pos <= 0x7F; Pos++) {
    ChkSum += I2C_EESReadByte(0xA0, Pos + Offset);
    if (I2C_Status) {
      UART_TxStr("not found\r\n");
      return false;
    }
  }

  if (ChkSum) {
    UART_TxStr("bad checksum = ");
    UART_TxNum(ChkSum, 1);
    UART_TxNewLine();
    return false;
  }

  I2C_EESReadBlock(0xA0, Offset, Header, 0x10);
  HeaderPtr = (TSettings *) &Header;

  if ((HeaderPtr->LynxID     == PILynxID) &&      // Check the board type
      (HeaderPtr->Customer   == PICustomer) &&
      (HeaderPtr->Project    == PIProject) &&
      (HeaderPtr->PartNumber == PIPartNumber)) {
    I2C_EESReadBlock(0xA0, Offset, (Uint8 *)&Settings, sizeof(Settings));
    UART_TxStr("ok\r\n");
    return true;
  }

  // Wasn't correct board

  UART_TxStr("bad data\r\n");
  return false;
}


//====================== Public functions ============================================

/*
  SaveEEPROMSetting

  This routine saves the settings to the EEPROM
 */
void SaveEEPROMSetting(void)
{
  UART_TxStr("Saving settings\r\n");
  SetSettingsChecksum();    // Calculate the checksum

  // Save the primary block

  eeprom_update_block((const void *) &Settings, (void *) 0x00, 0x80);

  // Save the backup block

  eeprom_update_block((const void *) &Settings, (void *) 0x80, 0x80);
}



/*
  LoadEEPROMSetting routine

  This routine loads settings from the EEPROM.  It will first check for
  a valid eternal EEPROM and if that fails will try the internal EEPROM.
  If that also fails it will load defaults from flash memory.  When either
  external EEPROM or flash is used the settings will be saved to the internal
  EEPROM for future use.
 */
void LoadEEPROMSetting(void)
{
  uint8_t   Pos;
  Uint8     ChkSum;


  UART_TxStr("Loading settings\r\n");
  Setting = 0;
  // Try and read an external E2 device if plugged in

  if (LoadExternalEEPROM()) {
    SaveEEPROMSetting();            // We have new settings so save them in NVM
    UART_TxStr("Settings loaded from external EEPROM\r\n");
    return;                         // and use them
  }

  // Check primary block

  ChkSum = 0;
  for (Pos = 0; Pos < sizeof(Settings); Pos++)  // Check the checksum of the block
    ChkSum += eeprom_read_byte((const uint8_t *)(Pos + 0x00));
  if (ChkSum) {
    UART_TxStr("Primary checksum bad\r\n");

  } else if ((eeprom_read_dword((uint32_t *) EELocID) == PILynxID) &&      // Check the board type
             (eeprom_read_word((uint16_t *) EELocCustomer)   == PICustomer) &&
             (eeprom_read_word((uint16_t *) EELocProject)    == PIProject) &&
             (eeprom_read_word((uint16_t *) EELocPartNumber) == PIPartNumber)&&
             (eeprom_read_word((uint16_t *) EELocVersion) == PISettingsVersion)) {
      eeprom_read_block((void *) &Settings, (const void *) 0x00, sizeof(Settings)); // All ok so load and use
      UART_TxStr("Loaded internal primary settings\r\n");
      return;

    } else {
      UART_TxStr("Primary block for wrong board type\r\n");
    }

    // Check backup block

    ChkSum = 0;
    for (Pos = 0; Pos < sizeof(Settings); Pos++)  // Check the checksum of the block
      ChkSum += eeprom_read_byte((const uint8_t *)(Pos + 0x80));
    if (ChkSum) {
      UART_TxStr("Backup checksum bad\r\n");

    } else if ((eeprom_read_dword((uint32_t *) EELocID + 0x80) == PILynxID) &&      // Check the board type
        (eeprom_read_word((uint16_t *) EELocCustomer + 0x80)   == PICustomer) &&
        (eeprom_read_word((uint16_t *) EELocProject + 0x80)    == PIProject) &&
        (eeprom_read_word((uint16_t *) EELocPartNumber + 0x80) == PIPartNumber)&&
        (eeprom_read_word((uint16_t *) EELocVersion + 0x80) == PISettingsVersion)) {
      eeprom_read_block((void *) &Settings, (const void *)  0x80, sizeof(Settings)); // All ok so load and use
      UART_TxStr("Loaded internal backup settings\r\n");
      return;

    } else {
      UART_TxStr("Backup block for wrong board type\r\n");
    }

    // Both blocks bad, load defaults and save them

    UART_TxStr("Loaded default settings\r\n");

    memcpy_PF(&Settings,&DefaultSettings, sizeof(Settings));
    SaveEEPROMSetting();
}



/*
  SaveExternalEEPROM routine

  This routine attempts to save config settings to an external EEPROM.
  It will return true if the EEPROM was updated.
 */
bool SaveExternalEEPROM(void)
{
  Uint8 Pos;


  UART_TxStr("Saving settings to external EEPROM\r\n");

  SetSettingsChecksum();    // Calculate the checksum

  // Fill unused space with zeros

  for (Pos = sizeof(Settings); Pos < 0xff; Pos++)
    I2C_EESWriteByte(0xA0, Pos, 0);

  // Save both copies

  I2C_EESWriteBlock(0XA0, 0x00, (Uint8 *)&Settings, sizeof(Settings));
  I2C_EESWriteBlock(0XA0, 0x80, (Uint8 *)&Settings, sizeof(Settings));
  return true;
}



/*
  LoadExternalEEPROM routine

  This routine attempts to read config settings from an external EEPROM.
  It will return true if the Settings was updated.
 */
bool LoadExternalEEPROM(void) {
  if (LoadExternalEEPROMBlock(true))      // Try the primary block
    return true;                          // Ok so exit with it

  return LoadExternalEEPROMBlock(false);  // Try the back up block;
}


/*
  PrintSettings routine

  This routine prints the current values of all settings.
 */
void PrintSettings(void)
{
  Uint8 Pos;


  UART_TxNum(Settings.Customer, 3);
  UART_TxChar('-');
  UART_TxNum(Settings.Project, 3);
  UART_TxChar('-');
  UART_TxNum(Settings.PartNumber, 3);
  UART_TxChar('_');
  UART_TxNum(Settings.Version, 1);
  UART_TxNewLine();
  for (Pos = 0; Pos < sizeof(Settings) - EELocData; Pos++)
    PrintSetting(Pos);
}



/*
  SettingsControl routine

  This routine allows changing the settings via the serial port.  The received
  character should be passed in.  If it is understood it will return true.
 */
bool SettingsControl(Uint8 Command)
{
  if (SerState != SerCmd) // Echo values being entered
    UART_TxChar(Command);

  switch (SerState) {
    case SerCmd:
      // Process the command

      switch (Command) {
        case 'n':  // Move to next setting
          if (++Setting >= sizeof(Settings) - EELocData)
            Setting = 0;
          break;

        case 'p':   // Move to previous setting
          if (!Setting--)
            Setting = sizeof(Settings) - (EELocData + 1);
          break;

        case '+':   // Increment setting value
          WriteSetting(Setting, ReadSetting(Setting) + 1);
          break;

        case '-':   // Decerement setting value
          WriteSetting(Setting, ReadSetting(Setting) - 1);
          break;

        case 'z':   // Zero setting value
          WriteSetting(Setting, 0);
          break;

        case 's':   // Enter setting number
          UART_TxStr("Enter setting number\r\n");
          SerState = Set1;
          break;

        case 'v':   // Enter setting value
          UART_TxStr("Enter setting value\r\n");
          SerState = Val1;
          break;

        case 'd':   // Dump the settings
          PrintSettings();
          break;

        case 'w':   // Write to external EEPROM
          SaveExternalEEPROM();
          break;

        default:
          return false;
          break;
      }

      // Show the result of command

      PrintSetting(Setting);
      break;

        case Set1:
          Setting = Command - '0';
          SerState = Set2;
          break;

        case Set2:
          Setting *= 10;
          Setting += Command - '0';
          SerState = Set3;
          break;

        case Set3:
          Setting *= 10;
          Setting += Command - '0';
          if (Setting >= sizeof(Settings))
            Setting = 0;
          UART_TxNewLine();
          PrintSetting(Setting);
          SerState = SerCmd;
          break;

        case Val1:
          Value = Command - '0';
          SerState = Val2;
          break;

        case Val2:
          Value *= 10;
          Value += Command - '0';
          SerState = Val3;
          break;

        case Val3:
          Value *= 10;
          Value += Command - '0';
          UART_TxNewLine();
          WriteSetting(Setting, Value);
          PrintSetting(Setting);
          SerState = SerCmd;
          break;

  }
  return true;
}
