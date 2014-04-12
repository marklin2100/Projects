/*
 * It's a low-fat FAT implementation
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "diet.h"
#include "sd.h"

#ifdef __linux__ 
#define HAS_FPRINTF_DEBUG 1
#endif

/* Converting numbers to host endianness */
static uint16_t diet_int16(uint8_t *p) {
	return (((uint16_t) p[1]) << 8) | ((uint16_t) p[0]);
}

static uint32_t diet_int32(uint8_t *p) {
	return (((uint32_t) p[3]) << 24) | (((uint32_t) p[2]) << 16)
		| (((uint32_t) p[1]) << 8) | p[0];
}

#define DIET_FS_FAT12 12
#define DIET_FS_FAT16 16
#define DIET_FS_FAT32 32

/* 
 * MBR routines and constants
 */

/* Pointers to the partition record */
/*#define MBR_PART1(mbr)	((uint8_t *)mbr + 0x1be)
#define MBR_PART2(mbr)	((uint8_t *)mbr + 0x1ce)
#define MBR_PART3(mbr)	((uint8_t *)mbr + 0x1de)
#define MBR_PART4(mbr)	((uint8_t *)mbr + 0x1ee)
*/
/* MBR parititon type, e.g. FAT, ext, ... */
//#define MBR_PART_TYPE(part)		(((uint8_t *)(part))[4])

/* MBR partition LBA address */
//#define MBR_PART_LBA(part) 		(diet_int32((uint8_t *)(part)+8))

/* MBR signature */

/*#define MBR_PART_IS_FAT(p)	\
		(MBR_PART_TYPE(p) == MBR_PART_FAT32 || \
		 MBR_PART_TYPE(p) == MBR_PART_FAT16 || \
		 MBR_PART_TYPE(p) == MBR_PART_FAT12)
*/

/* current FAT partition information */
static char diet_fat_type = 0;
static uint32_t diet_fat_offset = 0;
static uint32_t diet_rootdir_offset = 0;
static uint32_t diet_cluster2_offset = 0;
static uint32_t diet_cluster_size = 0;

/* internal buffer to store temporary data */
static uint8_t diet_block[DIET_CONFIG_BLOCK_SIZE];

/*
 * This function "mounts" volume:
 *   - calculates cluster size
 *   - calculates FAT table offset
 *   - calculates root directory offset
 *   - calculates first data cluster offset
 */
static int diet_mount(uint32_t part_lba) {
	uint16_t bytes_per_sector;
	uint16_t reserved_sectors;
	uint8_t sectors_per_cluster;
	uint8_t fat_copies;
	uint16_t max_root_entries;
	uint32_t sectors_per_fat;
	uint32_t cluster_root_dir;

	diet_read_block(part_lba, diet_block);

	bytes_per_sector = diet_int16(&diet_block[DIET_OFFSET_BYTES_PER_SECTOR]);
	sectors_per_cluster = diet_block[DIET_OFFSET_SECTORS_PER_CLUSTER];
	reserved_sectors = diet_int16(&diet_block[DIET_OFFSET_RESERVED_SECTORS]);
	fat_copies = diet_block[DIET_OFFSET_NUMBER_OF_FATS];
	max_root_entries = diet_int16(&diet_block[DIET_OFFSET_ROOT_ENTRIES_COUNT]);

	/* Try FAT16 */
	cluster_root_dir = 2;
	sectors_per_fat = diet_int16(&diet_block[DIET_OFFSET_FAT16_SECTORS_PER_FAT]);
	if (sectors_per_fat == 0) {
		/* Otherwise, try FAT32 */
		diet_fat_type = DIET_FS_FAT32;
		sectors_per_fat = diet_int32(&diet_block[DIET_OFFSET_FAT32_SECTORS_PER_FAT]);
		cluster_root_dir = diet_int32(&diet_block[DIET_OFFSET_CLUSTER_ROOT_DIR]);
	}

	diet_cluster_size = sectors_per_cluster * bytes_per_sector;
	diet_fat_offset = (part_lba + reserved_sectors) * bytes_per_sector; // OR <<9
	diet_rootdir_offset = diet_fat_offset
		+ fat_copies * sectors_per_fat * bytes_per_sector;
	/* each file record take 32 bytes */
	diet_cluster2_offset = diet_rootdir_offset + max_root_entries * 32; 
#if HAS_FPRINTF_DEBUG
	fprintf(stderr, "BytesPerSector: %d\n", bytes_per_sector);
	fprintf(stderr, "SectorsPerCluster: %d\n", sectors_per_cluster);
	fprintf(stderr, "ReservedSectors: %d\n", reserved_sectors);
	fprintf(stderr, "FatCopies: %d\n", fat_copies);
	fprintf(stderr, "MaxRootEntries: %d\n", max_root_entries);

	fprintf(stderr, "SectorsPerFat: %d\n", sectors_per_fat);
	fprintf(stderr, "ClusterRootDir: %d\n", cluster_root_dir);

	fprintf(stderr, "ClusterSize: %d\n", diet_cluster_size);
	fprintf(stderr, "FatOffset: %d (0x%x)\n", diet_fat_offset, diet_fat_offset);
	fprintf(stderr, "RootDirOffset: %d (0x%x)\n", diet_rootdir_offset, diet_rootdir_offset);
	fprintf(stderr, "Cluster2 Offset: %d (0x%x)\n", diet_cluster2_offset, diet_cluster2_offset);

	fprintf(stderr, "\n");
#endif

	return 0;
}

/*
 * Read MBR, find boot sector, find FAT table
 */
int diet_init() {
	/* read MBR */
	diet_read_block(DIET_MBR_BLOCK, diet_block);

#if DIET_CONFIG_CHECK_MBR_SIGNATURE
	/* check MBR signature */
	if (diet_int16(&diet_block[DIET_OFFSET_MBR_SIGNATURE]) != MBR_SIGNATURE) {
		return DIET_BAD_MBR;
	}
#endif

	/* Try to mount 1st partition */
	uint8_t part_type = diet_block[DIET_OFFSET_MBR_PARTITION1 
		+ DIET_OFFSET_MBR_PARTITION_TYPE];
	uint16_t part_lba = diet_int16(&diet_block[DIET_OFFSET_MBR_PARTITION1
			+ DIET_OFFSET_MBR_PARTITION_LBA]);

	if (part_type == DIET_MBR_FAT16 || part_type == DIET_MBR_FAT12
			|| part_type == DIET_MBR_FAT32) {
		if (part_type == DIET_MBR_FAT12) diet_fat_type = DIET_FS_FAT12;
		if (part_type == DIET_MBR_FAT16) diet_fat_type = DIET_FS_FAT16;
		if (part_type == DIET_MBR_FAT32) diet_fat_type = DIET_FS_FAT32;
		return diet_mount(part_lba);
	} else {
		return DIET_BAD_PART;
	}
}

int diet_open(const char *filename, diet_file_t *f) {
	int i;

	for (i = 0; i < DIET_MAX_ROOT_DIR_FILES; i++) {
		int j;
		uint32_t block_num, block_offset;
		struct {
			uint8_t name[8];
			uint8_t ext[3];
			uint8_t attr;
			uint8_t time1[8];
			uint8_t eai[2];
			uint8_t time2[4];
			uint8_t offset[2];
			uint8_t size[4];
		} *fileinfo;
		
		uint32_t addr;
		addr = diet_rootdir_offset + i * 32;

		block_num = (addr >> 9);
		block_offset = (addr & 0x1ff);

    if(diet_read_block(block_num, diet_block))//read block
    {
      return -1; //if read block error so return error
    }

		fileinfo = (void *) (diet_block + block_offset);

		uint8_t c = fileinfo->name[0];

		if (c == DIET_FILE_NAME_NONE) 
			break;

		if (c == 0x2e || c == 0xe5 || fileinfo->attr == DIET_FILE_ATTR_LFN) {
			continue;
		}

		if (c == 0x05) fileinfo->name[0] = 0xe5;
#if HAS_FPRINTF_DEBUG
		for (j = 0; j < 8; j++) {
			fprintf(stderr, "%c", fileinfo->name[j]);
		}
		fprintf(stderr, ".");
		fprintf(stderr, "%c", fileinfo->ext[0]);
		fprintf(stderr, "%c", fileinfo->ext[1]);
		fprintf(stderr, "%c\n", fileinfo->ext[2]);
#endif

		for (j = 0; j < 8; j++) {
			if (fileinfo->name[j] != filename[j]) {
				break;
			}
		}

		if (filename[j] != '.') {
			continue;
		}

		if (filename[j+1] != fileinfo->ext[0]
				|| filename[j+2] != fileinfo->ext[1]
				|| filename[j+3] != fileinfo->ext[2]) {
			continue;
		}

		addr = ((uint32_t) diet_int16(fileinfo->eai) << 16)
			| diet_int16(fileinfo->offset);

		f->cluster_offset = 0;
		f->start = f->cluster = addr;
		f->size = diet_int32(fileinfo->size);
		f->eof = 0;
		return 0;
	}
	// If we are here - return NOT FOUND error
	return -1;
}

void diet_next_cluster(uint32_t cur, uint32_t *next) {
	uint32_t cluster_offset;
	if (diet_fat_type == DIET_FS_FAT12) {
		int odd = 0;
		/* 1.5 bytes per cluster, use 3 to avoid float numbers */
		cluster_offset = cur * 3; 
		odd = cluster_offset & 1;
		if (odd) {
			cluster_offset--; 
		}
		cluster_offset = cluster_offset >> 1;

		diet_read_block((diet_fat_offset + cluster_offset) >> 9, diet_block);
		*next = (uint32_t) diet_int16(diet_block + (cluster_offset & 0x1ff));
		if (odd) {
			*next = (*next & 0xfff0) >> 4;
		} else {
			*next = *next & 0xfff;
		}
	} else if (diet_fat_type == DIET_FS_FAT16) {
		cluster_offset = cur * 2; /* 2 bytes per cluster */
		diet_read_block((diet_fat_offset + cluster_offset) >> 9, diet_block);
		*next = (uint32_t) diet_int16(diet_block + (cluster_offset & 0x1ff));
	} else if (diet_fat_type == DIET_FS_FAT32) {
		cluster_offset = cur * 4; /* 4 bytes per cluster */
		diet_read_block((diet_fat_offset + cluster_offset) >> 9, diet_block);
		*next = diet_int32(diet_block + (cluster_offset & 0x1ff));
	}
}

int diet_read(diet_file_t *f, uint8_t **buf) {
	uint32_t addr;

	if (f->cluster_offset >= diet_cluster_size) {
		uint32_t next;
		diet_next_cluster(f->cluster, &next);
		if (f->cluster == 0xffff) {
			//uart_puts("last cluster\r\n");
			f->eof= 1; // or treat as error?
			return 0;
		}
		f->cluster = next;
		f->cluster_offset -= diet_cluster_size;
	}

	addr = diet_cluster2_offset + ((f->cluster - 2) * diet_cluster_size) 
		+ f->cluster_offset;

	//diet_read_block(addr >> 9, diet_block);
  if(diet_read_block(addr >> 9, diet_block))//read block
  {
    //if read error ,return 0
    return 0;
  }
	*buf = diet_block;
	if (f->size < DIET_CONFIG_BLOCK_SIZE) {
		//uart_puts("Whole file is read\r\n");
		f->eof = 1;
		return f->size;
	} else {
		f->size -= DIET_CONFIG_BLOCK_SIZE;
		f->cluster_offset += DIET_CONFIG_BLOCK_SIZE;
		return DIET_CONFIG_BLOCK_SIZE;
	}
}

