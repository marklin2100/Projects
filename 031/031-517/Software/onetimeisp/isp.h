#ifndef __ISP_H__
#define __ISP_H__

#define TARGET    517

#define LOW(w) ((w) & 0xff)
#define HIGH(w) (((w) >> 8) & 0xff)
#define WORD(hi, lo) ((((uint16_t) hi) << 8) | (lo))

/*
 * ISP bus (UART in SPI mode)
 */
static void isp_init() {
	DDRD = (1 << PD3) | (1 << PD4) | (1 << PD5); /* MOSI and SCK and RESET */
}

static void isp_reset_on() {
	PORTD |= (1 << PD5);
}

static void isp_reset_off() {
	PORTD &= ~(1 << PD5);
}

static void isp_send(uint8_t *cmd, uint8_t *res, int sz) {
	int i;

	/*for (i = 0; i < sz; i++) {*/
		/*uart_puthex(cmd[i]);*/
	/*}*/
	/*uart_puts(" -> ");*/

	for (i = 0; i < sz; i++) {
		uint8_t c = cmd[i];
		uint8_t r = 0;
		uint8_t mask;
		for (mask = 0x80; mask; mask = mask >> 1) {
			if (c & mask) {
				PORTD |= (1 << PD3);
			} else {
			}
			asm("nop"); asm("nop");
			PORTD |= (1 << PD4);
			asm("nop"); asm("nop");
			r = r << 1;
			if (PIND & (1 << PD2)) {
				r++;
			}
			PORTD &= ~((1 << PD3) | (1 << PD4));
		}
		/*uart_puthex(r);*/
		res[i] = r;
	}
	/*uart_puts("\r\n");*/
}

static void isp_enter() {
	/* Give RESET a pulse for 2 cycles */
	isp_reset_on();
	asm("nop"); asm("nop");
	isp_reset_off();
	/* Wait 20 ms and send Programming Enable command */
	_delay_ms(20);
}

static void isp_exit() {
	isp_reset_on();
}

static int isp_enable() {
	uint8_t init[] = {0xac, 0x53, 0x00, 0x00};
	uint8_t res[4];

	isp_send(init, res, 4);
	return (res[2] == 0x53);
}

static uint32_t isp_chip_id() {
	uint8_t id0[] = {0x30, 0x00, 0x00, 0x00};
	uint8_t id1[] = {0x30, 0x00, 0x01, 0x00};
	uint8_t id2[] = {0x30, 0x00, 0x02, 0x00};
	uint8_t res[4];
	uint32_t chip_id = 0;

	isp_send(id0, res, 4);
	if (res[1] != 0x30) return 0;
	chip_id |= ((uint32_t) res[3] << 16);

	isp_send(id1, res, 4);
	if (res[1] != 0x30) return 0;
	chip_id |= ((uint32_t) res[3] << 8);

	isp_send(id2, res, 4);
	if (res[1] != 0x30) return 0;
	chip_id |= res[3];
	return chip_id;
}

static int isp_erase() {
	uint8_t erase[] = {0xac, 0x80, 0x00, 0x00};
	uint8_t res[4];

	isp_send(erase, res, 4);
	if (res[2] != 0x80) return 0;
	return 1;
}

static uint16_t isp_read(uint16_t addr) {
	uint8_t read_lo[] = {0x20, (addr >> 8) & 0xff, addr & 0xff, 0x00};
	uint8_t read_hi[] = {0x28, (addr >> 8) & 0xff, addr & 0xff, 0x00};
	uint8_t res[4];

	uint16_t r = 0;

	isp_send(read_lo, res, 4);
	r |= res[3];

	isp_send(read_hi, res, 4);
	r |= ((uint16_t) res[3] << 8);

	return r;
}

static int isp_load_page(uint8_t addr, uint16_t value) {
	uint8_t write_lo[] = {0x40, 0, addr, value & 0xff};
	uint8_t write_hi[] = {0x48, 0, addr, (value >> 8) & 0xff};
	uint8_t res[4];

	isp_send(write_lo, res, 4);
	isp_send(write_hi, res, 4);

	return 1;
}

static void isp_write_page(uint16_t addr) {
	uint8_t write_page[] = {0x4c, (addr >> 8) & 0xff, addr & 0xff, 0};
	uint8_t res[4];
	isp_send(write_page, res, 4);
	_delay_ms(100);
}


#endif /* __ISP_H__ */
