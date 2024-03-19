#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>
#include <fcntl.h>
#include <unistd.h>
#include "libscsi.h"

#define MAX_PWD_LEN 16
#define MIN_PWD_LEN 4

struct  arguments
{
	int cmd42_para;
	char *pwd[2];
	int pwd_num;
	// char *new_pwd;
	int cmd13;
	char *device;
	int arg_count;
};

int parse_pwd(char *arg, int max_num, struct arguments *argument)
{
	char *token;
	int count=0;
	const char s[2] = " ";
	token = strtok(arg, s);
	while(token != NULL)
	{
		if(count < max_num)
		{
			argument->pwd[count]=token;
			count++;
			token = strtok(NULL, s);
		}
		else
			return 0;
	}
	argument->pwd_num = count;
	return 1;
}

int match_pwd_rule(char *pwd, char **note)
{
	if(strlen(pwd) > MAX_PWD_LEN)
	{
		*note = "Password need 4~16 characters";
		return 0;
	}
	else if(strlen(pwd) < MIN_PWD_LEN)
	{
		*note = "Password need 4~16 characters";
		return 0;
	}
	return 1;
}

static int parse_opt(int key, char *arg, struct argp_state *state)
{
	struct arguments *argument = state->input;	

	switch(key){
		case 's':{
			if(argument->cmd42_para < 0)
				argument->cmd42_para = CMD42_SET_PWD;
			else
				goto Confused;
			break;
		}
		case 'q':{
			if(argument->cmd42_para < 0)
				argument->cmd42_para = CMD42_SET_LOCK;
			else
				goto Confused;
			break;
		}
		case 'c':{
			if(argument->cmd42_para < 0)
				argument->cmd42_para = CMD42_CLR_PWD;
			else
				goto Confused;
			break;
		}
		case 'l':{
			if(argument->cmd42_para < 0)
				argument->cmd42_para = CMD42_LOCK;
			else
				goto Confused;
			break;
		}
		case 'u':{
			if(argument->cmd42_para < 0)
				argument->cmd42_para = CMD42_UNLOCK;
			else
				goto Confused;
			break;
		}
		case 'e':{
			if(argument->cmd42_para < 0)
				argument->cmd42_para = CMD42_ERASE;
			else
				goto Confused;
			break;
		}
		case 't':{
			argument->cmd13 = 1;
			break;
		}
		case ARGP_KEY_ARG:{
			argument->device = arg;
			argument->arg_count++;
			// printf("%s\n", arg);
			break;
		}
		case ARGP_KEY_END:{
			if(argument->arg_count < 1)
				argp_error(state, "requires minimum 1 device path");
			break;
		}
		Confused:{
			argp_error(state, "the command is confuseing");	
			return -1;
		}
	}

	switch (key)
	{
		case 's': case 'q':{
			if(!parse_pwd(arg, 2, argument))
			{
				argp_failure(state, 0, 0,
								"Defining too many sets of password, "
								"please set according to the format: \n"
								"\t-s \"Password New_Password\""
						);
				return -1;
			}
			
			char *ret_note;
			if(!match_pwd_rule(argument->pwd[0], &ret_note) || 
				(argument->pwd[1] != NULL && !match_pwd_rule(argument->pwd[1], &ret_note)))
			{
				argp_failure(state, 0, 0, "%s", ret_note);
				return -1;
			}
			break;
		}
		case 'c': case 'l': case 'u':{
			if(!parse_pwd(arg, 1, argument))
			{
				argp_failure(state, 1, 0, "Password is illegal");
				return -1;
			}
			
			char *ret_note;
			if(!match_pwd_rule(argument->pwd[0], &ret_note))
			{
				argp_failure(state, 0, 0, "%s", ret_note);
				return -1;
			}
			break;
		}
		
		default:
			break;
	}

	return 0;

error:
	return -1;
}

int main(int argc, char **argv )
{
	struct argp_option options[] = {
		{0, 0, 0, 0, "Security Options:", 1},
		{"set-pwd", 's', "\"Password [New Password]\"", 0, "Set SD password or change password"},
		{"quick-lock", 'q', "\"Password [New Password]\"", 0, "Set SD password and lock the card"},		
		{"clear", 'c', "Password", 0, "Clear SD password"},
		{"lock"	, 'l', "Password", 0, "Lock SD by password"},
		{"unlock", 'u', "Password", 0, "Unlock SD by password"},
		{"erase", 'e', 0, 0, "Force erase SD card"},
		{0, 0, 0, 0, "Status Options:", 2},
		{"status", 't', 0, 0, "Check SD state"},
		{0}
	};

	struct arguments argument = {
		.cmd42_para = -1,
		.pwd = NULL,
		.cmd13 = 0,
		.device = NULL,
	};
	struct argp argp = {options, parse_opt, "Device", 0};
	if(!argp_parse(&argp, argc, argv, 0, 0, &argument))
	{
		printf("Device is %s\n"
			   "cmd42 is %d\n"
			   "pwd is %s\n"
			   "new_pwd is %s\n"
			   "cmd13 is %d\n",
			   argument.device,
			   argument.cmd42_para,
			   argument.pwd[0],
			   argument.pwd[1],
			   argument.cmd13);

		if(argument.cmd42_para != -1)
		{
			int ret = 0;
			ret = do_lock_unlock(argument.device, argument.cmd42_para, argument.pwd);
			if(ret == 1)
			{	
				int fd;
				int *status;
				if(argument.pwd[0] != NULL)
				{

				}
				fd = open(argument.device, O_RDWR);
				if (fd < 0) {
					perror("open");
					exit(1);
				}

				status = read_status(&fd);

				printf("lock/unlock: %d, result: %d\n", status[0], status[1]);

				if(status[1]==1)
					show_cmd42_error_msg(argument.cmd42_para, status[0], argument.pwd_num);
				else
				{
					if(status[0]==1)
						printf("Status: Lock\n");
					else
					{
						if(argument.cmd42_para==CMD42_ERASE)	
							disk_format(argument.device);
						printf("Status: Unlock\n");
					}
				}
				close(fd);
			}
		}
		else if(argument.cmd13 == 1)
		{
			int fd, ret;
			int *status;
			fd = open(argument.device, O_RDWR);
			if (fd < 0) {
				perror("open");
				exit(1);
			}
			status = read_status(&fd);

			ret = status[0];
			printf("lock/unlock: %d, result: %d\n", status[0],status[1]);
			if(ret)
				printf("Device is lock\n");
			else
				printf("Device is unlock\n");

			close(fd);

		}

		return 0;
	}
	return 1;
}