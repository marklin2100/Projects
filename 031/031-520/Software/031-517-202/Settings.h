#ifndef settings_h
#define settings_h
/*****************************************************************************

    Settings module

    This module implements settings stored in the AVR's EEPROM and updated
    from an I2C connected EEPROM or serial port command.
    See http://wiki.lynxinnovation.com/lynxwiki/pmwiki.php?n=Main.AVREEPROM

    David Annett, david@lynxinnovation.com, 18 February 2013

****************************************************************************/

//====================== Includes =============================================

#include "CommonTypes.h"
#include "Globals.h"

//====================== Constants ============================================


//====================== Types ================================================


//====================== Vars =================================================


//====================== Functions ============================================

/*
  SaveEEPROMSetting

  This routine saves the settings to the EEPROM
*/
void SaveEEPROMSetting(void);


/*
  LoadEEPROMSetting routine

  This routine loads settings from the EEPROM.  It will first check for
  a valid eternal EEPROM and if that fails will try the internal EEPROM.
  If that also fails it will load defaults from flash memory.  When either
  external EEPROM or flash is used the settings will be saved to the internal
  EEPROM for future use.
 */
void LoadEEPROMSetting(void);


/*
  SaveExternalEEPROM routine

  This routine attempts to save config settings to an external EEPROM.
  It will return true if the EEPROM was updated.
*/
bool SaveExternalEEPROM(void);


/*
  ReadExternalEEPROM routine

  This routine attempts to read config settings from an external EEPROM.
  It will return true if the Settings was updated.
*/
bool LoadExternalEEPROM(void);


/*
  PrintSettings routine

  This routine prints the current values of all settings.
 */
void PrintSettings(void);


/*
  SettingsControl routine

  This routine allows changing the settings via the serial port.  The received
  character should be passed in.  If it is understood it will return true.
 */
bool SettingsControl(Uint8 Command);

#endif
