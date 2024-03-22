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
int do_lock_unlock(char *device, int cmd42,char *pwd_arg)
{
	int fd, ret = 0;
	int cmd42_para; //parameter of cmd42
	char pwd[MAX_PWD_LENGTH+1]; //password
	int *status;

	fd = open(device, O_RDWR);
	if (fd < 0) {
		perror("open");
		exit(1);
	}

	// if(set_cmd_para(&cmd42_para, argv[2]) < 0)
	// {
	// 	close(fd);
	// 	exit(1);
	// }
	cmd42_para = cmd42;
	if(cmd42_para==CMD42_ERASE)
	{
		char *warnInfo = "Warning: all card data will be erased together with PWD";
		if(ask_yes_or_no(warnInfo) == 0)
		{
			close(fd);
			exit(1);
		}
		status = read_status(&fd);
		printf("lock/unlock: %d, result: %d\n", status[0], status[1]);
		
		if(status[0]==0)
		{
			show_cmd42_error_msg(cmd42_para, status[0]);
			close(fd);
			exit(1);
		}
	}
	else
	{		
		strcpy(pwd, pwd_arg);
	}
	
	ret = set_cmd42(cmd42_para, pwd, &fd);
	close(fd);
	return ret;
}

int set_cmd_para(int *cmd_para, char * arg)
{
	if (!strcmp("s", arg)) {
		*cmd_para = CMD42_SET_PWD;
		printf("Set password...\n");
	}
	else if (!strcmp("c", arg)) {
		*cmd_para = CMD42_CLR_PWD;
		printf("Clear password...\n");
	}
	else if (!strcmp("l", arg)) {
		*cmd_para = CMD42_LOCK;
		printf("Lock the card...\n");
	}
	else if (!strcmp("sl", arg)) {
		*cmd_para = CMD42_SET_LOCK;
		printf("Set password and lock the card ...\n");
	}
	else if (!strcmp("u", arg)) {
		*cmd_para = CMD42_UNLOCK;
		printf("Unlock the card ...\n");
	}
	else if (!strcmp("e", arg)) {
		*cmd_para = CMD42_ERASE;
		printf("Force erase ...\n");
	}
	else {
		printf("Invalid parameter:\n" "s\tset password\n" "c\tclear password\n" "l\tlock\n"
		"sl\tset password and lock\n" "u\tunlock\n" "e\tforce erase\n");
		return -1;
	}
	return 1;
}

void show_cmd42_error_msg(int cmd_para, int lock_status)
{
	printf("Warning: ");
	if (cmd_para == CMD42_SET_PWD) {
		printf("Already set password, try to use 'PasswordNewPassword' format to set new password\n");
	}
	else if (cmd_para == CMD42_CLR_PWD) {
		printf("Wrong password or already no set password\n");
	}
	else if (cmd_para == CMD42_LOCK) {
		if(lock_status==1)
			printf("Already lock\n");
		else
			printf("Wrong password or no password set yet\n");
	}
	else if (cmd_para == CMD42_SET_LOCK) {
		printf("Set password and lock the card fail\n");
	}
	else if (cmd_para == CMD42_UNLOCK) {
		if(lock_status==1)
			printf("Wrong password\n");
		else
			printf("Already unlock\n");
	}
	else if (cmd_para == CMD42_ERASE) {
		printf("Need to lock the card first\n");
	}
}

void disk_format(char *device)
{
	char cmd[30];
	strcpy(cmd, "sudo mkfs.vfat ");
	strcat(cmd, device);

	if(system(cmd) == -1)
		printf("Error to format\n");	
}

int do_read_status(int nargs, char **argv)
{
	int fd, ret = 0;
	int *curr_status;
	char *device;

	device = argv[nargs-1];
	fd = open(device, O_RDWR);
	if (fd < 0) {
		perror("open");
		exit(1);
	}

	curr_status = read_status(&fd);

	ret = curr_status[0];
	printf("lock/unlock: %d, result: %d\n", curr_status[0], curr_status[1]);
	if(ret)
		printf("Device is lock\n");
	else
		printf("Device is unlock\n");

	close(fd);
	return ret;
}

int * read_status(int *fd)
{
	int *ret = malloc(2);

	__u8 data_block[16]={0};
    unsigned char sense_b[16]={0};
    unsigned char CmdBlk16[16] = 
    { 0xF5, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};
    sg_io_hdr_t io_hdr;
    memset(&io_hdr, 0, sizeof(sg_io_hdr_t));

    io_hdr.interface_id = 'S';
    io_hdr.cmd_len = sizeof(CmdBlk16);
    io_hdr.cmdp = CmdBlk16;
    io_hdr.mx_sb_len = sizeof(sense_b);
    io_hdr.sbp = sense_b;
    io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
    io_hdr.dxfer_len = sizeof(data_block);
	io_hdr.dxferp = data_block;
		
    io_hdr.timeout = 20000;


    if(ioctl(*fd, SG_IO, &io_hdr) < 0)
    {
        printf("ioctl fail\n");
		ret[0] = -1;
        return ret;
    }

	if(data_block[1] & 0x02 )
		ret[0] = 1;
	else
		ret[0] = 0;

	if(data_block[1] & 0x01 )
		ret[1] = 1;
	else
		ret[1] = 0;

	return ret;
}

int set_cmd42(int cmd_para,  char *pwd, int *fd)
{
	int ret=0;

	__u8 data_block[DATA_BLOCK_SIZE]={0};
	__u8 data_block_onebyte[1]={0};
	int block_size = 0;
	int pwd_len = strlen(pwd);

	if (cmd_para==CMD42_ERASE)
		block_size = 2; //set blk size to 2-byte for Force Erase @DDR50 compability
	else
		block_size = DATA_BLOCK_SIZE;
	// printf("Set to data block length = %d byte(s).\n", block_size);
	if (cmd_para==CMD42_ERASE) {
		data_block_onebyte[0] = cmd_para;
	} 
	else {
		data_block[0] = cmd_para;
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

	if(cmd_para==CMD42_ERASE)	
		io_hdr.dxferp = data_block_onebyte;
	else
    	io_hdr.dxferp = data_block;
		
    io_hdr.timeout = 20000;
    if(ioctl(*fd, SG_IO, &io_hdr) < 0)
    {
        printf("ioctl fail\n");
        ret = -1;
    }
    else
    {
        ret = 1;
    }

	return ret;
}

int ask_yes_or_no(char * warnInfo)
{	
	printf("%s\n",warnInfo);
	printf("Do you want to continue? [Y/n] ");

	char answer[5];
	int c;
	while (1)
	{
		scanf("%3s", answer);
		if(answer[0] == 'Y' || answer[0] == 'y')
			return 1;
		else if(answer[0] == 'N' || answer[0] == 'n')
			return 0;
		else
			printf("Please press [Y/y] to continue or [N/n] to cancel :");

		while((c = fgetc(stdin)) != '\n' && c != EOF);//Flush stdin
	}
}