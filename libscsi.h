/* CMD42 */
#define SET_BLOCKLEN 16 /* ac [31:0] block len R1 */
#define LOCK_UNLOCK 42 /* adtc R1b */
#define CMD42_UNLOCK 0x0 /* UNLOCK */
#define CMD42_SET_PWD 0x1 /* SET_PWD */
#define CMD42_CLR_PWD 0x2 /* CLR_PWD */
#define CMD42_LOCK 0x4 /* LOCK */
#define CMD42_SET_LOCK 0x5 /* SET_PWD & LOCK */
#define CMD42_ERASE 0x8 /* ERASE */
#define MAX_PWD_LENGTH 32 /* max PWDS_LEN: old+new */
#define DATA_BLOCK_SIZE 512 /* data blk size for cmd42 */
#define R1_ERROR (1 << 19) /* R1 bit19 */
#define R1_LOCK_ULOCK_FAIL (1 << 24) /* R1 bit24 */



#include <linux/mmc/ioctl.h>
/* From kernel linux/mmc/core.h */
#define MMC_RSP_NONE	0			/* no response */
#define MMC_RSP_PRESENT	(1 << 0)
#define MMC_RSP_136	(1 << 1)		/* 136 bit response */
#define MMC_RSP_CRC	(1 << 2)		/* expect valid crc */
#define MMC_RSP_BUSY	(1 << 3)		/* card may send busy */
#define MMC_RSP_OPCODE	(1 << 4)		/* response contains opcode */

#define MMC_CMD_AC	(0 << 5)
#define MMC_CMD_ADTC	(1 << 5)
#define MMC_CMD_BC	(2 << 5)

#define MMC_RSP_SPI_S1	(1 << 7)		/* one status byte */
#define MMC_RSP_SPI_BUSY (1 << 10)		/* card may send busy */

#define MMC_SEND_STATUS		13	/* ac   [31:16] RCA        R1  */
#define MMC_CMD_AC	(0 << 5)
#define MMC_CMD_ADTC	(1 << 5)
#define MMC_CMD_BC	(2 << 5)


#define MMC_RSP_R1	(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R1B	(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE|MMC_RSP_BUSY)

#define RDF5CMD42 { 0xF5, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00}
#define RDF5CMD13 { 0xF5, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00}



int do_lock_unlock(char *device, int cmd42,char *pwd); //CMD42
void disk_format(char *device);
int do_read_status(int nargs, char **argv);

int set_cmd_para(int *cmd_para, char * arg);
int set_cmd42(int cmd_para,  char *pwd, int * fd);
int * read_status(int *fd);
void show_cmd42_error_msg(int cmd_para, int lock_status);
int ask_yes_or_no(char * warnInfo);

int send_status(int fd, __u32 *response);