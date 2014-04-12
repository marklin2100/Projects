#ifndef _IICIO_H
#define _IICIO_H

#include "Globals.h"

//#define IICIO_SDA PORTC.4
//#define IICIO_SCL PORTC.5

//#define IICIO_SDA_DIR DDRC.4
//#define IICIO_SCL_DIR DDRC.5

//#define IICIO_SDA_PIN PINC.4

#define I2CSDA    PC1
#define I2CSCL    PC0
#define I2CPort   PORTC
#define I2CPin    PINC
#define I2CDDR    DDRC

//#define IICIO_SDA_DIR_OUT() (IICIO_SDA_DIR=1);
#define IICIO_SDA_DIR_OUT() (I2CDDR |= (1 << I2CSDA))
//#define IICIO_SDA_DIR_IN() (IICIO_SDA_DIR=0);
#define IICIO_SDA_DIR_IN() (I2CDDR &= ~(1 << I2CSDA))

//#define IICIO_SCL_DIR_OUT() (IICIO_SCL_DIR=1);
#define IICIO_SCL_DIR_OUT() (I2CDDR |= (1 << I2CSCL))
//#define IICIO_SCL_DIR_IN() (IICIO_SCL_DIR=0);
#define IICIO_SCL_DIR_IN() (I2CDDR &= ~(1 << I2CSCL))

//#define IICIO_SDA_SET()  (IICIO_SDA=1);
#define IICIO_SDA_SET()  (I2CPort |= (1 << I2CSDA))
//#define IICIO_SDA_CLR()  (IICIO_SDA=0);
#define IICIO_SDA_CLR()  (I2CPort &= ~(1 << I2CSDA))


//#define IICIO_SCL_SET()  (IICIO_SCL=1);
#define IICIO_SCL_SET()  (I2CPort |= (1 << I2CSCL))
//#define IICIO_SCL_CLR()  (IICIO_SCL=0);
#define IICIO_SCL_CLR()  (I2CPort &= ~(1 << I2CSCL))

extern bool SlaveAck_Flag;
extern unsigned char IIC_Status;

extern void IICIO_Init(void);
extern void IICIO_Start(void);
extern void IICIO_Stop(void);
extern void IICIO_WriteByte(unsigned char wd);
extern unsigned char IICIO_ReadByte(void);
extern void IICIO_WriteData(unsigned char devadd,unsigned char regadd,unsigned char wd);
extern unsigned char IICIO_ReadData(unsigned char devadd,unsigned char regadd);

#endif
