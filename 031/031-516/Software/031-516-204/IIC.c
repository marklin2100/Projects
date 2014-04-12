#include "CommonTypes.h"

#include "IIC.h"
#include "Globals.h"
#include "UART.h"


void IIC_Delay(void)
{
  volatile int i = 5;//
  while(i--);
}


//bool SlaveAck_Flag;
//unsigned char IIC_Status;

void IIC_Init(void)
{
  //iic sda scl out
  IIC_SCL_OUT(1);
  IIC_SDA_OUT(1);
  IIC_SCL_OUT(2);
  IIC_SDA_OUT(2);
  IIC_SCL_OUT(3);
  IIC_SDA_OUT(3);
  IIC_SCL_OUT(4);
  IIC_SDA_OUT(4);
  IIC_SCL_OUT(5);
  IIC_SDA_OUT(5);

  IIC_Delay();
  IIC_SDA_HI(1);
  IIC_SDA_HI(2);
  IIC_SDA_HI(3);
  IIC_SDA_HI(4);
  IIC_SDA_HI(5);
  IIC_Delay();
  IIC_SCL_HI(1);
  IIC_SCL_HI(2);
  IIC_SCL_HI(3);
  IIC_SCL_HI(4);
  IIC_SCL_HI(5);
  IIC_Delay();

}




void IIC_Start(Uint8 channel)
{
  switch(channel)
  {
    case 1:
      IIC_SDA_HI(1);
      IIC_SCL_HI(1);
      IIC_Delay();
      IIC_SDA_LO(1);
      IIC_Delay();
      IIC_SCL_LO(1);
      IIC_Delay();
      break;
    case 2:
      IIC_SDA_HI(2);
      IIC_SCL_HI(2);
      IIC_Delay();
      IIC_SDA_LO(2);
      IIC_Delay();
      IIC_SCL_LO(2);
      IIC_Delay();
      break;
    case 3:
      IIC_SDA_HI(3);
      IIC_SCL_HI(3);
      IIC_Delay();
      IIC_SDA_LO(3);
      IIC_Delay();
      IIC_SCL_LO(3);
      IIC_Delay();
      break;
    case 4:
      IIC_SDA_HI(4);
      IIC_SCL_HI(4);
      IIC_Delay();
      IIC_SDA_LO(4);
      IIC_Delay();
      IIC_SCL_LO(4);
      IIC_Delay();
      break;
    case 5:
      IIC_SDA_HI(5);
      IIC_SCL_HI(5);
      IIC_Delay();
      IIC_SDA_LO(5);
      IIC_Delay();
      IIC_SCL_LO(5);
      IIC_Delay();
      break;
    default:
      break;
  }
}




void IIC_Stop(Uint8 channel)
{
  switch(channel)
  {
    case 1:
      IIC_Delay();
      IIC_SCL_HI(1);
      IIC_Delay();
      IIC_SDA_HI(1);
      break;
    case 2:
      IIC_Delay();
      IIC_SCL_HI(2);
      IIC_Delay();
      IIC_SDA_HI(2);
      break;
    case 3:
      IIC_Delay();
      IIC_SCL_HI(3);
      IIC_Delay();
      IIC_SDA_HI(3);
      break;
    case 4:
      IIC_Delay();
      IIC_SCL_HI(4);
      IIC_Delay();
      IIC_SDA_HI(4);
      break;
    case 5:
      IIC_Delay();
      IIC_SCL_HI(5);
      IIC_Delay();
      IIC_SDA_HI(5);
      break;
    default:
      break;
  }
}


void IIC_SCL_High(Uint8 channel)
{
  switch(channel)
  {
    case 1:
      IIC_SCL_HI(1);
      break;
    case 2:
      IIC_SCL_HI(2);
      break;
    case 3:
      IIC_SCL_HI(3);
      break;
    case 4:
      IIC_SCL_HI(4);
      break;
    case 5:
      IIC_SCL_HI(5);
      break;
    default:
      break;
  }
}
void IIC_SCL_Low(Uint8 channel)
{
  switch(channel)
  {
    case 1:
      IIC_SCL_LO(1);
      break;
    case 2:
      IIC_SCL_LO(2);
      break;
    case 3:
      IIC_SCL_LO(3);
      break;
    case 4:
      IIC_SCL_LO(4);
      break;
    case 5:
      IIC_SCL_LO(5);
      break;
    default:
      break;
  }
}

void IIC_SDA_High(Uint8 channel)
{
  switch(channel)
  {
    case 1:
      IIC_SDA_HI(1);
      break;
    case 2:
      IIC_SDA_HI(2);
      break;
    case 3:
      IIC_SDA_HI(3);
      break;
    case 4:
      IIC_SDA_HI(4);
      break;
    case 5:
      IIC_SDA_HI(5);
      break;
    default:
      break;
  }
}

void IIC_SDA_Low(Uint8 channel)
{
  switch(channel)
  {
    case 1:
      IIC_SDA_LO(1);
      break;
    case 2:
      IIC_SDA_LO(2);
      break;
    case 3:
      IIC_SDA_LO(3);
      break;
    case 4:
      IIC_SDA_LO(4);
      break;
    case 5:
      IIC_SDA_LO(5);
      break;
    default:
      break;
  }
}

void IIC_SDA_Out(Uint8 channel)
{
  switch(channel)
  {
    case 1:
      IIC_SDA_OUT(1);
      break;
    case 2:
      IIC_SDA_OUT(2);
      break;
    case 3:
      IIC_SDA_OUT(3);
      break;
    case 4:
      IIC_SDA_OUT(4);
      break;
    case 5:
      IIC_SDA_OUT(5);
      break;
    default:
      break;
  }
}

void IIC_SDA_In(Uint8 channel)
{
  switch(channel)
  {
    case 1:
      IIC_SDA_IN(1);
      break;
    case 2:
      IIC_SDA_IN(2);
      break;
    case 3:
      IIC_SDA_IN(3);
      break;
    case 4:
      IIC_SDA_IN(4);
      break;
    case 5:
      IIC_SDA_IN(5);
      break;
    default:
      break;
  }
}

BOOL IIC_ReadBit(Uint8 channel)
{
  BOOL NoACK;
  switch(channel)
  {
    case 1:
      if(IIC_PIN(1))
        NoACK = true;
      else
        NoACK = false;
      break;
    case 2:
      if(IIC_PIN(2))
        NoACK = true;
      else
        NoACK = false;
      break;
    case 3:
      if(IIC_PIN(3))
        NoACK = true;
      else
        NoACK = false;
      break;
    case 4:
      if(IIC_PIN(4))
        NoACK = true;
      else
        NoACK = false;
      break;
    case 5:
      if(IIC_PIN(5))
        NoACK = true;
      else
        NoACK = false;
      break;
    default:
      NoACK = true;
      break;
  }
  return NoACK;
}

BOOL IIC_WriteByte(Uint8 channel, Uint8 data)
{
  Uint8 i;
  BOOL NoAck_Flag;
  for(i = 0; i < 8; i++)
  {
    IIC_Delay();
    if((data&0x80) == 0)
    {
      IIC_SDA_Low(channel);
    }
    else
    {
      IIC_SDA_High(channel);
    }
    data <<= 1;
    IIC_Delay();
    IIC_SCL_High(channel);
    IIC_Delay();
    IIC_SCL_Low(channel);
    IIC_Delay();
  }
  IIC_SDA_In(channel);
  IIC_Delay();
  IIC_SCL_High(channel);
  IIC_Delay();

  if(IIC_ReadBit(channel))
  {
#if DebugVerbosity > 1
    UART_TxStr("IIC: No ACK\n\r");
#endif
    NoAck_Flag = true;
  }
  else
  {
    NoAck_Flag = false;
  }
  IIC_Delay();
  IIC_SCL_Low(channel);
  IIC_Delay();
  IIC_SDA_Out(channel);
  IIC_Delay();

  return NoAck_Flag;
}

