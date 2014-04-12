/*
  031-517-202 Boot from sd card program

  This program is for the Lynx Innovation MP3 board boot from sd card.

  Created by Mark Lin  28 Mar 2013

  Copyright Lynx Innovation 2013
 */

#include "CommonTypes.h"
#include "Globals.h"
#include "Timer.h"
#include "UARTPolled.h"
#include <avr/pgmspace.h>
#include <avr/boot.h>
#include "AT24CXX.h"
#include "I2C.h"
#include "ApplicationFlash.h"
// Constants
#define EELocID         0x0000
#define EELocCustomer   0x0004
#define EELocProject    0x0006
#define EELocPartNumber 0x0008
#define EELocVersion    0x000A
#define EECrcSum        0x000F
#define EELocData       0x0010
// Keys and lamps

#define CHECK_DATA      0X55;
extern Uint8 I2C_Status;   // 0 = OK, else ErrorNoACK...
volatile Uint8 USB_LED_STATUS = 0;
// Types

// Vars


// Forward declares



// Functions




void PortInit(void)
{
  // Set up the I/O lines
  DDRA = PortDirA;
  DDRB = PortDirB;
  DDRC = PortDirC;
  DDRD = PortDirD;

  PINA = PortPullUpA;
  PINB = PortPullUpB;
  PINC = PortPullUpC;
  PIND = PortPullUpD;


}


/*void BootInit(void)
{
}
*/

void MainInit(void)
{
  PortInit();
  Timer_Init();                               // Set up timers
  UARTInit();
  //BootInit();
}
volatile static Uint8 data,data1;
/*
  main routine
  Program entry point
 */
//int main(void) __attribute__((noreturn)); // Main never returns so don't waste stack space on it.
int main(void)
{
  volatile Uint16 i,j,k;
  static volatile Uint16 AT24C_Address,AppFlashAddress;
  Uint16 WordData;

  Uint8 ChkSum;
  TSettings  *AT24CPtr;
  MainInit();

  cli();      // Enable global interrupts

  // start
  UARTTxStr("\r\nthis is boot\r\n");
  UARTTxStr(__TIME__);
  UARTTxStr(" ");
  UARTTxStr(__DATE__);
  UARTTxStr("\r\n");

  //UART_Rx(CmdRxBuf, 1);
  AppFlashAddress = AT24C_AppFlash_ADDRESS;
  AT24C_Address = AT24C_AppFlash_ADDRESS;
  ChkSum = 0;
  // Init the peripherals
  data = CHECK_DATA;
  data1 = CHECK_DATA;
  I2C_EEWriteByte(0xA0, AT24C_TEST_ADD,data);
  data = 0;
  data = I2C_EEReadByte(0xA0, AT24C_TEST_ADD);
  if(I2C_Status)
  {
    UARTTxStr("external EEPROM  not found\r\n");
    DelayMS(50);
    asm("jmp 0x0000");
    return 0;
  }
  else
  {
    UARTTxStr("found external EEPROM  \r\n");
  }


  AT24CTest();

  //Read set from AT24C
  AT24ReadBlock(AT24C_TSeeting_ADDRESS, AT24C_PageBuffer);
  AT24CPtr = (TSettings *) &AT24C_PageBuffer;
  //Read set from eeprom
  //eeprom_read_block((void *) &Settings, (const void *) 0x00, sizeof(Settings));

  if ((eeprom_read_dword((uint32_t *) EELocID) == AT24CPtr->LynxID))      // Check the board type
  {
    if((eeprom_read_word((uint16_t *) EELocCustomer)   == AT24CPtr->Customer) &&
        (eeprom_read_word((uint16_t *) EELocProject)    == AT24CPtr->Project) &&
        (eeprom_read_word((uint16_t *) EELocPartNumber) == AT24CPtr->PartNumber))
    {
      if((eeprom_read_byte((uint8_t *) EELocVersion) < AT24CPtr->Version))//AT24C is new
      {
         //crc sum
        /*for(i = 0; i < AT24C_AppFlash_PAGE_NUM; i++)
        {
          //AT24ReadBlock(AT24C_Address, AppFlashPageBuffer);
          I2C_EEReadBlock(AT24C_ADD,AT24C_Address,AppFlashPageBuffer,AT24C_PAGE_SIZE);
          AT24C_Address += AT24C_PAGE_SIZE;
          for(j = 0; j < AT24C_PAGE_SIZE; j++)
          {
            ChkSum += AppFlashPageBuffer[j];
          }
        }*/

        AT24C_Address = AT24C_AppFlash_ADDRESS;
        AppFlashAddress = AT24C_AppFlash_ADDRESS;
        //if(AT24CPtr->Checksum == ChkSum)//crcsum == at24c in crcsum
       // {
           //all right copy at24c to application flash
           UARTTxStr("The AT24C is new need update software\r\n");
           for(i = 0; i < APP_FLASH_MAX_PAGE; i++)
           {
             //AT24ReadBlock(AT24C_Address, AppFlashPageBuffer);
             I2C_EEReadBlock(AT24C_ADD,AT24C_Address,AppFlashPageBuffer,AT24C_PAGE_SIZE);
             AT24C_Address += AT24C_PAGE_SIZE;
             I2C_EEReadBlock(AT24C_ADD,AT24C_Address,&AppFlashPageBuffer[AT24C_PAGE_SIZE],AT24C_PAGE_SIZE);
             AT24C_Address += AT24C_PAGE_SIZE;

             for(k = 0; k < SPM_PAGESIZE; k++)
             {
               UARTTxHexByte(AppFlashPageBuffer[k]);//print
               UARTTxStr(" ");
               if(k%16 == 0x0f)
                 UARTTxStr("\r\n");
             }
             UARTTxStr("\r\n");

             //AppFlashWritePage(AppFlashAddress,AppFlashPageBuffer);



               boot_rww_enable();
               boot_page_erase(AppFlashAddress);     //擦除一个Flash页 按页的起始地址查询
               boot_spm_busy_wait();       //等待页擦除完成
               for(k = 0;k < SPM_PAGESIZE;k += 2)       //将数据填入Flash缓冲页中
               {
                 WordData = AppFlashPageBuffer[k] + (AppFlashPageBuffer[k + 1] << 8);
                 boot_page_fill(k, WordData);//填充为16bit的页地址
               }
               boot_page_write(AppFlashAddress);     //将缓冲页数据写入一个Flash页
               boot_spm_busy_wait();       //等待页编程完成

               boot_rww_enable();


               AppFlashAddress += SPM_PAGESIZE;

               //read
               UARTTxStr("this is read from flash\r\n");
               for(k = 0; k < SPM_PAGESIZE; k++)
               {
                 UARTTxHexByte(pgm_read_byte(i*SPM_PAGESIZE + k));
                 UARTTxStr(" ");
                 if(k%16 == 0x0f)
                   UARTTxStr("\r\n");
               }
               UARTTxStr("\r\n");

              if(USB_LED_STATUS)
              {
                SetBit(USBLEDPort,USBLED);
                USB_LED_STATUS = 0;
              }
              else
              {
                ClearBit(USBLEDPort,USBLED);
                USB_LED_STATUS = 1;
              }
           }
        }//end if(AT24CPtr->Checksum == ChkSum)//crcsum == at24c in crcsum
        //else
        //{
          // UARTTxStr("CRC Sum bad please check  \r\n");
       // }
      //}//end if((eeprom_read_byte((uint8_t *) EELocVersion) < AT24CPtr->Version))//AT24C is new
      else if((eeprom_read_word((uint16_t *) EELocVersion) == AT24CPtr->Version))
      {
         //nothing to do
         UARTTxStr("The version the same nothing need to do\r\n");
      }
      else
      {
         //at24c is old
         UARTTxStr("The AT24C is old so copy new to AT24C\r\n");
         //copy flash to at24c
         j = 0;
         AT24C_Address = 0;
         for(i = 0; i < APP_FLASH_MAX_ADD; i++)
         {
           AppFlashPageBuffer[j] = pgm_read_byte(i);
           ChkSum += AppFlashPageBuffer[j];
           UARTTxHexByte(AppFlashPageBuffer[j]);//print
           UARTTxStr(" ");
           if(j%16 == 0x0f)
             UARTTxStr("\r\n");
           j++;
           if(i%AT24C_PAGE_SIZE == (AT24C_PAGE_SIZE-1))
           {
             j = 0;
             UARTTxStr("\r\n");
             //AT24WriteBlock(i, AppFlashPageBuffer);

             I2C_EEWriteBlock(AT24C_ADD,AT24C_Address,AppFlashPageBuffer,AT24C_PAGE_SIZE);

             UARTTxStr("The read from at24c\r\n");

             for(k = 0; k < AT24C_PAGE_SIZE; k++)
             {
               AppFlashPageBuffer[k] = 0;
             }

             //AT24ReadBlock(i, AppFlashPageBuffer);
             I2C_EEReadBlock(AT24C_ADD,AT24C_Address,AppFlashPageBuffer,AT24C_PAGE_SIZE);
             AT24C_Address += AT24C_PAGE_SIZE;
             for(k = 0; k < AT24C_PAGE_SIZE; k++)
             {
               UARTTxHexByte(AppFlashPageBuffer[k]);//print
               UARTTxStr(" ");
               if(k%16 == 0x0f)
                 UARTTxStr("\r\n");
             }
             UARTTxStr("\r\n");

             if(USB_LED_STATUS)
             {
               SetBit(USBLEDPort,USBLED);
               USB_LED_STATUS = 0;
             }
             else
             {
               ClearBit(USBLEDPort,USBLED);
               USB_LED_STATUS = 1;
             }

           }
         }
         eeprom_read_block((void *) &AppFlashPageBuffer, (const void *) 0x00, 128); // All ok so load and use
         AppFlashPageBuffer[EECrcSum] = ChkSum;
         //AT24WriteBlock(AT24C_TSeeting_ADDRESS, AppFlashPageBuffer);
         I2C_EEWriteBlock(AT24C_ADD,AT24C_TSeeting_ADDRESS,AppFlashPageBuffer,AT24C_PAGE_SIZE);

         for(k = 0; k < AT24C_PAGE_SIZE; k++)
         {
           UARTTxHexByte(AppFlashPageBuffer[k]);//print
           UARTTxStr(" ");
           if(k%16 == 0x0f)
             UARTTxStr("\r\n");
         }
         UARTTxStr("\r\n");
      }
    }//end if((eeprom_read_word((uint16_t *) EELocCustomer)   == AT24CPtr->Customer) &&
     //       (eeprom_read_word((uint16_t *) EELocProject)    == AT24CPtr->Project) &&
     //       (eeprom_read_word((uint16_t *) EELocPartNumber) == AT24CPtr->PartNumber))
    else
    {
       //the customer project partNumber not equip so nothing to do
       UARTTxStr("This software not correct nothing to do\r\n");
    }
  }//end if ((eeprom_read_dword((uint32_t *) EELocID) == AT24CPtr->LynxID))      // Check the board type
  else //at24c is null or no lynxID
  {
    //copy flash to at24c
    UARTTxStr("This AT24C is a null or no LYNX ID, so copy flash to AT24C\r\n");
    //copy flash to at24c
    for(i = 0; i < APP_FLASH_MAX_ADD; i++)
    {
      AppFlashPageBuffer[j] = pgm_read_byte(i);
      ChkSum += AppFlashPageBuffer[j];
      UARTTxHexByte(AppFlashPageBuffer[j]);//print
      UARTTxStr(" ");
      if(j%16 == 0x0f)
        UARTTxStr("\r\n");
      j++;
      AT24C_Address = 0;
      if(i%AT24C_PAGE_SIZE == (AT24C_PAGE_SIZE-1))
      {
        j = 0;
        UARTTxStr("\r\n");
        //AT24WriteBlock(i, AppFlashPageBuffer);
        I2C_EEWriteBlock(AT24C_ADD,AT24C_Address,AppFlashPageBuffer,AT24C_PAGE_SIZE);


        UARTTxStr("The read from at24c\r\n");

        for(k = 0; k < AT24C_PAGE_SIZE; k++)
        {
          AppFlashPageBuffer[k] = 0;
        }

        //AT24ReadBlock(i, AppFlashPageBuffer);
        I2C_EEReadBlock(AT24C_ADD,AT24C_Address,AppFlashPageBuffer,AT24C_PAGE_SIZE);
        AT24C_Address += AT24C_PAGE_SIZE;
        for(k = 0; k < AT24C_PAGE_SIZE; k++)
        {
          UARTTxHexByte(AppFlashPageBuffer[k]);//print
          UARTTxStr(" ");
          if(k%16 == 0x0f)
            UARTTxStr("\r\n");
        }
        UARTTxStr("\r\n");

        if(USB_LED_STATUS)
        {
          SetBit(USBLEDPort,USBLED);
          USB_LED_STATUS = 0;
        }
        else
        {
          ClearBit(USBLEDPort,USBLED);
          USB_LED_STATUS = 1;
        }

      }
    }
    eeprom_read_block((void *) &AppFlashPageBuffer, (const void *) 0x00, 128); // All ok so load and use
    AppFlashPageBuffer[EECrcSum] = ChkSum;
    //AT24WriteBlock(AT24C_TSeeting_ADDRESS, AppFlashPageBuffer);
    I2C_EEWriteBlock(AT24C_ADD,AT24C_TSeeting_ADDRESS,AppFlashPageBuffer,AT24C_PAGE_SIZE);

    for(k = 0; k < AT24C_PAGE_SIZE; k++)
    {
      UARTTxHexByte(AppFlashPageBuffer[k]);//print
      UARTTxStr(" ");
      if(k%16 == 0x0f)
        UARTTxStr("\r\n");
    }
    UARTTxStr("\r\n");


  }
  UARTTxStr("Boot is finish");

  asm("jmp 0x0000");
  return 0;
}



