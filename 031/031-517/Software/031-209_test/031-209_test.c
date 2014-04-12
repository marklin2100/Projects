/*
  031-517-204/5/6 program

  This program is for the Lynx Innovation 031-514-204/5/7/8 Beats headphone
  and speaker interface board.
  http://ftp.lynxinnovation.com/lynxwiki/pmwiki.php?n=Main.031-51
  It communicates with the 031-516-202 system control board. It all controls the
  volume for LM4811 audio amps.  It also sets the brightness of a LED strip
  using PWM

  Created by David Annett 6 February

  Copyright Lynx Innovation 2013
 */

//========================== Includes =========================================

#include <avr/pgmspace.h>
#include "CommonTypes.h"
#include "Globals.h"
#include "Timer.h"
#include "BoardComms.h"
#include "Volume.h"
#include "Wm8960.h"
#include "IIC.h"
#include "hw.h"

#if HWVer == 2
#include "UART.h"
#endif


//========================== Constants ========================================

// Run loop at 1kHz

#define LoopRate 1

// Loop rate ticks        SSSmmm
#define CommsIdleTimeout      10


//========================== Types ============================================


//========================== Vars =============================================

Uint8  CmdRxBuf[1];
Uint8  RxData[9];


//========================== Functions =======================================

/*
  SettingsControl routine

  This routine allows changing the settings via the serial port.
 */
void SettingsControl(void)
{
  Uint8 Command;
  Uint8 TempInt,channel;
  Uint16 TempData, WM8960Data;
  Uint8 WM8960Register;


  // See if new command available

  if (RxAvail < 9)
    return;

  // Grab it and kick of the next read

  UART_Rx(RxData, 9);

  Command = RxData[0];
  // Process the command

  // s channel data
  switch (Command)
  {
    case 's':  //
    case 'S':
      for(TempInt = 1; TempInt < 9; TempInt++)
      {
        switch(RxData[TempInt])
        {
          case 'a':
          case 'A':
            RxData[TempInt] = 0xa;
            break;
          case 'b':
          case 'B':
            RxData[TempInt] = 0xb;
            break;
          case 'c':
          case 'C':
            RxData[TempInt] = 0xc;
            break;
          case 'd':
          case 'D':
            RxData[TempInt] = 0xd;
            break;
          case 'e':
          case 'E':
            RxData[TempInt] = 0xe;
            break;
          case 'f':
          case 'F':
            RxData[TempInt] = 0xf;
            break;
          default:
            if((RxData[TempInt] >= '0') && (RxData[TempInt] <= '9'))
              RxData[TempInt] = RxData[TempInt] - '0';
            else
              RxData[TempInt] = 0;
            break;
        }
      }

      channel =  RxData[1] << 4;
      channel |= RxData[2];

      WM8960Register = RxData[3] << 4;
      WM8960Register |= RxData[4];

      TempData = RxData[5];
      TempData = TempData << 12;
      WM8960Data = TempData;
      TempData = RxData[6];
      TempData = TempData << 8;
      WM8960Data |= TempData;
      TempData = RxData[7];
      TempData = TempData << 4;
      WM8960Data |= TempData;
      WM8960Data |= RxData[8];

      WM8960_WriteData(channel,WM8960Register,WM8960Data);

      UART_Rx(RxData, 9);//receive

      if(WM8960Register == 0)
      {
        Volume_Set(WM8960Data);
      }
      break;

    default:
      UART_Rx(RxData, 9);
      UART_TxStr("?");
      UART_TxNum(Command);
      UART_TxStr("?\r\n");
      break;
  }
}



/*
  ReadProductID routine

  This routine read the product ID.
 */
Uint8 ReadProductID(void)
{
  /*if (!GetBit(IDInPin,ID1In))
    return 1;

  if (!GetBit(IDInPin,ID2In))
    return 2;

  if (!GetBit(IDInPin,ID3In))
    return 3;*/

  return ProductID;



/* Not using binary coding anymore
  Uint8 Result;


  Result = 0;

  if (!GetBit(IDInPin,ID3In))
    Result += 2;

  if (!GetBit(IDInPin,ID2In))
    Result++;

  return Result;
*/
}


/*
  ReadPosition routine

  This routine read the board bay position.
  Returns 0 as only one position.
 */
Uint8 ReadPosition(void)
{
/*

  Uint8 Result;


  Result = 0;

  if (!GetBit(IDInPin,ID1In))
    Result += 2;

  if (!GetBit(IDInPin,ID0In))
    Result++;

  return Result;
*/
  return 0;
}



/*
  PortInit routine

  This routine prepares the I/O ports for use.
*/
void PortInit(void)
{
  DDRA = PortDirA;
  PINA = PortPullUpA;
  PORTA = PortPullUpA;
  DDRB = PortDirB;
  DDRC = PortDirC;
  DDRD = PortDirD;


  PINB = PortPullUpB;
  PINC = PortPullUpC;
  PIND = PortPullUpD;


  PORTB = PortPullUpB;
  PORTB = PortPullUpC;
  PORTB = PortPullUpD;

  //Enable 74HC245 Input A Output B
  HC245Port |= (1 << HC245DIR);
  HC245Port &= (~(1 << HC245OE));
}



/*
  MainInit routine

  This routine performs the main initialisation.
*/
void MainInit(void)
{
  PortInit();
  Timer_Init();
  IIC_Init();
  UART_Init();
  sei();    // Enable global interrupts
  WM8960_Init();
}



/*
  main routine

  Program entry point
*/
int main(void)
{
  Uint8   TxBuf[BCMsgSize];
  Uint8   RxBuf[BCMsgSize];
  Uint8   Address;            // Address of device in bay
  Uint8   Destination;        // Address we will reply to
  Uint8   Param;

  volatile Uint16 SendTimes = 0;


  MainInit();

  UART_TxStr("\r\nPower up\r\n");

  UART_Rx(RxData, 9);             // Input register setting command

  Address = BCAOutput + ReadPosition();
  BCMessageInit(BCAMP3Contoller);         // Set up the UART
  BCMessageReceive(RxBuf);        // Kick off receive

  // Enter the main loop

  for( ; ; ) {                              // Run forever
    DelayMS(LoopRate);
    SettingsControl();


    SendTimes ++;
    if(SendTimes > 1000)
    {
      SendTimes = 0;
      /*TxBuf[BCPAddr] = BDCLCD;
      TxBuf[BCPType] = BCTInquireAnswer;
      TxBuf[BCPParam1] = 0;
      TxBuf[BCPParam2] = 0;
      BCMessageSend(TxBuf,true);      // Send the reply
      */

      if(ExchangeBoardMsg(BDCLCD, BCTInquire, 0, 0, BCTInquireAnswer))
      {
        UART_TxStr("Receive Inquire answer,So Sent volume is 115\r\n");
        Volume_Set(115);
      }
      else
      {
        UART_TxStr("Can't receive Inquire answer,Sent volume is 0\r\n");
        Volume_Set(0);
      }

    }

    /*if (BCRXAvail) {                        // We have a new message
      if ((RxBuf[BCPAddr] & 0b1111) == Address)  { // Check it is for us
        Destination = RxBuf[BCPAddr] >> 4;  // Pre-setup assuming we will reply
        Destination &= 0b1111;
        Destination |= Address << 4;
        TxBuf[BCPAddr] = Destination;
        DelayMS(2);                         // Allow line turn around delay
        switch (RxBuf[BCPType]) {
          case BCTInquire:                  // Master request of slave ID
            TxBuf[BCPType] = BCTInquireAnswer;
            TxBuf[BCPParam1] = ReadProductID();
            TxBuf[BCPParam2] = 0;
            BCMessageSend(TxBuf,true);      // Send the reply
            BCMessageReceive(RxBuf);        // Kick off receive of next frame
            break;

          case BCTVolume:                   // Volume set
            TxBuf[BCPType] = BCTAck;
            TxBuf[BCPParam1] = 0;
            TxBuf[BCPParam2] = 0;
            BCMessageSend(TxBuf,true);      // Send the reply
            Param = RxBuf[BCPParam1];       // Save parameter so we can receive next frame while processing this request
            BCMessageReceive(RxBuf);        // Kick off receive of next frame
//            Timer_Clear();
            Volume_Set(Param);
//            UART_TxStr("Volume_Set took ");
//            UART_TxNum(Timer_Read());
//            UART_TxStr("mS\r\n");
            break;

          case BCTAudioFormat:              // Audio format set
            TxBuf[BCPType] = BCTAck;
            TxBuf[BCPParam1] = 0;
            TxBuf[BCPParam2] = 0;
            BCMessageSend(TxBuf,true);      // Send the reply
            Param = RxBuf[BCPParam1];       // Save parameter so we can receive next frame while processing this request
            BCMessageReceive(RxBuf);        // Kick off receive of next frame
//            Timer_Clear();
            WM8960_SetAudioFormat(Param);
//            UART_TxStr("WM8960_SetAudioFormat took ");
//            UART_TxNum(Timer_Read());
//            UART_TxStr("mS\r\n");
            break;

          case BCTReset:                   // Volume set
            TxBuf[BCPType] = BCTAck;
            TxBuf[BCPParam1] = 0;
            TxBuf[BCPParam2] = 0;
            BCMessageSend(TxBuf,true);      // Send the reply
            Param = RxBuf[BCPParam1];       // Save parameter so we can receive next frame while processing this request
            BCMessageReceive(RxBuf);        // Kick off receive of next frame
            asm("jmp 0x0000");//reset
            break;

          default:  // Unknown command
            TxBuf[BCPType] = BCTNAck;
            TxBuf[BCPParam1] = BCNUnkownType;
            TxBuf[BCPParam2] = RxBuf[BCPType];
            BCMessageSend(TxBuf,true);      // Send the reply
            BCMessageReceive(RxBuf);        // Kick off receive of next frame
            break;
        }
      }
    }*/
  }
}
