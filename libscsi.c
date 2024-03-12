#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <scsi/sg.h>

#include <unistd.h>
#include <linux/fs.h> /* for BLKGETSIZE */

#include "libscsi.h"

//lock/unlock feature implementation
int do_lock_unlock(int nargs, char **argv)
{
	int fd, ret = 0;
	char *device;
	__u8 data_block[DATA_BLOCK_SIZE]={0};
	__u8 data_block_onebyte[1]={0};
	int block_size = 0;
	// struct mmc_ioc_cmd idata;
	// struct sg_io_hdr_t io_hdr;
	int cmd42_para; //parameter of cmd42
	char pwd[MAX_PWD_LENGTH+1]; //password
	int pwd_len; //password length
	__u32 r1_response; //R1 response token 
	// CHECK(nargs != 4, "Usage: mmc cmd42 <password> <s|c|l|u|e> <device>\n", exit(1));
	strcpy(pwd, argv[1]);
	pwd_len = strlen(pwd);

	if (!strcmp("s", argv[2])) {
		cmd42_para = CMD42_SET_PWD;
		printf("Set password: password=%s ...\n", pwd);
	}
	else if (!strcmp("c", argv[2])) {
		cmd42_para = CMD42_CLR_PWD;
		printf("Clear password: password=%s ...\n", pwd);
	}
	else if (!strcmp("l", argv[2])) {
		cmd42_para = CMD42_LOCK;
		printf("Lock the card: password=%s ...\n", pwd);
	}
	else if (!strcmp("sl", argv[2])) {
		cmd42_para = CMD42_SET_LOCK;
		printf("Set password and lock the card: password - %s ...\n", pwd);
	}
	else if (!strcmp("u", argv[2])) {
		cmd42_para = CMD42_UNLOCK;
		printf("Unlock the card: password=%s ...\n", pwd);
	}
	else if (!strcmp("e", argv[2])) {
		cmd42_para = CMD42_ERASE;
		printf("Force erase ... (Warning: all card data will be erased together with PWD!)\n");
	}
	else {
		printf("Invalid parameter:\n" "s\tset password\n" "c\tclear password\n" "l\tlock\n"
		"sl\tset password and lock\n" "u\tunlock\n" "e\tforce erase\n");
		exit(1);
	}
	device = argv[nargs-1];
	fd = open(device, O_RDWR);
	if (fd < 0) {
		perror("open");
		exit(1);
	}
	if (cmd42_para==CMD42_ERASE)
		block_size = 2; //set blk size to 2-byte for Force Erase @DDR50 compability
	else
		block_size = DATA_BLOCK_SIZE;
	printf("Set to data block length = %d byte(s).\n", block_size);
	if (cmd42_para==CMD42_ERASE) {
		data_block_onebyte[0] = cmd42_para;
	} 
	else {
		data_block[0] = cmd42_para;
		data_block[1] = pwd_len;
		memcpy((char *)(data_block+2), pwd, pwd_len);
	}

    unsigned char sense_b[32];
    unsigned char CmdBlk16[16] = 
    { 0xF5, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};
    sg_io_hdr_t io_hdr;
    memset(&io_hdr, 0, sizeof(sg_io_hdr_t));

    io_hdr.interface_id = 'S';
    io_hdr.cmd_len = sizeof(CmdBlk16);
    io_hdr.cmdp = CmdBlk16;
    io_hdr.mx_sb_len = sizeof(sense_b);
    io_hdr.sbp = sense_b;
    io_hdr.dxfer_direction = SG_DXFER_TO_DEV;
    io_hdr.dxfer_len = block_size;

	if(cmd42_para==CMD42_ERASE)	
		io_hdr.dxferp = data_block_onebyte;
	else
    	io_hdr.dxferp = data_block;
		
    io_hdr.timeout = 20000;
    if(ioctl(fd, SG_IO, &io_hdr) < 0)
    {
        printf("ioctl fail\n");
        ret = -1;
    }
    else
    {
        printf("ioctl success\n");
        printf("status: %d\n", io_hdr.status);
        ret = 1;
    }
	close(fd);
	return ret;
}