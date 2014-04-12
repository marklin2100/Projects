/*
  031-517-204/5/6 program

  This program is for the Lynx Innovation 031-517-204/5/6 Beats headphone
  and speaker interface board.
  http://ftp.lynxinnovation.com/lynxwiki/pmwiki.php?n=Main.031-517
  It communicates with the 031-517-202 system control board. It all controls the
  volume for LM4811 audio amps.  It also sets the brightness of a LED strip
  using PWM

  Created by David Annett 30 November 2012

  Copyright Lynx Innovation 2012
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

#if HWVer == 2
#include "UART.h"
#endif


//========================== Constants ========================================

// Run loop at 1kHz

#define LoopRate 1

// Loop rate ticks        SSSmmm
#define CommsIdleTimeout      10


#define LED_GRAPHICAL_ON()    SetBit(LED_GRAPICAL_DDR,LED_GRAPICAL); SetBit(LED_GRAPICAL_PORT,LED_GRAPICAL)
#define LED1_ON()             SetBit(LED1_DDR,LED1_Pin); SetBit(LED1_PORT,LED1_Pin)
#define LED1_OFF()            SetBit(LED1_DDR,LED1_Pin); ClearBit(LED1_PORT,LED1_Pin)
#define LED2_ON()             SetBit(LED2_DDR,LED2_Pin); SetBit(LED2_PORT,LED2_Pin)
#define LED2_OFF()            SetBit(LED2_DDR,LED2_Pin); ClearBit(LED2_PORT,LED2_Pin)
//========================== Types ============================================


//========================== Vars =============================================
Uint8 LampsOff[2];
Uint8 LampsOn[2];
Uint8  CmdRxBuf[1];
Uint8  RxData[9];

Uint8 a_Volume[4];
Uint8 LastVolume[4];

Uint8 ChannelNumbers;
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
        Volume_Set(WM8960Data,0);
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
  Uint8 Result;


  Result = 0;

  if (!GetBit(IDInPin,ID3In))
    Result += 2;

  if (!GetBit(IDInPin,ID2In))
    Result++;

  return Result;
}


/*
  ReadPosition routine

  This routine read the board bay position.
 */
Uint8 ReadPosition(void)
{
  Uint8 Result;


  Result = 0;

  if (!GetBit(IDInPin,ID1In))
    Result += 2;

  if (!GetBit(IDInPin,ID0In))
    Result++;

  return Result;
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
  LED_GRAPHICAL_ON();
}

void VarInit(void)
{
  unsigned char i;
  Uint8 Lamp;

  for(Lamp = 0; Lamp < 2; Lamp++)
  {
    LampsOff[Lamp] = 51;
    LampsOn[Lamp] = 255;
  }
  for(i = 0 ;i < 4; i++)
  {
    a_Volume[i] = 0;
    LastVolume[i] = 0;
  }

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
  WM8960_Init();//again
  VarInit();
}



void SetLamps(Uint8 NewLamps)
{
  if(NewLamps & 0b1)
    LED1 = LampsOn[0];
  else
    LED1 = LampsOff[0];

  if(NewLamps & 0b10)
    LED2 = LampsOn[1];
  else
    LED2 = LampsOff[1];
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
  Uint8   Param,Param2;
  Uint8   Volume = 0;
  Uint8   TempInt;
#ifdef CTRL_LED
  Uint8   TempInt;
  Uint8   RceiveAddress;
  Uint8   i;
  i = 0;
#endif
  MainInit();

  UART_TxStr("\r\nPower up\r\n");

  UART_Rx(RxData, 9);             // Input register setting command

  Address = BCAOutput + ReadPosition();
  BCMessageInit(Address);         // Set up the UART
  BCMessageReceive(RxBuf);        // Kick off receive

  if(ReadProductID()) // 1:pill 2:beats box 3:rave
    ChannelNumbers = 1;
  else
    ChannelNumbers = 5;//0:5 position headphone
  // Enter the main loop
  LED1_ON();
  LED2_ON();
  LED_GRAPHICAL_ON();
  SetLamps(3);
  for( ; ; ) {                              // Run forever
    Timer_Clear();
    DelayMS(LoopRate);
    TempInt++;
    SettingsControl();
    if (BCRXAvail)
    {                        // We have a new message
      //send data
#ifdef  SecondUART
#ifdef DumpComms
      UART_TxStr("Receive: ");
      for (TempInt = BCPSOH; TempInt <= BCPChecksum; TempInt++)
      {
        UART_TxUint8(RxBuf[TempInt]);
        UART_TxChar(' ');
      }
      UART_TxStr("\r\n");
#endif
#endif

      if ((RxBuf[BCPAddr] & 0b1111) == Address) // Check it is for us
      {
        Destination = RxBuf[BCPAddr] >> 4;  // Pre-setup assuming we will reply
        Destination &= 0b1111;
        Destination |= Address << 4;
        TxBuf[BCPAddr] = Destination;
        DelayMS(2);                         // Allow line turn around delay
        switch (RxBuf[BCPType])
        {
          case BCTInquire:                  // Master request of slave ID
            TxBuf[BCPType] = BCTInquireAnswer;
            TxBuf[BCPParam1] = ReadProductID();
            TxBuf[BCPParam2] = 0;
            BCMessageSend(TxBuf,true);      // Send the reply
            break;
          case BCTLamps: // Set lamps
            TxBuf[BCPType]   = BCTAck;
            TxBuf[BCPParam1] = 0;
            TxBuf[BCPParam2] = 0;
            BCMessageSend(TxBuf, true);           // Send the reply
            Param =  RxBuf[BCPParam1];
            SetLamps(Param);
            break;

          case BCTVolume:                   // Volume set
            TxBuf[BCPType] = BCTAck;
            TxBuf[BCPParam1] = 0;
            TxBuf[BCPParam2] = 0;
            BCMessageSend(TxBuf,true);      // Send the reply
            Volume = RxBuf[BCPParam1];       // Save parameter so we can receive next frame while processing this request
            Param2 = RxBuf[BCPParam2];       // Save parameter so we can receive next frame while processing this request
            Volume_Set(Volume,Param2);
            break;

          case BCTHeadphoneChGain:                   // Volume set
            TxBuf[BCPType] = BCTAck;
            TxBuf[BCPParam1] = 0;
            TxBuf[BCPParam2] = 0;
            BCMessageSend(TxBuf,true);      // Send the reply
            Param = RxBuf[BCPParam1];       // Save parameter so we can receive next frame while processing this request
            Param2 = RxBuf[BCPParam2];       // Save parameter so we can receive next frame while processing this request
            SetChannelAdjust(Param,Param2);
            break;

          case BCTHeadphoneChMax:                   // Volume set
            TxBuf[BCPType] = BCTAck;
            TxBuf[BCPParam1] = 0;
            TxBuf[BCPParam2] = 0;
            BCMessageSend(TxBuf,true);      // Send the reply
            Param = RxBuf[BCPParam1];       // Save parameter so we can receive next frame while processing this request
            Param2 = RxBuf[BCPParam2];       // Save parameter so we can receive next frame while processing this request
            SetChMaxVolume(Param, Param2);
            break;

          case BCTAudioFormat:              // Audio format set
            TxBuf[BCPType] = BCTAck;
            TxBuf[BCPParam1] = 0;
            TxBuf[BCPParam2] = 0;
            BCMessageSend(TxBuf,true);      // Send the reply
            Param = RxBuf[BCPParam1];       // Save parameter so we can receive next frame while processing this request
            WM8960_SetAudioFormat(Param);
            break;


          case BCTBrightness: // Set lamp brightness
            TxBuf[BCPType] = BCTAck;
            TxBuf[BCPParam1] = 0;
            TxBuf[BCPParam2] = 0;
            BCMessageSend(TxBuf, true);           // Send the reply
            Param = RxBuf[BCPParam1];       // Save parameter so we can receive next frame while processing this request
            Param2 = RxBuf[BCPParam2];       // Save parameter so we can receive next frame while processing this request
            if (Param == 1)
              LED1 = Param2;
            else if(Param == 2)
              LED2 = Param2;
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
           // BCMessageReceive(RxBuf);        // Kick off receive of next frame
            break;
        }

        //this send command tv change to 031-517-209 board
#if VideoTrackCtrl
        Uint8   VideoTrack;
        Uint8   SendVideoFlag;
        if(Volume > 47)
        {
          if(SendVideoFlag)
          {
            SendVideoFlag = false;

            if(Address == BCARightBay)
            {
              VideoTrack = 1;
            }
            else if(Address == BCALeftBay)
            {
              VideoTrack = 2;
            }

            TxBuf[BCPAddr] = Address << 4;
            TxBuf[BCPAddr] |= BDCLCD;
            TxBuf[BCPType]   = BCTPlayTrack;
            TxBuf[BCPParam1] = VideoTrack;
            TxBuf[BCPParam2] = 0;
            BCMessageSend(TxBuf, true);           // Send the reply
          }
        }
        else
        {
          SendVideoFlag = 1;
        }
#endif
      }


      //this for ctrl LED it itself
#ifdef CTRL_LED
      if(RxBuf[BCPType] == BCTVolume)
      {
        RceiveAddress = RxBuf[BCPAddr] & 0b1111;
        a_Volume[RceiveAddress-BCAOutput] = RxBuf[BCPParam1];

        if((a_Volume[RceiveAddress-BCAOutput] > 47) && (LastVolume[RceiveAddress-BCAOutput] > 47))//have volume
        {
          if((RceiveAddress) == Address)
          {
            LED1_ON();
          }
          else
          {
            LED1_OFF();
          }
        }

        i = 0;
        for(TempInt = 0; TempInt < 4; ) //all  volume off
        {
          if((a_Volume[TempInt] <= 47) && (LastVolume[TempInt] <= 47))
            i++;
          TempInt++;
        }

        if(i >= TempInt)
        {
          LED1_ON();
        }

        LastVolume[RceiveAddress-BCAOutput] = a_Volume[RceiveAddress-BCAOutput];
      }
#endif
      BCMessageReceive(RxBuf);        // Kick off receive of next frame
    }

  }
}
