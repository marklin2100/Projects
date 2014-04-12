/*
 * wm8960.c  --  WM8960 ALSA SoC Audio driver
 *
 * Author: Liam Girdwood
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */



#include "Wm8960.h"
#include "IIC.h"
#include "Globals.h"
#include "UART.h"
#include "CommonTypes.h"
#include "Timer.h"

Uint8 Volume = 0;
Uint8 AudioChannel;
//Power
//PWR 1
#define VREF_ON         (1 << 6)
#define AINL_ON         (1 << 5)
#define AINR_ON         (1 << 4)
#define ADCL_ON         (1 << 3)
#define ADCR_ON         (1 << 2)
#define MCIB_ON         (1 << 1)
#define DIGENB_ON       (0)

//PWR2
#define DACL_ON         (1 << 8)
#define DACR_ON         (1 << 7)
#define LOUT1_ON        (1 << 6)
#define ROUT1_ON        (1 << 5)
#define SPKL_ON         (1 << 4)
#define SPKR_ON         (1 << 3)
#define OUT3_ON         (1 << 1)
#define PLL_EN_ON       (1 << 0)

//PWR3 R47(2FH)
#define LMIC_ON          (1 << 5)
#define RMIC_ON          (1 << 4)
#define LOMIX_ON         (1 << 3)
#define ROMIX_ON         (1 << 2)

//R34 0X22 WM8960_LOUTMIX
#define LD2LO        (1 << 8)
#define LI2LO        (1 << 7)

//R37 0X25  WM8960 ROUTMIX
#define RD2RO        (1 << 8)
#define RI2RO        (1 << 7)

//R2 R3
#define OUT1VU       (1 << 8)
#define LO1ZC        (1 << 7)
Uint16 Out1Volume;//0x7f = +6DB step = 1Db


//R25
#define WMINDSEL5K   (0b11 << 7)


void WM8960Test(void)
{
  Uint8 i;
  while(1)
  {
    for(i = 1; i < 2; i++)
    {
      WM8960_WriteData(i,WM8960_ROUT1,0x159);// R3
      DelayMS(500);
    }
  }
}

Int8 ChannelVolume[5];

void WM8960_Init(void)
{
  Uint8 TempInt;
  for(TempInt = 1; TempInt <= 2; TempInt++)
  {
    WM8960_WriteData(TempInt,WM8960_RESET,0x00);                // R15
    WM8960_WriteData(TempInt,WM8960_RESET,0x00);                // R15
    WM8960_WriteData(TempInt,WM8960_RESET,0x00);                // R15
    WM8960_WriteData(TempInt,WM8960_RESET,0x00);                // R15
    WM8960_WriteData(TempInt,WM8960_CLOCK1,0x00);               // R4
    WM8960_WriteData(TempInt,WM8960_DACCTL1,0x00);              // R5
    WM8960_WriteData(TempInt,WM8960_IFACE1,0x02);               // R7 16bit i2s  6 =Enable slave mode   0e 32bit
    WM8960_WriteData(TempInt,WM8960_LDAC,0xff);                 // R10  DAC 0db
    WM8960_WriteData(TempInt,WM8960_RDAC,0x1ff);                // R11  DAC 0db
    WM8960_WriteData(TempInt,WM8960_POWER1,0x1c0);              // R25  0db  bit8 bit7 = 11 = 2 x 5kΩ divider enabled (for fast start-up)
    WM8960_WriteData(TempInt,WM8960_POWER2,0x1e0);              // R26  DACL DACR LOUT1 ROUT1
    WM8960_WriteData(TempInt,WM8960_LOUTMIX,0x100);             // R34  LD2LO
    WM8960_WriteData(TempInt,WM8960_ROUTMIX,0x100);             // R37  RD2RO
    WM8960_WriteData(TempInt,WM8960_POWER3,LOMIX_ON | ROMIX_ON);// enable LOMIX ROMIX   R47
    WM8960_WriteData(TempInt,WM8960_LOUT1,0x00);                // R2
    WM8960_WriteData(TempInt,WM8960_ROUT1,0x100);               // R3
  }
}

/*void WM8960_PathSelect(Uint8 Path)
{
  switch(Path)
  {
    case ANLOG:
      WM8960_WriteData(WM8960_LOUTMIX,0x80);//Left DAC to Left Output Mixer
      WM8960_WriteData(WM8960_ROUTMIX,0x80);//Left DAC to Left Output Mixer
      break;
    case DIGITAL:
      WM8960_WriteData(WM8960_LOUTMIX,0x100);//Left DAC to Left Output Mixer
      WM8960_WriteData(WM8960_ROUTMIX,0x100);//Left DAC to Left Output Mixer
      break;
    default:
      break;
  }
}
*/

void WM8960_SetVolume(Uint8 channel, Uint8 volume)
{
  volume = volume & 0b01111111;
  WM8960_WriteData(channel,WM8960_LOUT1,volume);//
  WM8960_WriteData(channel,WM8960_ROUT1,volume|0b100000000);//
  WM8960_WriteData(channel,WM8960_LOUT1,volume);//again
  WM8960_WriteData(channel,WM8960_ROUT1,volume|0b100000000);//
}


void WM8960_WriteData(Uint8 channel, Uint8 Register, Uint16 Data)
{
  Uint8 HightByte,LowByte;
  Uint8 I2C_Status;
  HightByte = Data >> 8;
  HightByte = HightByte & 0x01;
  HightByte |= (Register << 1);

  LowByte = Data; //bit 7 - 0

  IIC_Start(channel);
  IIC_Delay();
  if(!(IIC_WriteByte(channel,WM8960_Write_ADD)))
  {
    I2C_Status =  IIC_WriteByte(channel,HightByte);
    I2C_Status = IIC_WriteByte(channel,LowByte);
    IIC_Delay();
    IIC_Stop(channel);

#if DebugVerbosity > 1
    if(I2C_Status == 0)
    {
      UART_TxStr("Channel = ");
      UART_TxUint8(channel);
      UART_TxStr("Register = ");
      UART_TxUint8(Register);
      UART_TxStr(" Value = ");
      UART_TxUint8(Data>>8);
      UART_TxUint8(LowByte);
      UART_TxStr("\n\r");
    }
#endif
  }
}


#define MideaPlayer    0x0a
#define MP3Player      0x02

void WM8960_SetAudioFormat(Uint8 AudioFormat)
{
  Uint8 TempInt;
  Uint16 DataLength = MP3Player;
  switch(AudioFormat)
  {
    case 0:
      DataLength = MideaPlayer;
      break;
    case 1:
      DataLength = MP3Player;
      break;
  }

  for(TempInt = 1; TempInt <= 5; TempInt++)
  {
    WM8960_WriteData(TempInt, WM8960_IFACE1, DataLength);               //R7 16bit i2s  6 =Enable slave mode   0e 32bit
  }

}


