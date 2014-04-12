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
#include "UARTPolled.h"
#include "AT24CXX.h"
#include "Timer.h"

//====================== Constants ============================================

#define EELocID         0x0000
#define EELocCustomer   0x0004
#define EELocProject    0x0006
#define EELocPartNumber 0x0008
#define EELocVersion    0x000A
#define EELocData       0x0010


Uint8 AT24C_PageBuffer[AT24C_PAGE_SIZE];
//====================== Types ================================================

//====================== Public vars ==========================================


//====================== Private vars =========================================

bool AT24WriteBlock(Uint16 mem_addr, Uint8 *p_data)
{
  return I2C_EEWriteBlock(AT24C_ADD,mem_addr,p_data,AT24C_PAGE_SIZE);
}

bool AT24ReadBlock(Uint16 mem_addr, Uint8 *p_data)
{
  return I2C_EEReadBlock(AT24C_ADD,mem_addr,p_data,AT24C_PAGE_SIZE);
}







