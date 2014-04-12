#include "CommonTypes.h"
//#include"io.h"
#include "iicio.h"
#include "Globals.h"
//#include"001-032-201.h"

void IICDelay(void)
{
  volatile int i = 50;//
  while(i--)
  {
    ;
  }
}


bool SlaveAck_Flag;
unsigned char IIC_Status;

void IICIO_Init(void)
{
  IICIO_SDA_DIR_OUT();
  IICIO_SCL_DIR_OUT();
  //DDRC |= 0b00110000;

  IICDelay();
  IICIO_SDA_SET();
  IICDelay();
  IICIO_SCL_SET();
  IICDelay();
}


void IICIO_Start(void)
{

  IICIO_SDA_SET();
  IICIO_SCL_SET();
  IICDelay();
  IICIO_SDA_CLR();
  IICDelay();
  IICIO_SCL_CLR();
  IICDelay();
}

void IICIO_Stop(void)
{
  IICIO_SCL_SET();
  IICDelay();
  IICIO_SDA_SET();
  IICDelay();
}


void IICIO_WriteByte(unsigned char wd)
{
  unsigned char n;
  for(n=0;n<8;n++)
  {
    if((wd&0x80)==0)
    {
      IICIO_SDA_CLR();
    }
    else
    {
      IICIO_SDA_SET();
    }
    wd<<=1;
    IICDelay();
    IICIO_SCL_SET();
    IICDelay();
    IICIO_SCL_CLR();
    IICDelay();
  }

  IICIO_SDA_DIR_IN();
  IICDelay();
  IICIO_SCL_SET();
  IICDelay();
//  if(IICIO_SDA_PIN==0)
  if((I2CPin & (1 << I2CSDA)) == 0){
    SlaveAck_Flag=0;
    //UARTTxStr("Pcm9211 I2C_DummyWrite: ok\n\r");
  }
  //if((PINC & 0b00010000) == 0)
  else{
    SlaveAck_Flag=1;
    //UART_TxStr("Pcm9211 I2C_DummyWrite: no ack\n\r");
  }
  IICIO_SCL_CLR();
  IICDelay();
  IICIO_SDA_DIR_OUT();
  IICDelay();
}


unsigned char IICIO_ReadByte(void)
{
  unsigned char n;
  unsigned char rd;

  rd = 0;
  IICIO_SDA_DIR_IN();
  for(n=0;n<8;n++)
  {
    IICDelay();
    IICIO_SCL_SET();

    rd<<=1;

//    if(IICIO_SDA_PIN==0)
    if((I2CPin & (1 << I2CSDA)) == 0)
    //if((PINC & 0b00010000) == 0)
      rd&=0xfe;
    else
      rd|=1;

    IICDelay();
    IICIO_SCL_CLR();
    IICDelay();

  }

  IICIO_SDA_DIR_OUT();
  IICIO_SDA_SET();
  IICDelay();
  IICIO_SCL_SET();
  IICDelay();
  IICIO_SCL_CLR();
  IICDelay();
  return rd;
}


void IICIO_WriteData(unsigned char devadd,unsigned char regadd,unsigned char wd)
{
  IIC_Status=0;
  IICIO_Start();
  IICDelay();
  //if(SlaveAck_Flag==0)
  //{
  IICIO_WriteByte(devadd&0xfe);
  if(SlaveAck_Flag==0)
  {
    IICIO_WriteByte(regadd);
    if(SlaveAck_Flag==0)
    {
      IICIO_WriteByte(wd);
     // UART_TxStr("Pcm9211 Address = ");
     // UART_TxUint16(regadd);
     // UART_TxStr(" Value = ");
     // UART_TxUint16(wd);
     // UART_TxStr("\n\r");
      if(SlaveAck_Flag==1)
      {
        IIC_Status=3;
      }
    }
    else
    {
      IIC_Status=2;
    }
  }
  else
  {
    IIC_Status=1;
  }
  //}
  //else
  //{
  //rstatus=1;
  //}
  IICDelay();
  IICIO_Stop();
  //return IIC_Status;
}


unsigned char IICIO_ReadData(unsigned char devadd,unsigned char regadd)
{
  unsigned char rdata=0;
  IICIO_Start();
  IICDelay();
  //if(SlaveAck_Flag==0)
  //{
  IICIO_WriteByte(devadd&0xfe);
  if(SlaveAck_Flag==0)
  {
    IICIO_WriteByte(regadd);
    if(SlaveAck_Flag==0)
    {
      IICIO_Start();
      IICDelay();
      IICIO_WriteByte(devadd|0x1);
      if(SlaveAck_Flag==0)
      {
        rdata=IICIO_ReadByte();
      }
      else
      {
        IIC_Status=3;
      }
    }
    else
    {
      IIC_Status=2;
    }
  }
  else
  {
    IIC_Status=1;
  }
  //}
  //else
  //{
  //rstatus=1;
  //}
  IICDelay();
  IICIO_Stop();
  return rdata;
}
