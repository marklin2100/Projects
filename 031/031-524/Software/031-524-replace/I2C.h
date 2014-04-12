#ifndef i2c_multi_h
#define i2c_multi_h
/*****************************************************************************

    I2C_Multi module

    This module implements a bit bashed I2C interface

    David Annett, david@lynxinnovation.com, 15 February 2013

****************************************************************************/

//====================== Includes =============================================

#include "CommonTypes.h"

//====================== Constants ============================================

#define ErrorNoACKFromDeviceReg     1
#define ErrorNoACKFromDeviceAddress 2
#define ErrorNoACKFromDeviceWrite   3


//====================== Types ================================================


//====================== Vars =================================================

extern Uint8 I2C_Status;    // 0 = OK, else ErrorNoACK...


//====================== Functions ============================================

/*
  I2C_Init routine

  Inits bit banging port, must be called before using the other functions.
*/
void I2C_Init(void);



//========== Byte addressed EEPROMS i.e. 2Kb/256B or smaller ==================

/*
  I2C_EESReadByte routine

  This routine reads a byte from EEPROM memory on a I2C bus.
*/
Uint8 I2C_EESReadByte(Uint8 i2c_addr, Uint8 mem_addr);


/*
  I2C_EESReadBlock routine

  This routine reads a block from EEPROM memory on a I2C bus.
*/
bool I2C_EESReadBlock(Uint8 i2c_addr, Uint8 mem_addr, Uint8 *p_data, Uint16 nbytes);


/*
  I2C_EESWriteByte routine

  This routine writes a byte to EEPROM memory on a I2C bus.
*/
void I2C_EESWriteByte(Uint8 i2c_addr, Uint8 mem_addr, Uint8 value);


/*
  I2C_EESWriteBlock routine

  This routine writes a block to EEPROM memory on a I2C bus.
*/
bool I2C_EESWriteBlock(Uint8 i2c_addr, Uint8 mem_addr, Uint8 *p_data, Uint16 nbytes);


//========== Word addressed EEPROMS i.e. 4Kb/512B or larger ===================

/*
  I2C_EEReadByte routine

  This routine reads a byte from EEPROM memory on a I2C bus.
*/
Uint8 I2C_EEReadByte(Uint8 i2c_addr, Uint16 mem_addr);


/*
  I2C_EEReadBlock routine

  This routine reads a block from EEPROM memory on a I2C bus.
*/
bool I2C_EEReadBlock(Uint8 i2c_addr, Uint16 mem_addr, Uint8 *p_data, Uint16 nbytes);


/*
  I2C_EEWriteByte routine

  This routine writes a byte to EEPROM memory on a I2C bus.
*/
void I2C_EEWriteByte(Uint8 i2c_addr, Uint16 mem_addr, Uint8 value);


/*
  I2C_EEWriteBlock routine

  This routine writes a block to EEPROM memory on a I2C bus.
*/
bool I2C_EEWriteBlock(Uint8 i2c_addr, Uint16 mem_addr, Uint8 *p_data, Uint16 nbytes);


//========== Silicon Image Instaport chip =====================================

/*
  I2C_ReadByte routine

  This routine reads a byte, Silicon Image style, from a device on a I2C bus.
  The Silicon Image is documentation is lacking but it assumed the byte is
  actually the register contents.  Parameters are assumed to be as follows:
    device_id:  The I2C address in bits 7 to 1, bit 0 ignored.
    addr:       The register address within the device.
*/
//Uint8 I2C_ReadByte(Uint8 device_id, Uint8 addr);


/*
  I2C_ReadBlock routine

  This routine reads a byte, Silicon Image style, from a device on a I2C bus.
*/
//bool I2C_ReadBlock(Uint8 device_id, Uint8 addr, Uint8 *p_data, Uint16 nbytes);


/*
  I2C_WriteByte routine

  This routine write a byte, Silicon Image style, to a device on the I2C bus.
*/
//void I2C_WriteByte(Uint8 device_id, Uint8 offset, Uint8 value);


/*
  I2C_WriteBlock routine

  This routine write a byte, Silicon Image style, to a device on the I2C bus.
*/
//bool I2C_WriteBlock(Uint8 device_id, Uint8 addr, Uint8 *p_data, Uint16 nbytes);

#endif
