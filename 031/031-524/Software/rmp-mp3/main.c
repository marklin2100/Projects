#include "hw.h"
#include "vs.h"
#include "sd.h"

#include "diet.h"

int main() {
	int r;

	/* initialize GPIO */
	led_init();

	led_on(LED1);

	uart_init();

	uart_puts("\r\n\r\nloading...\r\n");

	/* 
	 * some ISP programmers toggle SCK pin for ~250ms after reset,
	 * so a delay on startup is required 
	 */
	_delay_ms(500);

	uart_puts("initialize SPI\r\n");
	spi_init();

	/* AVRs should send dummy byte to initialize SPI */
	spi_send(0xff);

	led_on(LED2);

	uart_puts("initialize SD card module\r\n");
	sd_init();

	//uart_puts("initialize sound chip VS1053\r\n");
	//vs_init();

#if RUN_TESTS
	/*
	uint16_t mem;
	uart_puts("running memory test\r\n");
	mem = vs_test_memory();
	uart_puthex16(mem); uart_puts("\r\n");
	*/
	uart_puts("running sine test\r\n");
	vs_test_sine_on();
	_delay_ms(4000);
	uart_puts("stopping sine test\r\n");
	vs_test_sine_off();
	for (;;);
	led_off(LED1);
	led_off(LED2);
#endif

	led_off(LED1);
	led_off(LED2);

	/* enable streaming and SDI_NEW */
	//uart_puts("using SDI_NEW straming\r\n");
	//vs_sci_write(0x00, 0x0820);

	/* set volume */
	//uart_puts("set audio volume\r\n");
	//vs_sci_write(0x0b, 0x0000);

	/* set SCI_CLOCK */
	//uart_puts("set audio clock\r\n");
	//vs_sci_write(0x03, 0x6800);

	uart_puts("set high speed for SPI bus\r\n");
	spi_set_high_speed();


#if 1
	r = diet_init();
	if (r != 0) {
		uart_puts("Bad filesystem/MBR: ");
		uart_puthex16(r);
		uart_puts("\r\n");
	}

	diet_file_t f;
	uint8_t *buf;
	int n;
#if 0
	r = diet_open("01.FLA", &f);
	if (r == -1) {
		uart_puts("File not found\r\n");
	}
	do {
		uart_puts(".");
		n = diet_read(&f, &buf);
		uart_puts("!");
		//vs_sdi_write(buf, 512);
		if (f.eof) {
			uart_puts("f.eof == ");
			uart_puthex(f.eof);
			uart_puts("\r\n");
			break;
		}
	} while (1);
	uart_puts("\r\nEOF\r\n");
#else
	r = diet_open("01.TXT", &f);
	uint16_t cksum = 0;
	int size = 0;
	if (r == -1) {
		uart_puts("File 01.TXT not found\r\n");
	} else 
	do {
		uart_puts(".");
		n = diet_read(&f, &buf);
		uart_puts("!");
		int i;
		for (i = 0; i < n; i++) {
			cksum = (cksum >> 1) + ((cksum & 1) << 15);
			cksum += buf[i];
			cksum &= 0xffff;
			size++;
		}
		//vs_sdi_write(buf, 512);
		if (f.eof) {
			uart_puts("f.eof == ");
			uart_puthex(f.eof);
			uart_puts("\r\n");
			break;
		}
	} while (1);
	/* r = (s & 0xffff) + ((s & 0xffffffff) >> 16);
	   checksum = (r & 0xffff) + (r >> 16); */
	uart_puts("Size in bytes: ");
	uart_puthex16(size);
	uart_puts("Checksum: ");
	uart_puthex16(cksum);
	uart_puts("\r\nEOF\r\n");
#endif
#endif

	uart_puts("starting playback\r\n");

	int i;
	for (i = 0; i < 10000; i++) {
		/*
		static uint8_t sd_block[2048];
#if 1
		sd_read_block(i, sd_block);
		i++;
		sd_read_block(i, sd_block+512);
		i++;
		sd_read_block(i, sd_block+1024);
		i++;
		sd_read_block(i, sd_block+1024+512);
#endif
		vs_sdi_write(sd_block, 2048);*/

		static uint8_t sd_block[512];
		sd_read_block(i, sd_block);
		uart_putc('!');
		vs_sdi_write(sd_block, 512);
		uart_putc('.');
	}

	uart_puts("starting endless loop\r\n");

	for (;;);
	return 0;
}

