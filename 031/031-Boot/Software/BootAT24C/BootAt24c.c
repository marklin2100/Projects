/*
  031-517-202 Boot from sd card program

  This program is for the Lynx Innovation MP3 board boot from AT24C512.

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


extern TSettings Settings;

enum TBootStatus BootStatus;
// Types

// Vars
static volatile Uint16 AT24C_Address,AppFlashAddress;

// Forward declares
void PrintHexBlock(Uint16 Length,Uint8 *data)
{
#if Debug == 1
  Uint16 k;
  for(k = 0; k < Length; k++)
  {
    UARTTxHexByte(*data++);
    UARTTxStr(" ");
    if(k%16 == 0x0f)
      UARTTxStr("\r\n");
  }
  UARTTxStr("\r\n");
#endif
}

void BlinkLed(void)
{
  volatile static Uint8 USB_LED_STATUS = 0;
  if(USB_LED_STATUS)
  {
    SetBit(USBLEDPort,USBLED_516);
    SetBit(USBLEDPort,USBLED_517);
    USB_LED_STATUS = 0;
  }
  else
  {
    ClearBit(USBLEDPort,USBLED_516);
    ClearBit(USBLEDPort,USBLED_517);
    USB_LED_STATUS = 1;
  }
}
// Functions


void FlashToAT24(void)
{
  //copy flash to at24c
  volatile Uint16 AppFlashVar,AppFlashPageVar,AT24PageNum;
  Uint8  ChkSum;
  ChkSum = 0;
  AppFlashPageVar = 0;
  AT24C_Address = 0;
  AT24PageNum = 0;
  for(AppFlashVar = 0; AppFlashVar < APP_FLASH_MAX_ADD; AppFlashVar++)
  {
    AppFlashPageBuffer[AppFlashPageVar] = pgm_read_byte(AppFlashVar);
    ChkSum += AppFlashPageBuffer[AppFlashPageVar];
    AppFlashPageVar++;

    //one page
    if(AppFlashVar%AT24C_PAGE_SIZE == (AT24C_PAGE_SIZE-1))
    {
      UARTTxStr("Copy to AT24,finish ");
      UARTTxNum(++AT24PageNum);
      UARTTxStr("of");
      UARTTxNum(AT24C_AppFlash_PAGE_NUM);
      UARTTxStr("\r\n");
      PrintHexBlock(AT24C_PAGE_SIZE,AppFlashPageBuffer);

      AppFlashPageVar = 0;
      if(!AT24WriteBlock(AT24C_Address, AppFlashPageBuffer))
      {
        UARTTxStr("Write block error\r\n");
      }
      //I2C_EEWriteBlock(AT24C_ADD,AT24C_Address,AppFlashPageBuffer,AT24C_PAGE_SIZE);
#if Debug == 1
      AT24ReadBlock(AT24C_Address, AppFlashPageBuffer);
      //I2C_EEReadBlock(AT24C_ADD,AT24C_Address,AppFlashPageBuffer,AT24C_PAGE_SIZE);
      AT24C_Address += AT24C_PAGE_SIZE;
      PrintHexBlock(AT24C_PAGE_SIZE,AppFlashPageBuffer);
#elif Debug == 0
      AT24C_Address += AT24C_PAGE_SIZE;
#endif
      BlinkLed();
    }
  }
  eeprom_read_block((void *) &AppFlashPageBuffer, (const void *) 0x00, 64); // All ok so load and use
  AppFlashPageBuffer[EECrcSum] = ChkSum;
  AT24WriteBlock(AT24C_TSeeting_ADDRESS, AppFlashPageBuffer);
  //I2C_EEWriteBlock(AT24C_ADD,AT24C_TSeeting_ADDRESS,AppFlashPageBuffer,AT24C_PAGE_SIZE);
  PrintHexBlock(AT24C_PAGE_SIZE,AppFlashPageBuffer);
}

void AT24ToFlash(void)
{
  volatile Uint16 AppFlashPageVar,TempInt,AT24PageVar,AppFlashPageNum;
  Uint8 ChkSum;
  TSettings  *AT24CPtr;
  //crc sum
  ChkSum = 0;
  AT24C_Address = AT24C_AppFlash_ADDRESS;
  UARTTxStr("CRC sum please wait ..\r\n");
  for(AT24PageVar = 0; AT24PageVar < AT24C_AppFlash_PAGE_NUM; AT24PageVar++)
  {
    AT24ReadBlock(AT24C_Address, AppFlashPageBuffer);
   //I2C_EEReadBlock(AT24C_ADD,AT24C_Address,AppFlashPageBuffer,AT24C_PAGE_SIZE);
    AT24C_Address += AT24C_PAGE_SIZE;
    for(TempInt = 0; TempInt < AT24C_PAGE_SIZE; TempInt++)
    {
      ChkSum += AppFlashPageBuffer[TempInt];
    }
    BlinkLed();
  }

  AT24C_Address = AT24C_AppFlash_ADDRESS;
  AppFlashAddress = AT24C_AppFlash_ADDRESS;
  AppFlashPageNum = 0;
  AT24ReadBlock(AT24C_TSeeting_ADDRESS, AT24C_PageBuffer);
  AT24CPtr = (TSettings *) &AT24C_PageBuffer;

  if(AT24CPtr->Checksum == ChkSum)//crcsum == at24c in crcsum
  {
    //all right copy at24c to application flash
    UARTTxStr("The AT24C is new need update software\r\n");
    for(AppFlashPageVar = 0; AppFlashPageVar < APP_FLASH_MAX_PAGE; AppFlashPageVar++)
    {
      //AT24ReadBlock(AT24C_Address, AppFlashPageBuffer);
      I2C_EEReadBlock(AT24C_ADD,AT24C_Address,AppFlashPageBuffer,AT24C_PAGE_SIZE);
      AT24C_Address += AT24C_PAGE_SIZE;
      I2C_EEReadBlock(AT24C_ADD,AT24C_Address,&AppFlashPageBuffer[AT24C_PAGE_SIZE],AT24C_PAGE_SIZE);
      AT24C_Address += AT24C_PAGE_SIZE;
      UARTTxStr("Update flash from AT24,finish ");
      UARTTxNum(++AppFlashPageNum);
      UARTTxStr("of");
      UARTTxNum(APP_FLASH_MAX_PAGE);
      UARTTxStr("\r\n");
      PrintHexBlock(SPM_PAGESIZE,AppFlashPageBuffer);

      AppFlashWritePage(AppFlashAddress,AppFlashPageBuffer);
      AppFlashAddress += SPM_PAGESIZE;

        //read
      PrintHexBlock(SPM_PAGESIZE,AppFlashPageBuffer);
      BlinkLed();
    }
  }//end if(AT24CPtr->Checksum == ChkSum)//crcsum == at24c in crcsum
  else
  {
    UARTTxStr("CRC Sum bad please check  \r\n");
  }
}

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

  PORTC = PortDirC; //Trun off usb led
}



void StartingUpInformation(void)
{
  UARTTxStr("\r\n This is boot :\r\n");
  UARTTxStr(__TIME__);
  UARTTxStr(" ");
  UARTTxStr(__DATE__);
  UARTTxStr("\r\n");
}

void MainInit(void)
{
  PortInit();
  Timer_Init();                               // Set up timers
  UARTInit();
  cli();      // disable global interrupts

}
/*
  main routine
  Program entry point
 */
//int main(void) __attribute__((noreturn)); // Main never returns so don't waste stack space on it.
int main(void)
{
 // volatile Uint16 i,j,k;
 // Uint16 WordData;


  TSettings  *AT24CPtr;
  MainInit();
  StartingUpInformation();

  //Initiation variable
  BootStatus = NoStatus;
  AppFlashAddress = AT24C_AppFlash_ADDRESS;
  AT24C_Address = AT24C_AppFlash_ADDRESS;

  //Read a block from AT24C
  if(AT24ReadBlock(AT24C_TSeeting_ADDRESS, AT24C_PageBuffer))
  {
    AT24CPtr = (TSettings *) &AT24C_PageBuffer;
    //Read a block from EEPROM
    eeprom_read_block((void *) &Settings, (const void *) 0x00, sizeof(Settings)); // All ok so load and use

    if(Settings.LynxID == AT24CPtr->LynxID)//the external EEPROM have software
    {
      if((Settings.Customer == AT24CPtr->Customer) &&
         (Settings.Project == AT24CPtr->Project)&&
         (Settings.PartNumber == AT24CPtr->PartNumber))
      {
        if(Settings.Version == AT24CPtr->Version)
          BootStatus = AT24VerSame;
        else if(Settings.Version < AT24CPtr->Version)
          BootStatus = AT24VerHigth;
        else
          BootStatus = AT24VerLow;
      }
      else
      {
        BootStatus = AT24DataErr;
      }
    }
    else //this is a new external EEPROM
    {
      BootStatus = AT24NoData;
    }
  }
  else
  {
    BootStatus = NoAT24C;
  }


  switch(BootStatus)
  {
    case NoAT24C:
      UARTTxStr("external EEPROM  not found\r\n");
      break;
    case AT24NoData:
      UARTTxStr("The external EEPROM is empty,copy flash to external EEPROM\r\n");
      //copy flash data to at24c
      FlashToAT24();
      break;
    case AT24DataErr:
      UARTTxStr("The external EEPROM not for this project\r\n");
      break;
    case AT24VerSame:
      UARTTxStr("The version are same so nothing need to do\r\n");
      ClearBit(USBLEDPort,USBLED_517);//turn on USB led
      ClearBit(USBLEDPort,USBLED_516);
      DelayMS(1000);
      SetBit(USBLEDPort,USBLED_517);
      SetBit(USBLEDPort,USBLED_516); // turn off USB led
      break;
    case AT24VerLow:
      UARTTxStr("The external EEPROM version is low,copy flash to external EEPROM\r\n");
      //copy flash data to at24c
      FlashToAT24();
      break;
    case AT24VerHigth:
      UARTTxStr("The external EEPROM version is high,copy external EEPROM to flash\r\n");
      ClearBit(USBLEDPort,USBLED_517);//turn on USB led
      ClearBit(USBLEDPort,USBLED_516);
      //copy external EEPROM data to application flash
      AT24ToFlash();
      break;
    default:
      break;
  } //

  UARTTxStr("Boot is finish");
  asm("jmp 0x0000");
}



