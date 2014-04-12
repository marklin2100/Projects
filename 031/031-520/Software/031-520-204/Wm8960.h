/*#ifndef _wm8960_H
#define _wm8960_H

#include "CommonTypes.h"

#define WM8960_ADD               0x34

#define LEFT_INPUT_VOLUME_ADD    0x00
#define RIGHT_INPUT_VOLUME_ADD   0x01
#define LOUT1_VOLUME_ADD         0X02
#define ROUT1_VOLUME_ADD         0X03
#define CLOCKING1_ADD            0x04
#define ADC_DAC_CONTROL_1_ADD    0X05
#define ADC_DAC_CONTROL_2_ADD    0X06
#define AUDIO_INTERFACE_ADD      0x07
#define CLOCKING2_ADD            0x08
#define AIDIO_INTERFACE2_ADD     0x09
#define LEFT_DAC_VOLUME_ADD      0x10
#define RIGHT_DAC_VOLUME_ADD     0x11
#define RESET_ADD                0X0F
#define THREE_D_CONTROL_ADD      0x10
#define ALC1_ADD                 0x11
#define ALC2_ADD                 0x12
#define ALC3_ADD                 0x13
#define NOISE_GATE_ADD           0x14
#define LEFT_ADC_VOLUME_ADD      0x15
#define RIGHT_ADC_VOLUME_ADD     0x16
#define ADDITIONAL_CONTROL1_ADD  0x17
#define ADDITIONAL_CONTROL2_ADD  0x18
#define POWER_MGMT1_ADD          0x19
#define POWER_MGMT2_ADD          0x1A
#define POWER_MGMT3_ADD          0x1B
#define ANTI_POP1_ADD            0x1C
#define ANTI_POP2_ADD            0x1D
#define ADCL_SIGNAL_PATH_ADD     0x20
#define ADCR_SIGNAL_PATH_ADD     0x21
#define LEFT_OUT_MIX_ADD         0x22
#define RIGHT_OUT_MIX_ADD        0x25
#define MONO_OUT_MIX1_ADD        0x26
#define MONO_OUT_MIX2_ADD        0x27
#define LEFT_SPEAKER_VOLUME_ADD  0x28
#define RIGHT_SPEAKER_VOLUME_ADD 0x29
#define OUT3_VOLUME_ADD          0x2A
#define LEFT_INPUT_BOOST_MIXER_ADD 0x2B
#define RIGHT_INPUT_BOOST_MIXER_ADD 0x2C
#define LEFT_BYPASS_ADD          0x2D
#define RIGHT_BYPASS_ADD         0x2E
#define POWER_MGMT_ADD           0x2F
#define ADDITIONAL_CONTROL4_ADD  0x30
#define CLASS_D_CONTROL_ADD      0x31
#define CLASS_D_CONTROL2_ADD     0x33
#define PLL1_ADD                 0x34
#define PLL2_ADD                 0x35
#define PLL3_ADD                 0x36
#define PLL4_ADD                 0x37

//LOUT1_VOLUME_ADD  ROUT1_VOLUME_ADD
#define OUT1VU                   1 << 8
#define LO1ZC                    1 << 7
Uint8 Lout1vol,Rout1vol;
//CLOCKING1_ADD
Uint8 DACDIV,SYSDIV; //BIT 5  3
#define CLKSEL                   1

//ADC_DAC_CONTROL_1_ADD
#define DACMU                    1 << 3


void WM8960_Init(void);
void WM8960_WriteByte(Uint8 wd);
void WM8960_WriteData(Uint8 Register,Uint16 data);
#endif*/

/*
 * wm8960.h  --  WM8960 Soc Audio driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _WM8960_H
#define _WM8960_H

#include "CommonTypes.h"
/* WM8960 register space */


#define WM8960_Write_ADD   0x34

#define WM8960_LINVOL     0x0
#define WM8960_RINVOL     0x1
#define WM8960_LOUT1      0x2
#define WM8960_ROUT1      0x3
#define WM8960_CLOCK1     0x4
#define WM8960_DACCTL1    0x5
#define WM8960_DACCTL2    0x6
#define WM8960_IFACE1     0x7
#define WM8960_CLOCK2     0x8
#define WM8960_IFACE2     0x9
#define WM8960_LDAC       0xa
#define WM8960_RDAC       0xb

#define WM8960_RESET      0xf
#define WM8960_3D         0x10
#define WM8960_ALC1       0x11
#define WM8960_ALC2       0x12
#define WM8960_ALC3       0x13
#define WM8960_NOISEG     0x14
#define WM8960_LADC       0x15
#define WM8960_RADC       0x16
#define WM8960_ADDCTL1    0x17
#define WM8960_ADDCTL2    0x18
#define WM8960_POWER1     0x19
#define WM8960_POWER2     0x1a
#define WM8960_ADDCTL3    0x1b
#define WM8960_APOP1      0x1c
#define WM8960_APOP2      0x1d

#define WM8960_LINPATH    0x20
#define WM8960_RINPATH    0x21
#define WM8960_LOUTMIX    0x22

#define WM8960_ROUTMIX    0x25
#define WM8960_MONOMIX1   0x26
#define WM8960_MONOMIX2   0x27
#define WM8960_LOUT2      0x28
#define WM8960_ROUT2      0x29
#define WM8960_MONO       0x2a
#define WM8960_INBMIX1    0x2b
#define WM8960_INBMIX2    0x2c
#define WM8960_BYPASS1    0x2d
#define WM8960_BYPASS2    0x2e
#define WM8960_POWER3     0x2f
#define WM8960_ADDCTL4    0x30
#define WM8960_CLASSD1    0x31

#define WM8960_CLASSD3    0x33
#define WM8960_PLL1       0x34
#define WM8960_PLL2       0x35
#define WM8960_PLL3       0x36
#define WM8960_PLL4       0x37


/*
 * WM8960 Clock dividers
 */
#define WM8960_SYSCLKDIV      0
#define WM8960_DACDIV         1
#define WM8960_OPCLKDIV       2
#define WM8960_DCLKDIV        3
#define WM8960_TOCLKSEL       4

#define WM8960_SYSCLK_DIV_1   (0 << 1)
#define WM8960_SYSCLK_DIV_2   (2 << 1)

#define WM8960_SYSCLK_MCLK    (0 << 0)
#define WM8960_SYSCLK_PLL     (1 << 0)

#define WM8960_DAC_DIV_1      (0 << 3)
#define WM8960_DAC_DIV_1_5    (1 << 3)
#define WM8960_DAC_DIV_2      (2 << 3)
#define WM8960_DAC_DIV_3      (3 << 3)
#define WM8960_DAC_DIV_4      (4 << 3)
#define WM8960_DAC_DIV_5_5    (5 << 3)
#define WM8960_DAC_DIV_6      (6 << 3)

#define WM8960_DCLK_DIV_1_5   (0 << 6)
#define WM8960_DCLK_DIV_2     (1 << 6)
#define WM8960_DCLK_DIV_3     (2 << 6)
#define WM8960_DCLK_DIV_4     (3 << 6)
#define WM8960_DCLK_DIV_6     (4 << 6)
#define WM8960_DCLK_DIV_8     (5 << 6)
#define WM8960_DCLK_DIV_12    (6 << 6)
#define WM8960_DCLK_DIV_16    (7 << 6)

#define WM8960_TOCLK_F19      (0 << 1)
#define WM8960_TOCLK_F21      (1 << 1)

#define WM8960_OPCLK_DIV_1    (0 << 0)
#define WM8960_OPCLK_DIV_2    (1 << 0)
#define WM8960_OPCLK_DIV_3    (2 << 0)
#define WM8960_OPCLK_DIV_4    (3 << 0)
#define WM8960_OPCLK_DIV_5_5  (4 << 0)
#define WM8960_OPCLK_DIV_6    (5 << 0)

//R47
#define WM8960_LOMIX          (1 << 3)
#define WM8960_ROMIX          (1 << 4)

//R34
#define WM8960_LD2LO          (1 << 8)
#define WM8960_LI2LO          (1 << 7)

//R45 Bypass
#define WM8960_LB2LO          (1 << 7)
 //
#define WM8960_RD2RO          (1 << 8)
#define WM8960_RI2RO          (1 << 7)

#define WM8960_RB2RO          (1 << 8)

#define ANLOG            1
#define DIGITAL          2


//void WM8960_PathSelect(Uint8 Path);
void WM8960_Init(void);
void WM8960_WriteData(Uint8 channel, Uint8 Register, Uint16 Data);
void WM8960_SetVolume(Uint8 channel, Uint8 volume);
void WM8960Test(void);
void WM8960_SetAudioFormat(Uint8 AudioFormat);

#endif
