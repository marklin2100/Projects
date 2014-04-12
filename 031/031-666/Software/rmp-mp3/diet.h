#ifndef __DIET_H__
#define __DIET_H__

#include <stdint.h>

enum {
	DIET_SUCCESS = 0,
	DIET_BAD_MBR = -1,
	DIET_BAD_PART = -2
};

/* Most of the SD cards support block size of 512 bytes */
#define DIET_CONFIG_BLOCK_SIZE 512

#define DIET_CONFIG_CHECK_MBR_SIGNATURE 0

/* For FAT16 must be <= 512 entries */
#define DIET_MAX_ROOT_DIR_FILES 4096

/* Function to use when reading blocks */
#define diet_read_block(block_number, buffer)	sd_read_block(block_number, buffer)

typedef struct {
	uint32_t size;
	uint32_t start;
	/* current position */
	uint32_t cluster;
	uint32_t cluster_offset;
	uint8_t eof;
} diet_file_t;

int diet_init();
int diet_open(const char *filename, diet_file_t *f);
int diet_read(diet_file_t *f, uint8_t **buf);

/* 
 * Constants 
 */

/* MBR information */
#define DIET_MBR_BLOCK 0

#define DIET_OFFSET_MBR_PARTITION1     0x1be
#define DIET_OFFSET_MBR_SIGNATURE 0x1fe

#define DIET_OFFSET_MBR_PARTITION_TYPE 0x04
#define DIET_OFFSET_MBR_PARTITION_LBA  0x08

#define DIET_MBR_FAT32 0x0b /* possible: 0x0c */
#define DIET_MBR_FAT16 0x06 /* possible: 0x04 or 0x0e */
#define DIET_MBR_FAT12 0x01
#define DIET_MBR_SIGNATURE 0xaa55


/* FAT information */
#define DIET_OFFSET_BYTES_PER_SECTOR 0x0b
#define DIET_OFFSET_SECTORS_PER_CLUSTER 0x0d
#define DIET_OFFSET_RESERVED_SECTORS 0x0e
#define DIET_OFFSET_NUMBER_OF_FATS   0x10
#define DIET_OFFSET_ROOT_ENTRIES_COUNT 0x11
#define DIET_OFFSET_FAT16_SECTORS_PER_FAT 0x16
#define DIET_OFFSET_FAT32_SECTORS_PER_FAT 0x24
#define DIET_OFFSET_CLUSTER_ROOT_DIR 0x2c

/* File entry information */

#define DIET_DIR_ATTR 0x0b
#define DIET_FILE_ATTR_LFN 0x0f
#define DIET_FILE_NAME_NONE 0x00


#endif /* __DIET_H__ */
