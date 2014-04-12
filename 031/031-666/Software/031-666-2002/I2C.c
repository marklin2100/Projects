/*****************************************************************************

    I2C_Multi module

    This module implements a bit bashed I2C interface

    David Annett, david@lynxinnovation.com, 15 February 2013

****************************************************************************/

//====================== Includes =============================================

#include <stdio.h>
#include "CommonTypes.h"
#include "Globals.h"
#include "I2C.h"
#include "Timer.h"


//====================== Constants ============================================

// TODO: Optimise I2CSPEED for fastest reliable transfers
//#define I2C_Tick() HalTimerWait(1)
#define I2CSPEED  50
void I2C_Tick() { volatile int v; int i; for (i=0; i < I2CSPEED/2; i++) v; }

// Ports for I2C, define in Globals.h e.g:
//#define I2C_DDR     DDRC
//#define I2C_PIN     PINC
//#define I2C_PORT    PORTC

// Pins to be used in the bit banging, define in Globals.h e.g:
//#define I2C_CLK 0
//#define I2C_DAT 1


#define I2C_DATA_HI() I2C_DDR &= ~(1 << I2C_DAT)
#define I2C_DATA_LO() I2C_DDR |= (1 << I2C_DAT)

#define I2C_CLOCK_HI() I2C_DDR &= ~(1 << I2C_CLK)
#define I2C_CLOCK_LO() I2C_DDR |= (1 << I2C_CLK)


//====================== Types ================================================


//====================== Vars =================================================

Uint8 I2C_Status;   // 0 = OK, else ErrorNoACK...


//====================== Functions ============================================

/*
  I2C_Init routine

  Initialises bit banging ports, must be called before using the other functions.
*/
void I2C_Init(void)
{
  I2C_Status = 0;                   // Assume device is present
  I2C_PORT &= ~((1 << I2C_DAT ) | (1 << I2C_CLK));

  I2C_CLOCK_HI();
  I2C_DATA_HI();

  I2C_Tick();
}



/*
  I2C_WriteBit routine

  This routine sends a single bit of data out an I2C bus.
*/
void I2C_WriteBit(Uint8 c)
{
  if (c > 0)
    I2C_DATA_HI();
  else
    I2C_DATA_LO();
  I2C_Tick();
  I2C_CLOCK_HI();
  I2C_Tick();
  I2C_CLOCK_LO();
  I2C_Tick();
  if (c > 0)
    I2C_DATA_LO();
  I2C_Tick();
}



/*
  I2C_ReadBit routine

  This routine reads a bit from a I2C bus.
*/
Uint8 I2C_ReadBit(void)
{
  Uint8 c;


  I2C_DATA_HI();
  I2C_Tick();
  I2C_CLOCK_HI();
  I2C_Tick();
  c = I2C_PIN;
  I2C_CLOCK_LO();
  I2C_Tick();
  return (c >> I2C_DAT) & 1;
}



/*
  I2C_Start routine

  Send a START Condition
*/
void I2C_Start(void)
{
  I2C_Status = 0;                   // Assume device is present

  // set both to high at the same time

  I2C_DATA_LO();
  I2C_Tick();
  I2C_CLOCK_LO();
  I2C_Tick();
}



/*
  I2C_Stop routine

  Send a STOP Condition
*/
void I2C_Stop(void)
{
  I2C_CLOCK_HI();
  I2C_Tick();
  I2C_DATA_HI();
  I2C_Tick();
}



/*
  I2C_Error routine

  This routine handles an error on an I2C bus.
*/
void I2C_Error(Uint8 ErrNum)
{
  I2C_Tick();
  I2C_Stop();
  I2C_Status = ErrNum;
}



// write a byte to the I2C slave device
//
Uint8 I2C_Write(Uint8 c)
{
  for (char i=0; i<8; i++) {
    I2C_WriteBit(c & 128);
    c<<=1;
  }

  return I2C_ReadBit();
}



// read a byte from the I2C slave device
//
Uint8 I2C_Read(Uint8 ack)
{
  Uint8 res = 0;


  for (char i=0; i<8; i++) {
    res <<= 1;
    res |= I2C_ReadBit();
  }

  if (ack > 0)
    I2C_WriteBit(0);
  else
    I2C_WriteBit(1);

  I2C_Tick();

  return res;
}

//========== Byte addressed EEPROMS i.e. 2Kb/256B or smaller ==================

/*
  I2C_EESReadByte routine

  This routine reads a byte from EEPROM memory on a I2C bus.
*/
Uint8 I2C_EESReadByte(Uint8 i2c_addr, Uint8 mem_addr)
{
  Uint8 Result;


  I2C_Start();
  if (I2C_Write(i2c_addr & 0xFE)) { // Send I2C address as write
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceAddress);
    return 0;
  }

  if (I2C_Write(mem_addr)) {        // Send memory address
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceReg);
    return 0;
  }

// Repeat start for read

  I2C_CLOCK_HI();
  I2C_Tick();
  I2C_Start();
  if (I2C_Write(i2c_addr | 0x01)) { // Send I2C address as read
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceAddress);
    return 0;
  }
  Result = I2C_Read(0);             // Read the register contents
  I2C_DATA_LO();
  I2C_Tick();
  I2C_Stop();
  return Result;
}



/*
  I2C_EESReadBlock routine

  This routine reads a block from EEPROM memory on a I2C bus.
*/
bool I2C_EESReadBlock(Uint8 i2c_addr, Uint8 mem_addr, Uint8 *p_data, Uint16 nbytes)
{
  Uint16  Pos;


  I2C_Start();
  if (I2C_Write(i2c_addr & 0xFE)) {   // Send I2C address as write
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceAddress);
    return false;
  }

  if (I2C_Write(mem_addr)) {          // Send memory address
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceReg);
    return false;
  }

// Repeat start for read

  I2C_CLOCK_HI();
  I2C_Tick();
  I2C_Start();
  if (I2C_Write(i2c_addr | 0x01)) {   // Send I2C address as read
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceAddress);
    return false;
  }

  for(Pos = 0; Pos < nbytes; Pos++) {
    p_data[Pos] = I2C_Read(Pos != (nbytes - 1));  // Read the memory contents
  }

  I2C_DATA_LO();
  I2C_Tick();
  I2C_Stop();

  return true;
}



/*
  I2C_EESWriteByte routine

  This routine writes a byte to EEPROM memory on a I2C bus.
*/
void I2C_EESWriteByte(Uint8 i2c_addr, Uint8 mem_addr, Uint8 value)
{
  I2C_Start();
  if (I2C_Write(i2c_addr & 0xFE)) { // Send device address as write
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceAddress);
    return;
  }

  if (I2C_Write(mem_addr)) {        // Send memory address
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceReg);
    return;
  }

  if (I2C_Write(value)) {           // Send data
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceWrite);
    return;
  }

  I2C_DATA_LO();
  I2C_Tick();
  I2C_Stop();
  DelayMS(6);  // Allow time for actual write to occur
}



/*
  I2C_EESWriteBlock routine

  This routine writes a block to EEPROM memory on a I2C bus.
*/
bool I2C_EESWriteBlock(Uint8 i2c_addr, Uint8 mem_addr, Uint8 *p_data, Uint16 nbytes)
{
  Uint16 Pos;


  while(nbytes > 8) {
    I2C_EESWriteBlock(i2c_addr, mem_addr, p_data, 8);
    mem_addr += 8;
    p_data = &p_data[8];
    nbytes -=8;
  }

  I2C_Start();
  if (I2C_Write(i2c_addr & 0xFE)) {   // Send device address as write
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceAddress);
    return false;
  }

  if (I2C_Write(mem_addr)) {          // Send memory address high byte
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceReg);
    return false;
  }

  for(Pos = 0; Pos < nbytes; Pos++) {
    if (I2C_Write(p_data[Pos])) {     // Send data
      I2C_DATA_LO();
      I2C_Error(ErrorNoACKFromDeviceWrite);
      return false;
    }
  }

  I2C_DATA_LO();
  I2C_Tick();
  I2C_Stop();
  DelayMS(6);  // Allow time for actual write to occur
  return true;
}



//========== Word addressed EEPROMS i.e. 4Kb/512B or larger ===================

/*
  I2C_EEReadByte routine

  This routine reads a byte from EEPROM memory on a I2C bus.
*/
Uint8 I2C_EEReadByte(Uint8 i2c_addr, Uint16 mem_addr)
{
  Uint8 Result;
  Uint8 Hi;
  Uint8 Low;


  I2C_Start();
  if (I2C_Write(i2c_addr & 0xFE)) { // Send I2C address as write
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceAddress);
    return 0;
  }

  Hi = (Uint8)(mem_addr / 0x100);
  Low = (Uint8)mem_addr & 0xFF;
  if (I2C_Write(Hi)) { // Send memory address high byte
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceReg);
    return 0;
  }
  if (I2C_Write(Low)) { // Send memory address low byte
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceReg);
    return 0;
  }

// Repeat start for read

  I2C_CLOCK_HI();
  I2C_Tick();
  I2C_Start();
  if (I2C_Write(i2c_addr | 0x01)) { // Send I2C address as read
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceAddress);
    return 0;
  }
  Result = I2C_Read(0);          // Read the register contents
  I2C_DATA_LO();
  I2C_Tick();
  I2C_Stop();
  return Result;
}



/*
  I2C_EEReadBlock routine

  This routine reads a block from EEPROM memory on a I2C bus.
*/
bool I2C_EEReadBlock(Uint8 i2c_addr, Uint16 mem_addr, Uint8 *p_data, Uint16 nbytes)
{
  Uint16  Pos;
  Uint8   Hi;
  Uint8   Low;


  I2C_Start();
  if (I2C_Write(i2c_addr & 0xFE)) { // Send I2C address as write
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceAddress);
    return false;
  }

  Hi = (Uint8)(mem_addr / 0x100);
  Low = (Uint8)mem_addr & 0xFF;
  if (I2C_Write(Hi)) { // Send memory address high byte
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceReg);
    return false;
  }
  if (I2C_Write(Low)) { // Send memory address low byte
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceReg);
    return false;
  }

// Repeat start for read

  I2C_CLOCK_HI();
  I2C_Tick();
  I2C_Start();
  if (I2C_Write(i2c_addr | 0x01)) { // Send I2C address as read
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceAddress);
    return false;
  }

  for(Pos = 0; Pos < nbytes; Pos++) {
    p_data[Pos] = I2C_Read(Pos != (nbytes - 1));          // Read the memory contents
  }

  I2C_DATA_LO();
  I2C_Tick();
  I2C_Stop();

  return true;
}



/*
  I2C_EEWriteByte routine

  This routine writes a byte to EEPROM memory on a I2C bus.
*/
void I2C_EEWriteByte(Uint8 i2c_addr, Uint16 mem_addr, Uint8 value)
{
  Uint8   Hi;
  Uint8   Low;


  I2C_Start();
  if (I2C_Write(i2c_addr & 0xFE)) { // Send device address as write
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceAddress);
    return;
  }

  Hi = (Uint8)(mem_addr / 0x100);
  Low = (Uint8)mem_addr & 0xFF;
  if (I2C_Write(Hi)) { // Send memory address high byte
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceReg);
    return;
  }
  if (I2C_Write(Low)) { // Send memory address low byte
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceReg);
    return;
  }

  if (I2C_Write(value)) { // Send data
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceWrite);
    return;
  }

  I2C_DATA_LO();
  I2C_Tick();
  I2C_Stop();
  DelayMS(6);  // Allow time for actual write to occur
}



/*
  I2C_EEWriteBlock routine

  This routine writes a block to EEPROM memory on a I2C bus.
*/
bool I2C_EEWriteBlock(Uint8 i2c_addr, Uint16 mem_addr, Uint8 *p_data, Uint16 nbytes)
{
  Uint16 Pos;
  Uint8  Hi;
  Uint8  Low;


  while(nbytes > 64) {
    I2C_EEWriteBlock(i2c_addr, mem_addr, p_data, 64);
    mem_addr += 64;
    p_data = &p_data[64];
    nbytes -=64;
  }
  I2C_Start();
  if (I2C_Write(i2c_addr & 0xFE)) { // Send device address as write
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceAddress);
    return false;
  }

  Hi = (Uint8)(mem_addr / 0x100);
  Low = (Uint8)mem_addr & 0xFF;
  if (I2C_Write(Hi)) { // Send memory address high byte
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceReg);
    return false;
  }
  if (I2C_Write(Low)) { // Send memory address low byte
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceReg);
    return false;
  }

  for(Pos = 0; Pos < nbytes; Pos++) {
    if (I2C_Write(p_data[Pos])) { // Send data
      I2C_DATA_LO();
      I2C_Error(ErrorNoACKFromDeviceWrite);
      return false;
    }
  }

  I2C_DATA_LO();
  I2C_Tick();
  I2C_Stop();
  DelayMS(6);  // Allow time for actual write to occur
  return true;
}


//========== Silicon Image Instaport chip =====================================

/*
  I2C_ReadByte routine

  This routine reads a byte, Silicon Image style, from a device on a I2C bus.
  The Silicon Image is documentation is lacking but it assumed the byte is
  actually the register contents.  Parameters are assumed to be as follows:
    device_id:  The I2C address in bits 7 to 1, bit 0 ignored.
    addr:       The register address within the device.
*/
/*
Uint8 I2C_ReadByte(Uint8 device_id, Uint8 addr )
{
  Uint8 Result;


  I2C_Start();
  if (I2C_Write(device_id & 0xFE)) { // Send device address as write
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceAddress);
    return 0;
  }
  if (I2C_Write(addr)) { // Send register address
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceReg);
    return 0;
  }

// Repeat start for read

  I2C_CLOCK_HI();
  I2C_Tick();
  I2C_Start();
  if (I2C_Write(device_id | 0x01)) { // Send device address as read
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceAddress);
    return 0;
  }
  Result = I2C_Read(0);          // Read the register contents
  I2C_DATA_LO();
  I2C_Tick();
  I2C_Stop();
  return Result;
}
*/


/*
  I2C_ReadBlock routine

  This routine reads a byte, Silicon Image style, from a device on a I2C bus.
*/
/*
bool I2C_ReadBlock(Uint8 device_id, Uint8 addr, Uint8 *p_data, Uint16 nbytes)
{
  Uint16 Pos;
  Uint8  TempByte;


  I2C_Start();
  if (I2C_Write(device_id & 0xFE)) { // Send device address as write
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceAddress);
    return 0;
  }
  if (I2C_Write(addr)) { // Send register/memory address
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceReg);
    return 0;
  }

  // Repeat start for read

  I2C_CLOCK_HI();
  I2C_Tick();
  I2C_Start();
  if (I2C_Write(device_id | 0x01)) { // Send I2C address as read
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceAddress);
    return false;
  }

  for(Pos = 0; Pos < nbytes; Pos++) {
    TempByte = I2C_Read(Pos != (nbytes - 1));          // Read the memory contents
    p_data[Pos] = TempByte;
  }

  I2C_DATA_LO();
  I2C_Tick();
  I2C_Stop();
  return true;
}
*/


/*
  I2C_WriteByte routine

  This routine write a byte, Silicon Image style, to a device on the I2C bus.
*/
/*
void I2C_WriteByte(Uint8 device_id, Uint8 offset, Uint8 value)
{
  I2C_Start();
  if (I2C_Write(device_id & 0xFE)) { // Send device address as write
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceAddress);
    return;
  }

  if (I2C_Write(offset)) { // Send register address
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceReg);
    return;
  }

  if (I2C_Write(value)) { // Send data
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceWrite);
    return;
  }

  I2C_DATA_LO();
  I2C_Tick();
  I2C_Stop();
}
*/


/*
  I2C_WriteBlock routine

  This routine write a byte, Silicon Image style, to a device on the I2C bus.
*/
/*
bool I2C_WriteBlock(Uint8 device_id, Uint8 addr, Uint8 *p_data, Uint16 nbytes)
{
  Uint16 Pos;


  I2C_Start();
  if (I2C_Write(device_id & 0xFE)) { // Send device address as write
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceAddress);
    return false;
  }

  if (I2C_Write(addr)) { // Send register address
    I2C_DATA_LO();
    I2C_Error(ErrorNoACKFromDeviceReg);
    return false;
  }

  for(Pos = 0; Pos < nbytes; Pos++) {
    if (I2C_Write(p_data[Pos])) { // Send data
      I2C_DATA_LO();
      I2C_Error(ErrorNoACKFromDeviceWrite);
      return false;
    }
  }

  I2C_DATA_LO();
  I2C_Tick();
  I2C_Stop();
  return true;
}
*/
