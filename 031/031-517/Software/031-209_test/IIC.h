#ifndef _IIC_H
#define _IIC_H

#include "Globals.h"


#define IIC_SCL_OUT(x)         ((IIC_DDR##x)   |= (1 << IIC_SCL##x))
#define IIC_SDA_OUT(x)         ((IIC_DDR##x)   |= (1 << IIC_SDA##x))

#define IIC_SDA_IN(x)          ((IIC_DDR##x)   &= (~(1 << IIC_SDA##x)))

#define IIC_PIN(x)             ((IIC_Pin##x)   &  (1 << IIC_SDA##x))
#define IIC_SCL_HI(x)          ((IIC_Port##x)  |= (1 << IIC_SCL##x))
#define IIC_SCL_LO(x)          ((IIC_Port##x)  &= (~(1 << IIC_SCL##x)))
#define IIC_SDA_HI(x)          ((IIC_Port##x)  |= (1 << IIC_SDA##x))
#define IIC_SDA_LO(x)          ((IIC_Port##x)  &= (~(1<< IIC_SDA##x)))


#define IIC_PLL_UP(x)          ((IIC_Pin##x) | (1 << IIC_SDA##x)); \
                               ((IIC_Port##x) | (1 << IIC_SDA##x))

void IIC_Delay(void);
void IIC_Init(void);
void IIC_Start(Uint8 channel);
void IIC_Stop(Uint8 channel);
BOOL IIC_ReadBit(Uint8 channel);
BOOL IIC_WriteByte(Uint8 channel, Uint8 data);

#endif
