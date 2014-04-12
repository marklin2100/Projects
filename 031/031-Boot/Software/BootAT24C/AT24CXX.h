#ifndef at24cxx_h
#define at24cxx_h
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

#define e2prom 512    //
#if e2prom == 02
  #define AT24C_PAGE_SIZE  64
  #define AT24C_PAGE_NUM   0x04
  #define AT24C_AppFlash_PAGE_NUM  4
  #define AT24C_TEST_ADD           255
#elif e2prom==512
  #define AT24C_PAGE_SIZE          128
  #define AT24C_PAGE_NUM           512
  #define AT24C_AppFlash_PAGE_NUM  448
  #define AT24C_TEST_ADD           0XF000
#endif
//--------------------------

#define AT24C_ADD 0xa0  //
#define AT24C_AppFlash_ADDRESS   0X00
#define AT24C_TSeeting_ADDRESS   0XE000
//====================== Types ================================================


//====================== Vars =================================================
extern Uint8 AT24C_PageBuffer[AT24C_PAGE_SIZE];

//====================== Functions ============================================




bool AT24WriteBlock(Uint16 mem_addr, Uint8 *p_data);
bool AT24ReadBlock(Uint16 mem_addr, Uint8 *p_data);






#endif
