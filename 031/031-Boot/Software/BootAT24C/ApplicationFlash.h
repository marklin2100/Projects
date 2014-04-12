#ifndef application_flash_h
#define application_flash_h
/*****************************************************************************

    I2C_Multi module

    This module implements a bit bashed I2C interface

    David Annett, david@lynxinnovation.com, 15 February 2013

****************************************************************************/

//====================== Includes =============================================

#include "CommonTypes.h"

//====================== Constants ============================================

#define APP_FLASH_MAX_ADD       0xE000  //56k*1024
#define APP_FLASH_MAX_PAGE      224//APP_FLASH_MAX_ADD/SPM_PAGESIZE // 0XE000/256 = 224

#define FLASH_SIZE            0x10000 //ATMEGA644
#define BOOTFLASH_SIZE        0x2000 //ATMEGA644
//====================== Types ================================================


//====================== Vars =================================================


extern Uint8 AppFlashPageBuffer[SPM_PAGESIZE];

//====================== Functions ============================================

/*
  FlashReadPage

  Inits bit banging port, must be called before using the other functions.
*/
void FlashReadPage(Uint16 Address);
void AppFlashWritePage(Uint16 FlashAddress,Uint8 *data);

#endif
