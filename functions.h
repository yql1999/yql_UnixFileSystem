#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include"heads.h"
using namespace std;

void initGV(FILE* file);
void init();
int login();

bool readInode(inode* i);
bool writeInode(inode* i);
bool readDir(unsigned int blockId,dirs* d);
bool writeDir(unsigned int blockId,dirs* d);
bool readNextBG();
bool writeSuperBlock();
int readContent(inode* i);
int writeContent(inode* i,string text);
int readCurDir();
int getFreeBlock();
int getFreeInode();
string trim(string s);
string getText(inode* temp);
bool writeOS();
int freeBlock(unsigned int blockId);
int freeInode(unsigned int inodeId);
void rmInode(unsigned short inodeId);
int passwd();
void clear();
void exit();
void help(string cmd);
int superMkdir(inode* parent,char name[MAX_NAME_SIZE],unsigned short ownerId,unsigned short groupId);

int chmod(char name[MAX_NAME_SIZE],unsigned int power);
int chown(char name[MAX_NAME_SIZE],unsigned short ownerId);
int chgrp(char name[MAX_NAME_SIZE],unsigned short groupId);
bool whetherContainsIllegalCharacters(string name);
int mv(char oldName[MAX_NAME_SIZE],char newName[MAX_NAME_SIZE]);
int cp(char source[MAX_NAME_SIZE],char des[MAX_NAME_SIZE]);
int rm(char name[MAX_NAME_SIZE]);
int cat(char name[MAX_NAME_SIZE]);
int ln(char source[MAX_NAME_SIZE],char des[MAX_NAME_SIZE]);
bool checkFileName(char name[MAX_NAME_SIZE]);
int touch(inode* parent,char name[MAX_NAME_SIZE]);
int textAppend(char name[MAX_NAME_SIZE]);
int vi(char name[MAX_NAME_SIZE]);

string ls();
string pwd();
int cd(char name[MAX_NAME_SIZE]);
int mkdir(inode* parent,char name[MAX_NAME_SIZE]);
int rmdir(char name[MAX_NAME_SIZE]);

void choose();
void menu();

#endif