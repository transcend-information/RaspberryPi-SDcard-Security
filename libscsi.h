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


int do_lock_unlock(int nargs, char **argv); //CMD42
int do_lock_unlock_test(char *device, int cmd42,char **pwd);
void disk_format(char *device);
int do_read_status(int nargs, char **argv);

int set_cmd_para(int *cmd_para, char * arg);
int set_cmd42(int cmd_para,  char *pwd, int * fd);
int * read_status(int *fd);
void show_cmd42_error_msg(int cmd_para, int lock_status, int pwd_num);
int ask_yes_or_no(char * warnInfo);