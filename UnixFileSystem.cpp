/*
UnixFileSystem.cpp : ģ��UNIX�ļ�ϵͳ
Author��Ҷٻ��
Time��2019/12/13
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
