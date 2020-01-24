#ifndef __STRUCTS_H__
#define __STRUCTS_H__

#include"defines.h"


struct superBlock{
	unsigned int size;
	unsigned int freeInodeNum;
	unsigned int freeInode[INODE_NUM];
	unsigned int nextFreeInode;
	unsigned int freeBlockNum;
	unsigned int freeBlock[BLOCK_GROUP_SIZE];
	unsigned int nextFreeBlock;
};

struct finode{
	unsigned int fileSize;
	unsigned int linkNum;
	unsigned int address[6];
	unsigned short userId;
	unsigned short groupId;
	unsigned int power;
	unsigned long latestCreateTime;
	unsigned long latestWriteTime;
	unsigned long latestReadTime;
};

struct inode{
	finode finode;
	inode* parent;
	unsigned short inodeId;
};

struct dir{
	char name[MAX_NAME_SIZE];
	unsigned short inodeId;
};

struct dirs{
	unsigned short dirNum;
	dir dir[MAX_DIRECT_NUM];
	char padding[DIR_PADING_SIZE];
};

struct user{
	unsigned short userId;
	unsigned short groupId;
	char userName[MAX_NAME_SIZE];
	char userPassword[MAX_NAME_SIZE];
};

struct users{
	unsigned short userNum;
	user userlist[MAX_USERS];
};

struct group{
	unsigned short groupId;
	char groupName[MAX_NAME_SIZE];
};

struct groups{
	unsigned short groupNum;
	group grouplist[MAX_GROUP_NUM];
};


#endif