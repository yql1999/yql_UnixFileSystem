#ifndef __DEFINES_H__
#define __DEFINES_H__


//#define DISKPATH "D:\\OS\\UnixPath\\disk.dat"
#define DISKPATH "D:\\OS\\disk.dat"
#define DISK_SIZE 204800
#define BLOCK_SIZE 512
#define SUPER_BLOCK_SIZE 260
#define INODE_NUM 40
#define BLOCK_NUM 358
#define BLOCK_GROUP_SIZE 20
#define BLOCK_START 42
#define FINODE_SIZE 64

#define MAX_NAME_SIZE 14
#define MAX_DIRECT_NUM 31
#define DIR_PADING_SIZE 14
#define MAX_USERS 5
#define MAX_GROUP_NUM 5
#define OWNER_SIZE 16
#define GROUP_SIZE 16
#define OWNERS_SIZE 160
#define GROUPS_SIZE 82
#define FILE_MAX_SIZE 10510*512
#define ROOT 0

#define STATUS_SUCCESS 1
#define STATUS_NO_FREE_BLOCK -1
#define STATUS_NO_FREE_INODE -2
#define STATUS_BEYOND_DIRECT_NUM -3
#define STATUS_READONLY -4
#define STATUS_ERROR -5
#define STATUS_FILE_OPEN_ERROR -6
#define STATUS_FILENAME_EXIST	-7
#define STATUS_FILENAME_NOTEXIST -8
#define STATUS_BEYOND_RIGHT -9
#define STATUS_MOD_ERROR -10
#define STATUS_USER_NOTEXIST -11
#define STATUS_GROUP_NOTEXIST -12
#define STATUS_PASSWORD_WRONG -13
#define STATUS_REPSWD_WRONG -14
#define STATUS_NOT_FILE	-15
#define STATUS_BEYOND_SIZE -16
#define STATUS_WRITEONLY -17
#define STATUS_NOT_DIRECT -18
#define STATUS_TYPE_NOT_MATCH -19
#define STATUS_SOURCE_AND_TARGET_ERROR -20
#define STATUS_FILENAME_Illegal -21
#define STATUS_CANNOT_WRITE -22

#endif