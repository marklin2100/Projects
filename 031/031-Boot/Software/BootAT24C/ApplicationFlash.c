/*****************************************************************************

    Application flash mode


    Mark Lin, Mark@lynxinnovation.cn, 02 Apr 2013

****************************************************************************/

//====================== Includes =============================================

#include <stdio.h>
#include "CommonTypes.h"
#include "Globals.h"
#include "ApplicationFlash.h"
#include <avr/boot.h>


//====================== Constants ============================================

// TODO: Optimise I2CSPEED for fastest reliable transfers



//====================== Types ================================================


//====================== Vars =================================================
Uint8 AppFlashPageBuffer[SPM_PAGESIZE];



//====================== Functions ============================================

/*
  FlashRead

  Initialises bit banging ports, must be called before using the other functions.
*/
void AppFlashWritePage(Uint16 FlashAddress,Uint8 *data)
{
  Uint16 i,IntData;
  boot_page_erase(FlashAddress);     //erase on page
  boot_spm_busy_wait();              //
  for(i=0;i<SPM_PAGESIZE;i+=2)       //
  {
    IntData = *(data+i) + (*(data+i+1)<<8);
    boot_page_fill(i, IntData);//
  }
  boot_page_write(FlashAddress);     //
  boot_spm_busy_wait();       //
  boot_rww_enable();
}

