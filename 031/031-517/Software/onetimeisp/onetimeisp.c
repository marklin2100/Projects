#include <util/delay.h>

#include "hw.h"
#include "isp.h"

#include "target.h"

#define FLASH_PAGE_SIZE 128
static volatile uint16_t blink_time = 0;;
enum {
  PANIC_ISP_NOT_ENABLED,
  PANIC_BAD_SIGNATURE,
  PANIC_CHIP_ERASE_FAILED,
  PANIC_INVALID_CHECKSUM
};

static void cksum(uint16_t *sum, uint16_t w) {
  *sum = (*sum >> 1) + ((*sum & 1) << 15);
  *sum += LOW(w);
  *sum &= 0xffff;
  *sum = (*sum >> 1) + ((*sum & 1) << 15);
  *sum += HIGH(w);
  *sum &= 0xffff;
}

static void panic(int code) {
  uart_putnum("ISP panic: ", code);
  for (;;) {
    led_off();
    _delay_ms(2000);
    int i;
    for (i = 3; i >= 0; i--) {
      if (code & (1 << i)) {
        led_green();
      } else {
        led_red();
      }
      _delay_ms(250);
      led_off();
      _delay_ms(250);
    }
  }
}

int main() {
  uint32_t chip_id;
  unsigned int offset;
  uint16_t read_cksum, write_cksum;
  uint16_t page_addr = 0;

  led_init();
  uart_init();
  isp_init();

  /* Prologue */
  _delay_ms(500);
  uart_puts("Started One-Time-ISP\r\n");
  led_red();
  _delay_ms(500);
  led_off();
  _delay_ms(500);

  /* Wait for Programming Enable response */
  uart_puts("Running Programming Enable command...\r\n");
  isp_enter();
  if (isp_enable() == 0) { /* retry */
    uart_puts("Warning: Programming Enable command failed, retrying...\r\n");
    isp_exit();
    isp_enter();
    if (isp_enable() == 0) {
      uart_puts("Error: Programming Enable command failed\r\n");
      panic(PANIC_ISP_NOT_ENABLED);
    }
  }

  /* Start ISP process */
  led_red();

  /* Read chip signature bytes */
  uart_puts("Retrieveing chip id...\r\n");
  chip_id = isp_chip_id();
  uart_putnum("Chip id: ", chip_id);

  if (chip_id != 0x1e960a) {
    uart_puts("Error: Bad chip id\r\n");
    panic(PANIC_BAD_SIGNATURE);
  }

  /* Perform chip erase */
  uart_puts("Performing chip erase...\r\n");
  if (!isp_erase()) {
    uart_puts("Performing chip erase...\r\n");
    panic(PANIC_CHIP_ERASE_FAILED);
  }

  isp_exit();
  isp_enter();
  uart_puts("Re-enabling programming mode...\r\n");
  if (isp_enable() == 0) {
    uart_puts("Error: Programming Enable command failed\r\n");
    panic(PANIC_ISP_NOT_ENABLED);
  }

  /* Send the firmware */
  uart_puts("Uploading the firmware...\r\n");
  led_green();

  read_cksum = write_cksum = 0;
  uint8_t hi;
  uint8_t lo;
  uint16_t w;
  uint8_t page_offset;
  for (offset = 0; offset < (fw_bin_len + fw1_bin_len); offset += 2) {
    page_offset = (offset >> 1) % FLASH_PAGE_SIZE;
    if(offset < fw_bin_len){
      hi = pgm_read_byte(&fw_bin[offset+1]);
      lo = pgm_read_byte(&fw_bin[offset]);
    }else{
      hi = pgm_read_byte(&fw1_bin[offset+1-fw_bin_len]);
      lo = pgm_read_byte(&fw1_bin[offset-fw_bin_len]);
    }
    w = WORD(hi, lo);

    if (page_offset == 0) {
      if (offset != 0) {
        uart_putnum("writing page: ",page_addr);
        isp_write_page(page_addr);
      }
      page_addr = offset >> 1;
    }
    cksum(&write_cksum, w);
    isp_load_page(page_offset, w);

    blink_time++;
    if(blink_time >= 0x03f){
      led_blink();
      blink_time = 0;
    }

  }

  uart_putnum("writing page: ",page_addr);
  isp_write_page(page_addr);
  led_blink();
  uart_putnum("offset = ",offset);
  uart_putnum("Checksum: ", write_cksum);


  blink_time = 0;
  for (offset = 0; offset < (fw_bin_len + fw1_bin_len); offset += 2) {
    uint16_t w = isp_read(offset >> 1);
    cksum(&read_cksum, w);
    blink_time++;
    if(blink_time >= 0x3f){
      led_blink();
      blink_time = 0;
    }
  }
  uart_putnum("Verifying checksum: ", read_cksum);

  if (read_cksum != write_cksum) {
    uart_puts("Error: Invalid checksum\r\n");
    panic(PANIC_INVALID_CHECKSUM);
  }

  /* End */
  uart_puts("ISP completed successfully\r\n");
  isp_exit();

  led_green();
  for (;;);
  return 0;
}
