/*
UnixFileSystem.cpp : 模拟UNIX文件系统
Author：叶倩琳
Time：2019/12/13
*/

#include"stdafx.h"
#include "heads.h"

int main(){
	init();
	if(login()==STATUS_SUCCESS){
		menu();
		choose();
	}
	return 0;
}

