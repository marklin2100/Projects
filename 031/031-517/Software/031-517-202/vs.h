#ifndef __VS_H__
#define __VS_H__

#include <stdlib.h>
#include <stdint.h>

#define VS_CMD_WRITE 0x02
#define VS_CMD_READ 0x03

#define VS_XCS	PA3
#define VS_XDCS PA2
#define VS_DREQ PA1
#define VS_DDR  DDRA
#define VS_PORT PORTA
#define VS_PIN  PINA

#define vs_xcs_high()	VS_PORT |= (1 << VS_XCS)
#define vs_xcs_low()	VS_PORT &= ~(1 << VS_XCS)
#define vs_xdcs_high()	VS_PORT |= (1 << VS_XDCS)
#define vs_xdcs_low()	VS_PORT &= ~(1 << VS_XDCS)

#define SCI_MODE 0x0
#define SCI_STATUS 0x1
#define SCI_BASS 0x2
#define SCI_CLOCKF 0x3
#define SCI_DECODETIME 0x4
#define SCI_AUDATA 0x5
#define SCI_WRAM 0x06
#define SCI_WRAMADDR 0x07
#define SCI_HDAT0 0x8
#define SCI_HDAT1 0x9
#define SCI_AIADDR 0xa
#define SCI_VOL 0xb

void vs_init();
void vs_reset_soft();
void vs_wait();
uint16_t vs_sci_read(uint8_t addr);
void vs_sci_write(uint8_t addr, uint16_t value);
void vs_sdi_write(uint8_t *buf, size_t bufsz);
void vs_cancel();
int vs_finish(); /* call this after the track is over */

/* 0x00 - maximum, 0xfe - minimum, 0xff - mute */
void vs_set_volume(uint8_t left, uint8_t right);

/* 
 * treble: treble control in 1.5 dB steps, -8..7,0=off 
 * treble_freq: lower treble frequency in 1000Hz steps (1..15)
 * bass: bass control in 1dB steps, 0..15, 0=off
 * bass_freq: lower bass frequency in 10Hz steps (2..15)
 *
 * E.g.:
 *   0x00f6 = 15 dB enhancement below 60 Hz
 *   0x7a00 = 10.5 dB enhancement above 10kHz
 */
void vs_set_bass(uint8_t treble, uint8_t bass, 
		uint8_t treble_freq, uint8_t bass_freq);

void vs_set_i2s();

uint16_t vs_test_memory();
void vs_test_sine_on();
void vs_test_sine_off();
#ifdef VS10XX_FLAC
void	vs_load_plugin_flac();
#endif

#endif /* __VS_H__ */
