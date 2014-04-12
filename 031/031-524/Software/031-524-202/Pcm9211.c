#include "CommonTypes.h"
#include"Pcm9211.h"
#include "iicio.h"
//#include "timer.h"
//#include"002-014-202.h"


/*
 * -10db  to +20 db step 2db  total 15 step
*/
void Pcm9211_Volume_Set(Uint8 Volume){
  if(Volume > 0x0f){
    Volume = 0x0f;
  }
  if(Volume){
    Volume = DigitalATTMin+Volume*6;
  }
  else {
    Volume = 0;   //No Volume
  }
  IICIO_WriteData(0x80,PCM9211_ADC_L_ATT_CON_ADD,Volume);
  IICIO_WriteData(0x80,PCM9211_ADC_R_ATT_CON_ADD,Volume);
}



void Pcm9211_Anlog(void)
{

  //IICIO_WriteData(0x80,0x40,0);

  //#System RST Control
 // IICIO_WriteData(0x80,0x40,0);
//  IICIO_WriteData(0x80,0x40,0x33);
 // IICIO_WriteData(0x80,0x40,0xD0);

  //#XTI Source, Clock (SCK/BCK/LRCK) Frequency Setting
  //# XTI CLK source 12.288 and BCK 3.072, LRCK 48k = XTI/512
//  IICIO_WriteData(0x80,0x31,0x1a);
//  IICIO_WriteData(0x80,0x33,0x22);
//  IICIO_WriteData(0x80,0x20,0x00);
 // IICIO_WriteData(0x80,0x24,0x00);

  //#ADC clock source is chosen by REG42
 // IICIO_WriteData(0x80,0x26,0x01);

  //#XTI Source, Secondary Bit/LR Clock (SBCK/SLRCK) Frequency Setting
//  IICIO_WriteData(0x80,0x33,0x22);

  /*
  //#*********************************************************
  //#-------------------------------Start DIR settings---------------------------------------
  //#REG. 21h, DIR Receivable Incoming Biphase's Sampling Frequency Range Setting
  IICIO_WriteData(0x80,0x21,0x00);

  //#REG. 22h, DIR CLKSTP and VOUT delay
  IICIO_WriteData(0x80,0x22,0x01);

  //#REG. 23h, DIR OCS start up wait time and Process for Parity Error Detection and ERROR Release Wait Time Setting
  IICIO_WriteData(0x80,0x23,0x04);

  //# REG 27h DIR Acceptable fs Range Setting & Mask
  IICIO_WriteData(0x80,0x27,0x00);

  //# REG 2Fh, DIR Output Data Format, 24bit I2S mode
  IICIO_WriteData(0x80,0x2F,0x04);

  //# REG. 30h, DIR Recovered System Clock (SCK) Ratio Setting
  IICIO_WriteData(0x80,0x30,0x02);

  //#REG. 32h, DIR Source, Secondary Bit/LR Clock (SBCK/SLRCK) Frequency Setting
  IICIO_WriteData(0x80,0x32,0x22);

  //#REG 34h DIR Input Biphase Signal Source Select and RXIN01 Coaxial Amplifier
  //#--PWR down amplifier, Select RXIN2
  //#w 80 34 C2
  //#--PWR up amplifier, select RXIN0
  IICIO_WriteData(0x80,0x34,0x0f);
  //#--PWR up amplifier, select RXIN1
  //#w 80 34 01

  IICIO_WriteData(0x80,0x35,0x0f);
  //IICIO_WriteData(0x80,0x36,0x0f);

  //#REG. 37h, Port Sampling Frequency Calculator Measurement Target Setting, Cal and DIR Fs
  IICIO_WriteData(0x80,0x37,0x07);
  //#REG 38h rd DIR Fs
  //r 80 38 01
  //#***********************************************************
  //#------------------------------------ End DIR settings------------------------------------------

  */
//  IICIO_WriteData(0x80,0x60,0x22);//DIT clock adc
//  IICIO_WriteData(0x80,0x61,0x10);
//  IICIO_WriteData(0x80,0x62,0x00);

  /*
  //#***********************************************************
  //#---------------------------------Start  MainOutput Settings--------------------------------------
  //#MainOutput
  //#REG. 6Ah, Main Output & AUXOUT Port Control
  IICIO_WriteData(0x80,0x6A,0x00);

  //#REG. 6Bh, Main Output Port (SCKO/BCK/LRCK/DOUT) Source Setting
  IICIO_WriteData(0x80,0x6B,0x22);

  //#REG. 6Dh, MPIO_B & Main Output Port Hi-Z Control
  IICIO_WriteData(0x80,0x6D,0xff);
  //#***********************************************************
  //#------------------------------------ End MainOutput settings------------------------------------------

  //# read back all registers to ensure GUI integrity
  //r 80 20 5E
  */
  IICIO_WriteData(0x80,0x40,0xD0);
  IICIO_WriteData(0x80,0x60,0x22);//DIT clock adc
  IICIO_WriteData(0x80,0x78,0xDD); //
}

void Pcm9211_Digital(void)
{
  //IICIO_WriteData(0x80,0x40,0xD0);
  IICIO_WriteData(0x80,0x40,0xE0);
  IICIO_WriteData(0x80,0x78,0xEE); //
}

void Pcm9211_AnalogToDigital(void)
{
  IICIO_WriteData(0x80,0x40,0xD0);//Power up ADC.Power down DIT
  IICIO_WriteData(0x80,0x6c,0x22);//AUX Output Port   SCK Source Control BCK/LRCK/DATA Source Control  ADC
}

void Pcm9211_Init(void)
{
  void Pcm9211_AnalogToDigital(void);
}



void Pcm9211_WriteByte(unsigned char wd)
{


}

