#ifndef __SD_H__
#define __SD_H__
/*
 * SD card interface
 */

//#define sd_select()   SD_PORT &= ~(1 << SD_SS);SD_PORT &= ~(1 << SD_SS0)
//#define sd_deselect() SD_PORT |= (1 << SD_SS);SD_PORT |= (1 << SD_SS0)

#define sd_selectx(x)   SD_PORT &= ~(1 << SD_SS##x)
#define sd_deselectx(x) SD_PORT |= (1 << SD_SS##x)

#define sd_detect() (SD_CD_PIN & (1 << SD_CD))

#define sd_send(b)	spi_send(b)
#define sd_recv()		spi_send(0xff)

enum {
	CMD_GO_IDLE_STATE = 0x00,
	CMD_SEND_OP_COND = 0x01,
	CMD_SEND_IF_COND = 0x08,
	CMD_SET_BLOCKLEN = 0x10,
	CMD_READ_SINGLE_BLOCK = 0x11,
	CMD_SD_SEND_OP_COND = 0x29,
	CMD_APP = 0x37,
	CMD_READ_OCR = 0x3a
};

enum {
	R1_IDLE_STATE = 0,
	R1_ERASE_RESET = 1,
	R1_ILL_COMMAND = 2,
	R1_COM_CRC_ERR = 3,
	R1_ERASE_SEQ_ERR = 4,
	R1_ADDR_ERR = 5,
	R1_PARAM_ERR = 6
};

extern uint8_t sdok;

void sd_select(void);
int sd_init();
void sd_deselect(void);
void set_sd_channel(uint8_t channel);
uint8_t sd_cmd(uint8_t cmd, uint32_t arg);
uint8_t sd_read_block(uint32_t block_offset, uint8_t block[512]);


#endif /* __SD_H__ */
