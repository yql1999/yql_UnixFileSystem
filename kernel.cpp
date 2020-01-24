#include"stdafx.h"
#include"heads.h"
#include"functions.h"
using namespace std;


superBlock* super=new superBlock();
inode* root=new inode();
users* userlist=new users();
groups* grouplist=new groups();
user* curUser;
inode* curInode=root;
vector<dir*>curPath;
dirs* curDir=new dirs();

void initGV(FILE* f){
	super->size=DISK_SIZE;
	super->freeBlockNum=BLOCK_NUM;
	super->freeInodeNum=INODE_NUM-1;
	for(int j=0;j<INODE_NUM;j++)
		super->freeInode[j]=INODE_NUM+1-j;
	super->nextFreeInode=INODE_NUM-2;
	
	root->parent=NULL;
	root->inodeId=2;
	//root->users=1;
	time_t now;
	now=time(NULL);
	root->finode.latestCreateTime=now;
	root->finode.fileSize=0;
	root->finode.groupId=0;
	root->finode.linkNum=0;
	root->finode.power=14;
	root->finode.latestWriteTime=now;
	root->finode.userId=0;
	root->finode.latestReadTime=now;
	
	userlist->userNum=3;
	userlist->userlist[0].userId=1;
	userlist->userlist[0].groupId=1;
	strcpy(userlist->userlist[0].userName,"yql01");
	strcpy(userlist->userlist[0].userPassword,"123456");
	userlist->userlist[1].userId=2;
	userlist->userlist[1].groupId=1;
	strcpy(userlist->userlist[1].userName,"yql02");
	strcpy(userlist->userlist[1].userPassword,"123456");
	userlist->userlist[2].userId=3;
	userlist->userlist[2].groupId=2;
	strcpy(userlist->userlist[2].userName,"yql03");
	strcpy(userlist->userlist[2].userPassword,"123456");
	
	grouplist->groupNum=2;
	grouplist->grouplist[0].groupId=1;
	strcpy(grouplist->grouplist[0].groupName,"group01");
	grouplist->grouplist[1].groupId=2;
	strcpy(grouplist->grouplist[1].groupName,"group02");
	
	f=fopen(DISKPATH,"wb");
	int biockGroupNum=BLOCK_NUM/BLOCK_GROUP_SIZE;
	int LastBlockNum=BLOCK_NUM-biockGroupNum*BLOCK_GROUP_SIZE;
	
	fseek(f,DISK_SIZE,SEEK_SET);
	fwrite(" ",1,1,f);
	if(LastBlockNum==0){
		for(int i=0;i<BLOCK_GROUP_SIZE;i++)
			super->freeBlock[i]=BLOCK_START+BLOCK_GROUP_SIZE-1-i;
		super->nextFreeBlock=BLOCK_GROUP_SIZE-1;
		for(int j=0;j<biockGroupNum;j++){
			unsigned int blocksNum=BLOCK_GROUP_SIZE;
			unsigned int blocks[BLOCK_GROUP_SIZE];
			for(int k=0;k<BLOCK_GROUP_SIZE;k++)
				blocks[k]=LastBlockNum+BLOCK_START+BLOCK_GROUP_SIZE*(j+1)-k-1;
			fseek(f,BLOCK_SIZE*(LastBlockNum+BLOCK_START+BLOCK_GROUP_SIZE*j-1),SEEK_SET);
			fwrite(&blocksNum,sizeof(blocksNum),1,f);
			fwrite(&blocks,sizeof(blocks),1,f);
		}
		unsigned int blocksNum=0;
		unsigned int blocks[BLOCK_GROUP_SIZE];
		fseek(f,BLOCK_SIZE*(LastBlockNum+BLOCK_START+BLOCK_GROUP_SIZE*biockGroupNum-1),SEEK_SET);
		fwrite(&blocksNum,sizeof(blocksNum),1,f);
		fwrite(&blocks,sizeof(blocks),1,f);
	}
	else{
		for(int i=0;i<LastBlockNum;i++)
			super->freeBlock[i]=BLOCK_START+LastBlockNum-1-i;
		super->nextFreeBlock=LastBlockNum-1;
		for(int j=0;j<biockGroupNum;j++){
			unsigned int blocksNum=BLOCK_GROUP_SIZE;
			unsigned int blocks[BLOCK_GROUP_SIZE];
			for(int k=0;k<BLOCK_GROUP_SIZE;k++)
				blocks[k]=LastBlockNum+BLOCK_START+BLOCK_GROUP_SIZE*(j+1)-k-1;
			fseek(f,BLOCK_SIZE*(LastBlockNum+BLOCK_START+BLOCK_GROUP_SIZE*j-1),SEEK_SET);
			fwrite(&blocksNum,sizeof(blocksNum),1,f);
			fwrite(&blocks,sizeof(blocks),1,f);
		}
		unsigned int blocksNum=0;
		unsigned int blocks[BLOCK_GROUP_SIZE];
		fseek(f,BLOCK_SIZE*(LastBlockNum+BLOCK_START+BLOCK_GROUP_SIZE*biockGroupNum-1),SEEK_SET);
		fwrite(&blocksNum,sizeof(blocksNum),1,f);
		fwrite(&blocks,sizeof(blocks),1,f);
	}
	fseek(f,BLOCK_SIZE,SEEK_SET);
	fwrite(super,sizeof(superBlock),1,f);
	fwrite(userlist,sizeof(users),1,f);
	fwrite(grouplist,sizeof(groups),1,f);
	fseek(f,BLOCK_SIZE*root->inodeId,SEEK_SET);
	fwrite(&root->finode,sizeof(finode),1,f);
	fclose(f);
	dir* newDir=new dir();
	newDir->inodeId=2;
	strcpy(newDir->name,"root");
	curPath.push_back(newDir);
	superMkdir(root,"yql01",1,1);
	superMkdir(root,"yql02",2,1);
	superMkdir(root,"yql03",3,2);
}


void init(){
	FILE* f=fopen(DISKPATH,"rb");
	if(f==NULL)
		initGV(f);
	else{
		fseek(f,BLOCK_SIZE,SEEK_SET);
		fread(super,sizeof(superBlock),1,f);
		fread(userlist,sizeof(users),1,f);
		fread(grouplist,sizeof(groups),1,f);
		root->inodeId=2;
		root->parent=NULL;
		//root->users=3;
		fseek(f,BLOCK_SIZE*root->inodeId,SEEK_SET);
		fread(&root->finode,sizeof(finode),1,f);
		fclose(f);
		dir* newdir=new dir();
		newdir->inodeId=2;
		strcpy(newdir->name,"root");
		curPath.push_back(newdir);
		readCurDir();
	}
}

int login(){
	cout<<"please input your userName:";
	string userName;
	cin>>userName;
	cout<<"please input your password:";
	char password[MAX_NAME_SIZE]={0};
	int i=0;
	while(i<MAX_NAME_SIZE){
		/*password[i]=_getch();
		if(password[i]==13){
		password[i]='\0';
		break;
		}*/
		password[i]=getch();
		if((password[i]!=13)&&(password[i]!='\b')){
			i++; 
			cout<<'*';
			cout.flush();
		}
		else if((password[i]=='\b')&&(i>=1)){
			cout<<"\b"<<" "<<"\b";
			cout.flush();
			i--;
		}
		else if(password[i]==13){
			password[i]='\0'; 
			break;
		}
	}
	cout<<endl;
	char tempname[MAX_NAME_SIZE];
	strcpy(tempname,userName.c_str());
	//cout<<password<<endl;
	for(int j=0;j<userlist->userNum;j++)
		if(strcmp(tempname,userlist->userlist[j].userName)==0&&strcmp(password,userlist->userlist[j].userPassword)==0){
			curUser=&userlist->userlist[j];
			return STATUS_SUCCESS;
		}
		cout<<"UserName or Password Wrong!!!"<<endl;
		return login();
}

bool readInode(inode* i){
	FILE* f=fopen(DISKPATH,"rb");
	if(f==NULL)
		return false;
	else{
		fseek(f,BLOCK_SIZE*i->inodeId,SEEK_SET);
		fread(&i->finode,sizeof(finode),1,f);
		fclose(f);
		return true;
	}
}

bool writeInode(inode* i){
	FILE *f=fopen(DISKPATH,"rb+");
	if(f==NULL)
		return false;
	else{
		fseek(f,BLOCK_SIZE*i->inodeId,SEEK_SET);
		fwrite(&i->finode,sizeof(finode),1,f);
		fclose(f);
		return true;
	}
}

bool readDir(unsigned int blockId,dirs* curDir){
	FILE *f=fopen(DISKPATH,"rb");
	if(f==NULL)
		return false;
	else{
		fseek(f,BLOCK_SIZE*blockId,SEEK_SET);
		fread(curDir,sizeof(dirs),1,f);
		fclose(f);
		return true;
	}
}

bool writeDir(unsigned int blockId,dirs* curDir){
	FILE *f=fopen(DISKPATH,"rb+");
	if(f==NULL)
		return false;
	else{
		fseek(f,BLOCK_SIZE*blockId,SEEK_SET);
		fwrite(curDir,sizeof(dirs),1,f);
		fclose(f);
		return true;
	}
}

bool readNextBG(){
	FILE* f=fopen(DISKPATH,"rb");
	if(f==NULL)
		return false;
	else{
		fseek(f,BLOCK_SIZE*super->freeBlock[0],SEEK_SET);
		fread(&super->nextFreeBlock,sizeof(super->nextFreeBlock),1,f);
		fread(&super->freeBlock,sizeof(super->freeBlock),1,f);
		super->freeBlockNum--;
		super->nextFreeBlock--;
		fclose(f);
		writeSuperBlock();
		return true;
	}
}

bool writeSuperBlock(){
	FILE* f=fopen(DISKPATH,"rb+");
	if(f==NULL)
		return false;
	else{
		fseek(f,BLOCK_SIZE,SEEK_SET);
		fwrite(super,sizeof(superBlock),1,f);
		fclose(f);
		return true;
	}
}

int readContent(inode* newInode){
	FILE* f=fopen(DISKPATH,"rb");
	if(f==NULL)
		return STATUS_FILE_OPEN_ERROR;
	else{
		int int_blocks=newInode->finode.fileSize/BLOCK_SIZE;
		int remain_blocks=newInode->finode.fileSize%BLOCK_SIZE;
		char content[BLOCK_SIZE];
		int one=0,pos=0,pos1=0,pos2=0,temp=0;
		int blockId;
		if(int_blocks<4)
			temp=int_blocks;
		else
			temp=4;
		for(int i=0;i<temp;i++){
			fseek(f,BLOCK_SIZE*newInode->finode.address[i],SEEK_SET);
			//cout<<"newInode->finode.address[i]:"<<newInode->finode.address[i]<<endl;
			fread(content,BLOCK_SIZE,1,f);
			cout<<content;
			//cout<<"read 1~4:"<<blockId;
		}
		if(int_blocks>4 && int_blocks<106)
			temp=int_blocks;
		if(int_blocks>132)
			temp=132;
		for(int m=4;m<temp;m++){
			fseek(f,BLOCK_SIZE*newInode->finode.address[4]+pos1,SEEK_SET);
			fread(&blockId,sizeof(int),1,f); 
			fseek(f,BLOCK_SIZE*blockId,SEEK_SET);
			fread(content,BLOCK_SIZE,1,f);
			cout<<content;
			//cout<<"read 5~106:"<<blockId;
			pos1+=4;
		}
		cout<<endl;
		if(int_blocks>132)
			temp=int_blocks;
		for(int n=132;n<temp;n++){
			fseek(f,newInode->finode.address[5]+pos2,SEEK_SET);
			fread(&blockId,sizeof(int),1,f);
			fseek(f,BLOCK_SIZE*blockId,SEEK_SET);
			fread(&blockId,sizeof(int),1,f);
			fseek(f,BLOCK_SIZE*blockId,SEEK_SET);
			fread(content,BLOCK_SIZE,1,f);
			//cout<<content;
			pos2+=4;
		}

		if(remain_blocks>0){
			fseek(f,BLOCK_SIZE*newInode->finode.address[int_blocks],SEEK_SET);
			fread(content,remain_blocks,1,f);
			for(int i=0;i<remain_blocks;i++)
				cout<<content[i];
		}
		fclose(f);
		return STATUS_SUCCESS;
	}
}

int writeContent(inode* newInode,string text){
	FILE* f=fopen(DISKPATH,"rb+");
	if(f==NULL)
		return STATUS_FILE_OPEN_ERROR;
	else{
		int int_blocks=newInode->finode.fileSize/BLOCK_SIZE;	
		int remain_blocks=newInode->finode.fileSize%BLOCK_SIZE;
		int int_texts=text.size()/BLOCK_SIZE;
		int remain_texts=text.size()%BLOCK_SIZE;
		int pos=0,pos1=0,pos2=0,pos3=0,temp=0;
		
		if(int_blocks<=4){
			if(remain_blocks==0){
				for(int i=0;i<int_texts;i++){
					int blockId=getFreeBlock();
					if(blockId<0){
						fclose(f);
						return blockId;
					}
					else{
						newInode->finode.address[int_blocks++]=blockId;
						fseek(f,BLOCK_SIZE*blockId,SEEK_SET);
						fwrite(text.substr(pos,BLOCK_SIZE).c_str(),BLOCK_SIZE,1,f);
						newInode->finode.fileSize+=BLOCK_SIZE;
						pos+=BLOCK_SIZE;
					}
				}
				if(remain_texts>0){
					int blockId=getFreeBlock();
					if(blockId<0){
						fclose(f);
						return blockId;
					}
					else{
						newInode->finode.address[int_blocks++]=blockId;
						fseek(f,BLOCK_SIZE*blockId,SEEK_SET);
						fwrite(text.substr(pos,remain_texts).c_str(),BLOCK_SIZE,1,f);
						newInode->finode.fileSize+=remain_texts;
						pos+=remain_texts;
					}
					fclose(f);
					return STATUS_SUCCESS;
				}
				fclose(f);
				return STATUS_SUCCESS;
			}
			else{
				int left_block=BLOCK_SIZE-remain_blocks;
				if(text.size()<=left_block){
					fseek(f,BLOCK_SIZE*newInode->finode.address[int_blocks]+remain_blocks,SEEK_SET);
					fwrite(text.c_str(),text.size(),1,f);
					newInode->finode.fileSize+=text.size();
					fclose(f);
					return STATUS_SUCCESS;
				}
				else{
					fseek(f,BLOCK_SIZE*newInode->finode.address[int_blocks++]+remain_blocks,SEEK_SET);
					fwrite(text.c_str(),left_block,1,f);
					newInode->finode.fileSize+=left_block;
					int left_text=text.size()-left_block;
					int int_left_text_blocks=left_text/BLOCK_SIZE;
					int remain_left_text_blocks=left_text%BLOCK_SIZE;
					int tpos=left_block;
					
					for(int i=0;i<int_left_text_blocks;i++){
						int blockId=getFreeBlock();
						if(blockId<0){
							fclose(f);
							return blockId;
						}
						else{
							newInode->finode.address[int_blocks++]=blockId;
							fseek(f,BLOCK_SIZE*blockId,SEEK_SET);
							fwrite(text.substr(tpos,BLOCK_SIZE).c_str(),BLOCK_SIZE,1,f);
							newInode->finode.fileSize+=BLOCK_SIZE;
							tpos+=BLOCK_SIZE;
						}
					}
					if(remain_left_text_blocks>0){
						int blockId=getFreeBlock();
						if(blockId<0){
							fclose(f);
							return blockId;
						}
						else{
							newInode->finode.address[int_blocks++]=blockId;
							fseek(f,BLOCK_SIZE*blockId,SEEK_SET);
							fwrite(text.substr(tpos,remain_left_text_blocks).c_str(),BLOCK_SIZE,1,f);
							newInode->finode.fileSize+=remain_left_text_blocks;
							tpos+=remain_left_text_blocks;
						}
						fclose(f);
						return STATUS_SUCCESS;
					}
					fclose(f);
					return STATUS_SUCCESS;
				}
			}
		}

		if(int_texts>4){
			if(remain_texts>0)	
				int_texts++;
			if(int_texts<=132)
				temp=int_texts;
			if(int_texts>132)
				temp=132;
			int one=getFreeBlock();
			newInode->finode.address[4]=one;
			for(int m=4;m<temp;m++){
				int blockId=getFreeBlock();
				fseek(f,BLOCK_SIZE*one+pos1,SEEK_SET);
				
				fwrite(&blockId,sizeof(int),1,f);
				//cout<<"write 5~132:"<<&blockId;
				fseek(f,BLOCK_SIZE*blockId,SEEK_SET);
				fwrite(text.substr(pos,BLOCK_SIZE).c_str(),BLOCK_SIZE,1,f);
				newInode->finode.fileSize+=BLOCK_SIZE;
				pos1+=4;
				pos+=BLOCK_SIZE;
			}
			cout<<endl;

			if(int_texts>132)
				temp=int_texts;
			int two=getFreeBlock();
			newInode->finode.address[5]=two;
			for(int n=106;n<temp;n++){
				int blockId=getFreeBlock();
				fseek(f,BLOCK_SIZE*two+pos2,SEEK_SET);
				fwrite(&blockId,sizeof(int),1,f);
				fseek(f,BLOCK_SIZE*blockId,SEEK_SET);
				
				int three=getFreeBlock();
				fwrite(&blockId,sizeof(int),1,f);
				fseek(f,BLOCK_SIZE*blockId,SEEK_SET);
				fwrite(text.substr(pos,BLOCK_SIZE).c_str(),BLOCK_SIZE,1,f);
				newInode->finode.fileSize+=BLOCK_SIZE;
				pos2+=4;
				pos3+=4;
				pos+=BLOCK_SIZE;
			}
		}
		return STATUS_SUCCESS;
	}
}

int getFreeBlock(){
	if(super->freeBlockNum>0){
		if(super->nextFreeBlock>0){
			super->freeBlockNum--;
			super->nextFreeBlock--;
			writeSuperBlock();
			return super->freeBlock[super->nextFreeBlock+1];
		}
		else{
			readNextBG();
			return getFreeBlock();
		}
	}
	else
		return STATUS_NO_FREE_BLOCK;
}

int getFreeInode(){
	if(super->freeInodeNum>0){
		super->freeInodeNum--;
		super->nextFreeInode--;
		writeSuperBlock();
		return super->freeInode[super->nextFreeInode+1];
	}
	else
		return STATUS_NO_FREE_INODE;
}

string trim(string s){  
    if(s.empty())  
        return s;  
    s.erase(0,s.find_first_not_of(" "));  
    s.erase(s.find_last_not_of(" ") + 1);  
    return s;  
}

string getText(inode* newInode){
	string s;
	FILE* f=fopen(DISKPATH,"rb");
	if(f==NULL)
		return "";
	else{
		int int_blocks=newInode->finode.fileSize/BLOCK_SIZE;
		int remain_blocks=newInode->finode.fileSize%BLOCK_SIZE;
		char content[BLOCK_SIZE];
		for(int i=0;i<int_blocks;i++){
			fseek(f,BLOCK_SIZE*newInode->finode.address[i],SEEK_SET);
			fread(content,BLOCK_SIZE,1,f);
			s+=content;
		}
		if(remain_blocks>0){
			fseek(f,BLOCK_SIZE*newInode->finode.address[int_blocks],SEEK_SET);
			fread(content,remain_blocks,1,f);
			for(int i=0;i<remain_blocks;i++)
				s+=content[i];
		}
		fclose(f);
		return s;
	}
}

bool writeOS(){
	FILE* f=fopen(DISKPATH,"rb+");
	if(f==NULL)
		return false;
	else{
		fseek(f,BLOCK_SIZE+sizeof(superBlock),SEEK_SET);
		fwrite(userlist,sizeof(users),1,f);
		fclose(f);
		return true;
	}
}

int freeBlock(unsigned int blockId){
	if(super->nextFreeBlock<=18){
		super->freeBlockNum++;
		super->nextFreeBlock++;
		super->freeBlock[super->nextFreeBlock]=blockId;
		writeSuperBlock();
		return STATUS_SUCCESS;
	}
	else{
		FILE* f=fopen(DISKPATH,"rb+");
		if(f==NULL)
			return STATUS_FILE_OPEN_ERROR;
		else{
			unsigned int blocksNum=BLOCK_GROUP_SIZE;
			fseek(f,BLOCK_SIZE*(super->freeBlock[0]-BLOCK_GROUP_SIZE*1),SEEK_SET);
			fwrite(&blocksNum,sizeof(unsigned int),1,f);
			fwrite(&super->freeBlock,sizeof(super->freeBlock),1,f);
			fclose(f);
			super->freeBlockNum+=2;
			super->freeBlock[0]=super->freeBlock[0]-BLOCK_GROUP_SIZE*1;
			super->freeBlock[1]=blockId;
			super->nextFreeBlock=1;
			writeSuperBlock();
			return STATUS_SUCCESS;
		}
	}
}

int freeInode(unsigned int inodeId){
	super->freeInodeNum++;
	super->nextFreeInode++;
	super->freeInode[super->nextFreeInode]=inodeId;
	writeSuperBlock();
	return STATUS_SUCCESS;
}

void rmInode(unsigned short inodeId){
	inode* newInode=new inode();
	newInode->inodeId=inodeId;
	readInode(newInode);
	if(newInode->finode.linkNum>0){
		newInode->finode.linkNum--;
		writeInode(newInode);
		delete newInode;
	}
	else if(newInode->finode.power<8){
		freeInode(newInode->inodeId);
		if(newInode->finode.fileSize!=0){
			int int_blocks=newInode->finode.fileSize/BLOCK_SIZE;
			int remain_blocks=newInode->finode.fileSize%BLOCK_SIZE;
			for(int i=0;i<(remain_blocks>0?int_blocks+1:int_blocks);i++)
				freeBlock(newInode->finode.address[i]);
			delete newInode;
		}
	}
	else{
		freeInode(newInode->inodeId);
		if(newInode->finode.fileSize!=0){
			dirs* newdirs=new dirs();
			readDir(newInode->finode.address[0],newdirs);
			for(int i=0;i<newdirs->dirNum;i++)
				rmInode(newdirs->dir[i].inodeId);
			freeBlock(newInode->finode.address[0]);
			delete newdirs;
			delete newInode;
		}
	}
}

int passwd(){
	cout<<"please input your old password:";
	char password[MAX_NAME_SIZE]={0};
	for(int i=0;i<MAX_NAME_SIZE;i++){
		password[i]=_getch();
		if(password[i]==13){
			password[i]='\0';
			break;
		}
	}
	cout<<endl;
	if(strcmp(curUser->userPassword,password)!=0)
		return STATUS_PASSWORD_WRONG;
	else{
		cout<<"please input your new password:";
		char newpassword[MAX_NAME_SIZE]={0};
		for(int j=0;j<MAX_NAME_SIZE;j++){
			newpassword[j]=_getch();
			if(newpassword[j]==13){
				newpassword[j]='\0';
				break;
			}
		}
		cout<<endl;
		cout<<"please input confirm your new password:";
		char repassword[MAX_NAME_SIZE]={0};
		for(int k=0;k<MAX_NAME_SIZE;k++){
			repassword[k]=_getch();
			if(repassword[k]==13){
				repassword[k]='\0';
				break;
			}
		}
		cout<<endl;
		if(strcmp(newpassword,repassword)!=0)
			return STATUS_REPSWD_WRONG;
		else{
			strcpy(curUser->userPassword,newpassword);
			for(int q=0;q<userlist->userNum;q++)
				if(userlist->userlist[q].userName==curUser->userName){
					strcpy(userlist->userlist[q].userPassword,newpassword);
					writeOS();
					return STATUS_SUCCESS;
				}
		}
	}
	return STATUS_ERROR;
}

void clear(){
	system("cls");
}

void exit(){
	cout << "*** 当前用户正在注销 ***" << endl;
	Sleep(3000);
	system("cls");
	//init();
	FILE* f=fopen(DISKPATH,"rb");
	if(f==NULL)
		initGV(f);
	else{
		fseek(f,BLOCK_SIZE,SEEK_SET);
		fread(super,sizeof(superBlock),1,f);
		fread(userlist,sizeof(users),1,f);
		fread(grouplist,sizeof(groups),1,f);
		root->inodeId=2;
		root->parent=NULL;
		//root->users=3;
		fseek(f,BLOCK_SIZE*root->inodeId,SEEK_SET);
		fread(&root->finode,sizeof(finode),1,f);
		fclose(f);
		dir* newdir=new dir();
		newdir->inodeId=2;
		strcpy(newdir->name,"root");
		//curPath.push_back(newdir);
		readCurDir();
	}
	if(login()==STATUS_SUCCESS){
		menu();
		choose();
	}
}

void help(string cmd){
	if(cmd == "chmod")
		printf("*** 改变文件权限，格式为chmod [power] [filename/dirname] ***");
	else if(cmd == "chown")
		printf("*** 改变文件拥有者，格式为chown [user] [filename] ***");
	else if(cmd == "chgrp")
		printf("*** 改变文件所属组，格式为chgrp [group] [filename] ***");
	else if(cmd == "pwd")
		printf("*** 显示当前目录，格式为pwd ***");
	else if(cmd == "cd")
		printf("*** 更改当前目录，格式为 cd [../ or ,/ or dirname] ***");
	else if(cmd == "mkdir")
		printf("*** 创建新目录，格式为 mkdir [dirname] ***");
	else if(cmd == "rmdir")
		printf("*** 删除目录，格式为 rmdir [dirname] ***");
	else if(cmd == "mv")
		printf("*** 重命名文件，格式为 mv [filename1] [filename2]***");	
	else if(cmd == "cp")
		printf("*** 复制文件，格式为 cp [filename1] [filename2] ***");
	else if(cmd == "rm")
		printf("*** 删除文件，格式为 rm [filename] ***");
	else if(cmd == "ln")
		printf("*** 链接文件，格式为 ln [filename1] [filename2] ***");
	else if(cmd == "cat")
		printf("*** 显示文件内容，格式为 cat [filename] ***");
	else if(cmd == "passwd")
		printf("*** 修改用户口令，格式为 passwd ***");
	else if(cmd == "touch")
		printf("*** 创建新文件，格式为 touch [filename] ***");
	else if(cmd == ">>")
		printf("*** 文本内容追加，格式为 >> [filename] ***");
	else if(cmd == "vi")
		printf("*** 编辑文件，格式为 vi [filename] ***");
	else if(cmd == "super")
		printf("*** 查看超级块状态，格式为 super ***");
	else if(cmd == "ls")
		printf("*** 显示目录内容，格式为ls ***");
	else if(cmd == "clear")
		printf("*** 清屏操作，格式为clear ***");
	else if(cmd == "exit")
		printf("*** 注销操作，格式为exit ***");			
	cout << endl; 
}

int readCurDir(){
	FILE* f=fopen(DISKPATH,"rb");
	if(f==NULL)
		return STATUS_FILE_OPEN_ERROR;
	else{
		if(curInode->finode.fileSize==0)
			curDir->dirNum=0;
		else{
			fseek(f,BLOCK_SIZE*curInode->finode.address[0],SEEK_SET);
			fread(curDir,sizeof(dirs),1,f);
			fclose(f);
		}
		return STATUS_SUCCESS;
	}
}

int superMkdir(inode* parent,char name[MAX_NAME_SIZE],unsigned short userId,unsigned short groupId){
	bool exist=checkFileName(name);
	if(parent->finode.fileSize==0){
		int blockId=getFreeBlock();
		if(blockId<0)
			return blockId;
		else{
			int inodeId=getFreeInode();
			if(inodeId<0)
				return inodeId;
			else{
				parent->finode.fileSize=sizeof(dirs);
				time_t now;
				now=time(NULL);
				parent->finode.latestWriteTime=now;
				parent->finode.address[0]=blockId;
				writeInode(parent);
				dir* newdir=new dir();
				strcpy(newdir->name,name);
				newdir->inodeId=inodeId;
				finode* newFinode=new finode();
				newFinode->latestCreateTime=now;
				newFinode->fileSize=0;
				newFinode->groupId=groupId;
				newFinode->linkNum=0;
				newFinode->power=14;
				newFinode->latestWriteTime=now;
				newFinode->userId=userId;
				newFinode->latestReadTime=now;
				inode newInode;
				newInode.finode=*newFinode;
				newInode.inodeId=inodeId;
				newInode.parent=parent;
				//newInode.users=0;
				writeInode(&newInode);
				dirs newDirs;
				newDirs.dirNum=1;
				newDirs.dir[0]=*newdir;
				writeDir(blockId,&newDirs);
				//delete newDirs;
				*curDir=newDirs;
				delete newFinode;
				delete newdir;
				return STATUS_SUCCESS;
			}
		}
	}
	else
		if(parent->finode.fileSize!=0){
			int inodeId=getFreeInode();
			if(inodeId<0)
				return inodeId;
			else{
				time_t now;
				now=time(NULL);
				parent->finode.latestWriteTime=now;
				dir *newDir=new dir();
				strcpy(newDir->name,name);
				newDir->inodeId=inodeId;
				finode *newFinode=new finode();
				newFinode->latestCreateTime=now;
				newFinode->fileSize=0;
				newFinode->groupId=groupId;
				newFinode->linkNum=0;
				newFinode->power=14;
				newFinode->latestWriteTime=now;
				newFinode->userId=userId;
				newFinode->latestReadTime=now;
				inode newInode;
				newInode.finode=*newFinode;
				newInode.inodeId=inodeId;
				newInode.parent=parent;
				//newInode.users=0;
				writeInode(&newInode);
				dirs newDirs;
				readDir(parent->finode.address[0],&newDirs);
				if(newDirs.dirNum<MAX_DIRECT_NUM){
					newDirs.dir[newDirs.dirNum]=*newDir;
					newDirs.dirNum++;
					writeDir(parent->finode.address[0],&newDirs);
					*curDir=newDirs;
					delete newFinode;
					delete newDir;
					return STATUS_SUCCESS;
				}
				else
					return STATUS_BEYOND_DIRECT_NUM;
			}
		}
		return STATUS_ERROR;
}

int chmod(char name[MAX_NAME_SIZE],unsigned int power){
	for(int i=0;i<curDir->dirNum;i++){
		if(strcmp(curDir->dir[i].name,name)==0){
			if(power==8||power==9||power==11||power==13||power>14)
				return STATUS_MOD_ERROR;
			inode* newInode=new inode();
			newInode->inodeId=curDir->dir[i].inodeId;
			readInode(newInode);
			if(newInode->finode.userId!=curUser->userId&&curUser->userId!=ROOT){
				delete newInode;
				return STATUS_BEYOND_RIGHT;
			}
			if((power<8&&newInode->finode.power<8)||(power>9&&newInode->finode.power>9)){
				newInode->finode.power=power;
				writeInode(newInode);
				delete newInode;
				return STATUS_SUCCESS;
			}
			delete newInode;
			return STATUS_BEYOND_RIGHT;
		}
	}
	return STATUS_FILENAME_NOTEXIST;
}

int chown(char name[MAX_NAME_SIZE],unsigned short userId){
	for(int i=0;i<curDir->dirNum;i++){
		if(strcmp(curDir->dir[i].name,name)==0){
			for(int j=0;j<userlist->userNum;j++){
				if(userId==userlist->userlist[j].userId){
					inode* newInode=new inode();
					newInode->inodeId=curDir->dir[i].inodeId;
					readInode(newInode);
					if(curUser->userId==newInode->finode.userId||curUser->userId==ROOT){
						newInode->finode.userId=userId;
						writeInode(newInode);
						delete newInode;
						return STATUS_SUCCESS;
					}
					else {
						delete newInode;
						return STATUS_BEYOND_RIGHT;
					}
				}
			}
			return STATUS_USER_NOTEXIST;
		}
	}
	return STATUS_FILENAME_NOTEXIST;
}

int chgrp(char name[MAX_NAME_SIZE],unsigned short groupId){
	for(int i=0;i<curDir->dirNum;i++){
		if(strcmp(curDir->dir[i].name,name)==0){
			for(int j=0;j<grouplist->groupNum;j++){
				if(groupId==grouplist->grouplist[j].groupId){
					inode* newInode=new inode();
					newInode->inodeId=curDir->dir[i].inodeId;
					readInode(newInode);
					if(curUser->userId==newInode->finode.userId||curUser->userId==ROOT){
						newInode->finode.groupId=groupId;
						writeInode(newInode);
						cout<<"ok";
						delete newInode;
						return STATUS_SUCCESS;
					}
					else {
						delete newInode;
						return STATUS_BEYOND_RIGHT;
					}
				}
			}
			return STATUS_GROUP_NOTEXIST;
		}
	}
	return STATUS_FILENAME_NOTEXIST;
}

bool whetherContainsIllegalCharacters(string name){
	int i, len;
	bool flag = true;
	len = name.length(); 
	for(i = 0; i < len; i++){
		if(name[i] == '/' || name[i] == '\\' || name[i] == ':' ||
			name[i] == '<' || name[i] == '>' || name[i] == '|'){
			flag = false;
			break;
		}
	}
	return flag;
}

int mv(char oldName[MAX_NAME_SIZE],char newName[MAX_NAME_SIZE]){
	if(!whetherContainsIllegalCharacters(newName))
		return STATUS_FILENAME_Illegal;
	for(int j=0;j<curDir->dirNum;j++)
		if(strcmp(curDir->dir[j].name,newName)==0)
			return STATUS_FILENAME_EXIST;
		for(int i=0;i<curDir->dirNum;i++){
			if(strcmp(curDir->dir[i].name,oldName)==0){
				inode* newInode=new inode();
				newInode->inodeId=curDir->dir[i].inodeId;
				readInode(newInode);
				if(newInode->finode.userId==curUser->userId||curUser->userId==ROOT){
					strcpy(curDir->dir[i].name,newName);
					writeDir(curInode->finode.address[0],curDir);
					return STATUS_SUCCESS;
				}
				else return STATUS_BEYOND_RIGHT;
				delete newInode;
			}
		}
		return STATUS_FILENAME_NOTEXIST;
}

int cp(char source[MAX_NAME_SIZE],char target[MAX_NAME_SIZE]){
	if(strcmp(source,target)==0)	
		return STATUS_SOURCE_AND_TARGET_ERROR;
	else{
		for(int i=0;i<curDir->dirNum;i++){
			if(strcmp(curDir->dir[i].name,source)==0){
				inode* newInode=new inode();
				newInode->inodeId=curDir->dir[i].inodeId;
				readInode(newInode);
				if(newInode->finode.power<4){
					delete newInode;
					return STATUS_READONLY;
				}
				if(newInode->finode.power>7){
					delete newInode;
					return STATUS_NOT_FILE;
				}
				if(newInode->finode.userId!=curUser->userId&&newInode->finode.userId!=ROOT){
					delete newInode;
					return STATUS_BEYOND_RIGHT;
				}
				for(int j=0;j<curDir->dirNum;j++){
					if(strcmp(curDir->dir[j].name,target)==0){
						int flag=rm(target);
						if(flag!=STATUS_SUCCESS){
							delete newInode;
							return flag;
						}
					}
				}
				int flag2=touch(curInode,target);
				if(flag2!=STATUS_SUCCESS){
					delete newInode;
					return flag2;
				}
				for(int k=0;k<curDir->dirNum;k++){
					if(strcmp(curDir->dir[k].name,target)==0){
						inode* tempInode=new inode();
						tempInode->inodeId=curDir->dir[k].inodeId;
						readInode(tempInode);
						string content=getText(newInode);
						writeContent(tempInode,content);
						writeInode(tempInode);
						delete tempInode;
						delete newInode;
						return STATUS_SUCCESS;
					}
				}
			}
		}	
		return STATUS_FILENAME_NOTEXIST;
	}
}

int rm(char name[MAX_NAME_SIZE]){
	for(int i=0;i<curDir->dirNum;i++){
		if(strcmp(curDir->dir[i].name,name)==0){
			inode* newInode=new inode();
			newInode->inodeId=curDir->dir[i].inodeId;
			readInode(newInode);
			if(newInode->finode.power<6){
				delete newInode;
				return STATUS_READONLY;
			}
			if(newInode->finode.power>7){
				delete newInode;
				return STATUS_NOT_FILE;
			}
			if(newInode->finode.userId!=curUser->userId&&curUser->userId!=ROOT){
				delete newInode;
				return STATUS_BEYOND_RIGHT;
			}
			if(newInode->finode.linkNum>0){
				newInode->finode.linkNum--;
				writeInode(newInode);
			}
			else{
				int int_blocks=newInode->finode.fileSize/BLOCK_SIZE;
				int remain_blocks=newInode->finode.fileSize%BLOCK_SIZE;
				if(newInode->finode.fileSize>0){
					for(int m=0;m<(remain_blocks>0?int_blocks+1:int_blocks);m++)
						freeBlock(newInode->finode.address[m]);	
				}
				freeInode(newInode->inodeId);
			}
			dirs* newdirs=new dirs();
			newdirs->dirNum=curDir->dirNum-1;
			int k=0;
			for(int j=0;j<curDir->dirNum;j++)
				if(j!=i)
					newdirs->dir[k++]=curDir->dir[j];
				writeDir(curInode->finode.address[0],newdirs);
				dirs* tempdirs=curDir;
				curDir=newdirs;
				delete newInode;
				delete tempdirs;
				return STATUS_SUCCESS;
		}		
	}
	return STATUS_FILENAME_NOTEXIST;
}

int cat(char name[MAX_NAME_SIZE]){
	for(int i=0;i<curDir->dirNum;i++){
		if(strcmp(curDir->dir[i].name,name)==0){
			inode* newInode=new inode();
			newInode->inodeId=curDir->dir[i].inodeId;
			readInode(newInode);
			if(newInode->finode.power<4)
				return STATUS_WRITEONLY;
			if(newInode->finode.power>7)
				return STATUS_NOT_FILE;
			if(newInode->finode.userId!=curUser->userId&&curUser->userId!=ROOT)
				return STATUS_BEYOND_RIGHT;
			int content=readContent(newInode);
			delete newInode;
			return content;
		}
	}	
	return STATUS_FILENAME_NOTEXIST;
}

int ln(char source[MAX_NAME_SIZE],char target[MAX_NAME_SIZE]){
	if(strcmp(source,target)==0)
		return STATUS_SOURCE_AND_TARGET_ERROR;
	for(int i=0;i<curDir->dirNum;i++){				
		if(strcmp(curDir->dir[i].name,source)==0){
			inode* newInode=new inode();
			newInode->inodeId=curDir->dir[i].inodeId;
			readInode(newInode);
			if(newInode->finode.userId!=curUser->userId&&curUser->userId!=ROOT){
				delete newInode;
				return STATUS_BEYOND_RIGHT;
			}
			for(int j=0;j<curDir->dirNum;j++){
				if(strcmp(curDir->dir[j].name,target)==0){
					inode* tempInode=new inode();
					tempInode->inodeId=curDir->dir[j].inodeId;
					readInode(tempInode);
					if(tempInode->finode.userId!=curUser->userId&&curUser->userId!=ROOT){
						delete tempInode;
						delete newInode;
						return STATUS_BEYOND_RIGHT;
					}
					if(tempInode->finode.power<8&&newInode->finode.power>7 || tempInode->finode.power>7&&newInode->finode.power<8){
						delete tempInode;
						delete newInode;
						return STATUS_TYPE_NOT_MATCH;
					}
					rmInode(curDir->dir[j].inodeId);
					curDir->dir[j].inodeId=curDir->dir[i].inodeId;
					writeDir(curInode->finode.address[0],curDir);
					delete tempInode;
					delete newInode;
					return STATUS_SUCCESS;
				}
			}
			dir newdir;
			newdir.inodeId=curDir->dir[i].inodeId;
			strcpy(newdir.name,target);
			curDir->dir[curDir->dirNum]=newdir;
			curDir->dirNum++;
			writeDir(curInode->finode.address[0],curDir);
			delete newInode;
			return STATUS_SUCCESS;
		}
	}
	return STATUS_FILENAME_NOTEXIST;
}

bool checkFileName(char name[MAX_NAME_SIZE]){
	FILE* file=fopen(DISKPATH,"rb");
	if(file==NULL)
		return true;
	else{
		dirs curDir;
		fseek(file,BLOCK_SIZE* curInode->finode.address[0],SEEK_SET);
		fread(&curDir,sizeof(dirs),1,file);
		fclose(file);
		for(int i=0;i<curDir.dirNum;i++)
			if(strcmp(curDir.dir[i].name,name)==0)
				return true;
			return false;
	}
}

int touch(inode* parent,char name[MAX_NAME_SIZE]){
	bool exist=checkFileName(name);
	if(exist)
		return STATUS_FILENAME_EXIST;
	if(parent->finode.userId!=curUser->userId&&curUser->userId!=ROOT)
		return STATUS_BEYOND_RIGHT;
	if(parent->finode.power==1 || parent->finode.power==4 || parent->finode.power==5 || parent->finode.power==12)
		return STATUS_CANNOT_WRITE;
	if(parent->finode.fileSize==0){
		int blockId=getFreeBlock();
		if(blockId<0)
			return blockId;
		else{
			int inodeId=getFreeInode();
			if(inodeId<0)
				return inodeId;
			else{
				parent->finode.fileSize=sizeof(dirs);
				time_t now=time(NULL);
				parent->finode.latestWriteTime=now;
				parent->finode.address[0]=blockId;
				writeInode(parent);
				dir* newdir=new dir();
				strcpy(newdir->name,name);
				newdir->inodeId=inodeId;
				finode* newfinode=new finode();
				newfinode->fileSize=0;
				newfinode->userId=parent->finode.userId;
				newfinode->groupId=parent->finode.groupId;
				newfinode->linkNum=0;
				newfinode->power=6;
				newfinode->latestCreateTime=now;
				newfinode->latestWriteTime=now;
				newfinode->latestReadTime=now;
				inode newinode;
				newinode.finode=*newfinode;
				newinode.inodeId=inodeId;
				newinode.parent=parent;
				//newinode.users=parent->users;
				writeInode(&newinode);
				dirs newdirs;
				newdirs.dirNum=1;
				newdirs.dir[0]=*newdir;
				writeDir(blockId,&newdirs);
				*curDir=newdirs;
				delete newfinode;
				delete newdir;
				return STATUS_SUCCESS;
			}
		}
	}
	else if(parent->finode.fileSize!=0){
		int inodeId=getFreeInode();
		if(inodeId<0)
			return inodeId;
		else{
			time_t now;
			now=time(NULL);
			parent->finode.latestWriteTime=now;
			dir* newdir=new dir();
			strcpy(newdir->name,name);
			newdir->inodeId=inodeId;
			finode* newfinode=new finode();
			newfinode->fileSize=0;
			newfinode->userId=parent->finode.userId;
			newfinode->groupId=parent->finode.groupId;
			newfinode->linkNum=0;
			newfinode->power=6;
			newfinode->latestCreateTime=now;
			newfinode->latestWriteTime=now;
			newfinode->latestReadTime=now;
			inode newinode;
			newinode.finode=*newfinode;
			newinode.inodeId=inodeId;
			newinode.parent=parent;
			//newinode.users=parent->users;
			writeInode(&newinode);
			dirs newdirs;
			readDir(parent->finode.address[0],&newdirs);
			if(newdirs.dirNum<MAX_DIRECT_NUM){
				newdirs.dir[newdirs.dirNum]=*newdir;
				newdirs.dirNum++;
				writeDir(parent->finode.address[0],&newdirs);
				*curDir=newdirs;
				delete newfinode;
				delete newdir;
				return STATUS_SUCCESS;
			}
			else
				return STATUS_BEYOND_DIRECT_NUM;
		}
	}
	else 
		return STATUS_ERROR;
}

int textAppend(char name[MAX_NAME_SIZE]){
	for(int i=0;i<curDir->dirNum;i++){
		if(strcmp(curDir->dir[i].name,name)==0){
			inode* newInode=new inode();
			newInode->inodeId=curDir->dir[i].inodeId;
			readInode(newInode);
			if(newInode->finode.power==1||newInode->finode.power==4||newInode->finode.power==5){
				delete newInode;
				return STATUS_CANNOT_WRITE;
			}
			if(newInode->finode.power>7){
				delete newInode;
				return STATUS_NOT_FILE;
			}
			if(newInode->finode.userId!=curUser->userId&&curUser->userId!=ROOT)
				return STATUS_BEYOND_RIGHT;
			string s;
			getline(cin,s,char(17));
			if(s.size()+newInode->finode.fileSize>FILE_MAX_SIZE){
				delete newInode;
				return STATUS_BEYOND_SIZE;
			}
			int flag=writeContent(newInode,s);
			writeInode(newInode);
			delete newInode;
			return flag;
		}
	}
	return STATUS_FILENAME_NOTEXIST;
}

int vi(char name[MAX_NAME_SIZE]){
	for(int i=0;i<curDir->dirNum;i++){
		if(strcmp(curDir->dir[i].name,name)==0){
			inode* newInode=new inode();
			newInode->inodeId=curDir->dir[i].inodeId;
			readInode(newInode);
			if(newInode->finode.power<4)
				return STATUS_WRITEONLY;
			if(newInode->finode.power>7)
				return STATUS_NOT_FILE;
			if(newInode->finode.power==1||newInode->finode.power==4||newInode->finode.power==5){
				delete newInode;
				return STATUS_CANNOT_WRITE;
			}
			if(newInode->finode.userId!=curUser->userId&&curUser->userId!=ROOT)
				return STATUS_BEYOND_RIGHT;
			int content=readContent(newInode);
			string s;
			getline(cin,s);
			if(s.size()+newInode->finode.fileSize>FILE_MAX_SIZE){
				delete newInode;
				return STATUS_BEYOND_SIZE;
			}
			int flag=writeContent(newInode,s);
			writeInode(newInode);
			delete newInode;
			return flag;
		}
	}
	return STATUS_FILENAME_NOTEXIST;
}

string ls(){
	string s;
	for(int i=0;i<curDir->dirNum;i++)
		s=s+curDir->dir[i].name+" ";
	return s;
}

string pwd(){
	string s;
	for(int i=0;i<curPath.size();i++)
		s=s+curPath[i]->name+"/";
	return s;
}

int cd(char name[MAX_NAME_SIZE]){
	if(strcmp(name,"./")==0)
		return STATUS_SUCCESS;
	if(strcmp(name,"../")==0){
		if(curInode->parent!=NULL){
			inode* newInode=curInode;
			curInode=curInode->parent;
			delete newInode;
			dir* newDir=curPath[curPath.size()-1];
			curPath.pop_back();
			delete newDir;
			readCurDir();
		}
		return STATUS_SUCCESS;
	}
	for(int i=0;i<curDir->dirNum;i++){
		if(strcmp(curDir->dir[i].name,name)==0){
			dir* newDir=new dir();
			*newDir=curDir->dir[i];
			inode* newInode=new inode();
			newInode->inodeId=newDir->inodeId;
			newInode->parent=curInode;
			//newInode->users=curInode->users;
			readInode(newInode);
			if(newInode->finode.power>7){
				curInode=newInode;
				curPath.push_back(newDir);
				readCurDir();
				return STATUS_SUCCESS;
			}
			else{
				delete newDir;
				delete newInode;
				return STATUS_NOT_DIRECT;
			}
		}
	}
	return STATUS_FILENAME_NOTEXIST;
}

int mkdir(inode* parent,char name[MAX_NAME_SIZE]){
	if(checkFileName(name))
		return STATUS_FILENAME_EXIST;
	if(parent->finode.userId!=curUser->userId&&curUser->userId!=ROOT)
		return STATUS_BEYOND_RIGHT;
	if(parent->finode.power==12)
		return STATUS_CANNOT_WRITE;
	if(parent->finode.fileSize==0){
		int blockId=getFreeBlock();
		if(blockId<0)
			return blockId;
		else{
			int inodeId=getFreeInode();
			if(inodeId<0)
				return inodeId;
			else{
				parent->finode.fileSize=sizeof(dirs);
				time_t now=time(NULL);
				parent->finode.latestWriteTime=now;
				parent->finode.address[0]=blockId;
				writeInode(parent);
				dir* newDir=new dir();
				strcpy(newDir->name,name);
				newDir->inodeId=inodeId;
				finode* newFinode=new finode();
				newFinode->fileSize=0;
				newFinode->power=14;
				newFinode->linkNum=0;
				newFinode->userId=parent->finode.userId;
				newFinode->groupId=parent->finode.groupId;
				newFinode->latestCreateTime=now;
				newFinode->latestReadTime=now;
				newFinode->latestWriteTime=now;
				inode newInode;
				newInode.finode=*newFinode;
				newInode.inodeId=inodeId;
				newInode.parent=parent;
				//newInode.users=parent->users;
				writeInode(&newInode);
				dirs newDirs;
				newDirs.dirNum=1;
				newDirs.dir[0]=*newDir;
				writeDir(blockId,&newDirs);
				*curDir=newDirs;
				delete newFinode;
				delete newDir;
				return STATUS_SUCCESS;
			}
		}
	}
	else{
		int inodeId=getFreeInode();
		if(inodeId<0)
			return inodeId;
		else{
			time_t now=time(NULL);
			parent->finode.latestWriteTime=now;
			dir* newDir=new dir();
			strcpy(newDir->name,name);
			newDir->inodeId=inodeId;
			finode* newFinode=new finode();
			newFinode->fileSize=0;
			newFinode->power=14;
			newFinode->linkNum=0;
			newFinode->userId=parent->finode.userId;
			newFinode->groupId=parent->finode.groupId;
			newFinode->latestCreateTime=now;
			newFinode->latestReadTime=now;
			newFinode->latestWriteTime=now;
			inode newInode;
			newInode.finode=*newFinode;
			newInode.inodeId=inodeId;
			newInode.parent=parent;
			//newInode.users=parent->users;
			writeInode(&newInode);
			dirs newDirs;
			readDir(parent->finode.address[0],&newDirs);
			if(newDirs.dirNum<MAX_DIRECT_NUM){
				newDirs.dir[newDirs.dirNum]=*newDir;
				newDirs.dirNum++;
				writeDir(parent->finode.address[0],&newDirs);
				*curDir=newDirs;
				delete newFinode;
				delete newDir;
				return STATUS_SUCCESS;
			}
			else
				return STATUS_BEYOND_DIRECT_NUM;
		}
	}
	return STATUS_ERROR;
}

int rmdir(char name[MAX_NAME_SIZE]){
	for(int i=0;i<curDir->dirNum;i++){
		if(strcmp(curDir->dir[i].name,name)==0){
			inode* newInode=new inode();
			newInode->inodeId=curDir->dir[i].inodeId;
			readInode(newInode);
			if(newInode->finode.power<8){
				delete newInode;
				return STATUS_NOT_DIRECT;
			}
			if(newInode->finode.userId!=curUser->userId&&curUser->userId!=ROOT){
				delete newInode;
				return STATUS_BEYOND_RIGHT;
			}
			rmInode(curDir->dir[i].inodeId);
			dirs* newDirs=new dirs();
			newDirs->dirNum=curDir->dirNum-1;
			int k=0;
			for(int j=0;j<curDir->dirNum;j++)
				if(j!=i)
					newDirs->dir[k++]=curDir->dir[j];
			writeDir(curInode->finode.address[0],newDirs);
			dirs* tempDirs=curDir;
			curDir=newDirs;
			delete newInode;
			delete tempDirs;
			return STATUS_SUCCESS;
		}
	}
	return STATUS_FILENAME_NOTEXIST;
}

void choose(){
	cout<<curUser->userName<<"@UnixFileSystem:~"<<pwd()<<"$";
	char c[20];
	cin.getline(c,20,'\n');
	while(c[0]==NULL)
		cin.getline(c,100,'\n');
	string command=c;
	command=trim(command);
	int flag=-1;
	int subPos=command.find_first_of(" ");
	if(subPos==-1){
		if(command=="ls")
			flag=1;
		else if(command=="pwd")
			flag=5;
		else if(command=="passwd")
			flag=14;
		else if(command=="super")
			flag=17;
		else if(command=="clear")
			flag=19;
		else if(command=="exit")
			flag=20;
	}
	else{
		string c=command.substr(0,subPos);
		int d=command.find_first_of("help");
		if(c=="chmod")
			flag=2;
		else if(c=="chown")
			flag=3;
		else if(c=="chgrp")
			flag=4;
		else if(c=="cd")
			flag=6;
		else if(c=="mkdir")
			flag=7;
		else if(c=="rmdir")
			flag=8;
		else if(c=="mv")
			flag=9;
		else if(c=="cp")
			flag=10;
		else if(c=="rm")
			flag=11;
		else if(c=="ln")
			flag=12;
		else if(c=="cat")
			flag=13;
		else if(c=="touch")
			flag=15;
		else if(c==">>")
			flag=16;
		else if(c=="vi")
			flag=18;
		else if(d!=-1)
			flag=21;
	}
	switch(flag){
	case 1:{
		cout<<ls()<<endl;
		break;
		   };
	case 2:{
		string pattern=command.substr(subPos+1);
		pattern=trim(pattern);
		int subPos2=pattern.find_first_of(" ");
		if(subPos2==-1)
			cout<<"Error Pattern..."<<endl;
		else{
			string power=pattern.substr(0,subPos2);
			string fileName=pattern.substr(subPos2+1);
			fileName=trim(fileName);
			int subPos3=fileName.find_first_of(" ");
			if(subPos3!=-1)
				cout<<"Error pattern..."<<endl;
			else{
				unsigned short m=atoi(power.c_str());
				char name[MAX_NAME_SIZE];
				strcpy(name,fileName.c_str());
				int result=chmod(name,m);
				if(result==STATUS_MOD_ERROR)
					cout<<"Error: Pattern is illegal..."<<endl;
				else if(result==STATUS_FILENAME_NOTEXIST)
					cout<<"Error: FileName is nonexisted..."<<endl;
				else if(result==STATUS_BEYOND_RIGHT)
					cout<<"Error: Beyond Your Right..."<<endl;
			}
			
		}
		break;
		   };
	case 3:{
		string pattern=command.substr(subPos+1);
		pattern=trim(pattern);
		int subPos2=pattern.find_first_of(" ");
		if(subPos2==-1)
			cout<<"Error Pattern..."<<endl;
		else{
			string userId=pattern.substr(0,subPos2);
			string fileName=pattern.substr(subPos2+1);
			fileName=trim(fileName);
			int subPos3=fileName.find_first_of(" ");
			if(subPos3!=-1)
				cout<<"Error pattern..."<<endl;
			else{
				unsigned short oi=atoi(userId.c_str());
				char name[MAX_NAME_SIZE];
				strcpy(name,fileName.c_str());
				int result=chown(name,oi);
				//int result=chown(fileName,userId);
				//cout<<name<<endl;
				//cout<<oi<<endl;
				if(result==STATUS_USER_NOTEXIST)
					cout<<"Error: userId is nonexisted..."<<endl;
				else if(result==STATUS_FILENAME_NOTEXIST)
					cout<<"Error: FileName is nonexisted..."<<endl;
				else if(result==STATUS_BEYOND_RIGHT)
					cout<<"Error: Beyond Your Right..."<<endl;
			}
			
		}
		break;
		   };
	case 4:{
		string pattern=command.substr(subPos+1);
		pattern=trim(pattern);
		int subPos2=pattern.find_first_of(" ");
		if(subPos2==-1)
			cout<<"Error Pattern..."<<endl;
		else{
			string groupId=pattern.substr(0,subPos2);
			string fileName=pattern.substr(subPos2+1);
			fileName=trim(fileName);
			int subPos3=fileName.find_first_of(" ");
			if(subPos3!=-1)
				cout<<"Error pattern..."<<endl;
			else{
				unsigned short gi=atoi(groupId.c_str());
				char name[MAX_NAME_SIZE];
				strcpy(name,fileName.c_str());
				int result=chgrp(name,gi);
				if(result==STATUS_GROUP_NOTEXIST)
					cout<<"Error: GroupId is nonexisted..."<<endl;
				else if(result==STATUS_FILENAME_NOTEXIST)
					cout<<"Error: FileName is nonexisted..."<<endl;
				else if(result==STATUS_BEYOND_RIGHT)
					cout<<"Error: Beyond Your Right..."<<endl;
			}
			
		}
		break;
		   };
	case 5:{
		cout<<pwd()<<endl;
		break;
		   };
	case 6:{
		string fileName=command.substr(subPos+1);
		fileName=trim(fileName);
		int subPos2=fileName.find_first_of(" ");
		if(subPos2!=-1)
			cout<<"Error DirectName..."<<endl;
		else{
			char name[MAX_NAME_SIZE];
			strcpy(name,fileName.c_str());
			int result=cd(name);
			if(result==STATUS_FILENAME_NOTEXIST)
				cout<<"Error: Dir is not existed..."<<endl;
			else if(result==STATUS_NOT_DIRECT)
				cout<<"Error: Not a direct..."<<endl;
		}
		break;
		   };
	case 7:{
		string fileName=command.substr(subPos+1);
		fileName=trim(fileName);
		int subPos2=fileName.find_first_of(" ");
		if(subPos2!=-1)
			cout<<"Error FileName..."<<endl;
		else{
			char name[MAX_NAME_SIZE];
			strcpy(name,fileName.c_str());
			int result=mkdir(curInode,name);
			if(result==STATUS_NO_FREE_BLOCK)
				cout<<"Error: No free block..."<<endl;
			else if(result==STATUS_NO_FREE_INODE)
					cout<<"Error: No free inode..."<<endl;
				else if(result==STATUS_BEYOND_DIRECT_NUM)
						cout<<"Error: Beyond direct Num..."<<endl;
					else if(result==STATUS_CANNOT_WRITE)
							cout<<"Error: The dir can not be written..."<<endl;
						else if(result==STATUS_ERROR)
								cout<<"Error: Unexpected..."<<endl;
							else if(result==STATUS_FILENAME_EXIST)
									cout<<"Error: FileName has existed..."<<endl;
								else if(result==STATUS_BEYOND_RIGHT)
										cout<<"Error: Beyond Your Right..."<<endl;
		}
		break;
		   };
	case 8:{
		string fileName=command.substr(subPos+1);
		fileName=trim(fileName);
		int subPos2=fileName.find_first_of(" ");
		if(subPos2!=-1)
			cout<<"Error FileName..."<<endl;
		else{
			char name[MAX_NAME_SIZE];
			strcpy(name,fileName.c_str());
			int result=rmdir(name);
			if(result==STATUS_NOT_DIRECT)
				cout<<"Error: Not a dir..."<<endl;
			else if(result==STATUS_BEYOND_RIGHT)
				cout<<"Error: Byond your right..."<<endl;
			else if(result==STATUS_FILENAME_NOTEXIST)
				cout<<"Error: FileName doesn't exist..."<<endl;
		}
		break;
		   };
	case 9:{
		string pattern=command.substr(subPos+1);
		pattern=trim(pattern);
		int subPos2=pattern.find_first_of(" ");
		if(subPos2==-1)
			cout<<"Error Pattern..."<<endl;
		else{
			string oldName=pattern.substr(0,subPos2);
			string newName=pattern.substr(subPos2+1);
			newName=trim(newName);
			int subPos3=newName.find_first_of(" ");
			if(subPos3!=-1)
				cout<<"Error pattern..."<<endl;
			else{
				char oldName1[MAX_NAME_SIZE];
				strcpy(oldName1,oldName.c_str());
				char newName1[MAX_NAME_SIZE];
				strcpy(newName1,newName.c_str());
				int result=mv(oldName1,newName1);
				if(result==STATUS_FILENAME_NOTEXIST)
					cout<<"Error: FileName is not existed..."<<endl;
				else if(result==STATUS_BEYOND_RIGHT)
					cout<<"Error: Beyond Your Right..."<<endl;
				else if(result==STATUS_FILENAME_Illegal)
					cout<<"Error: NewName is illegal..."<<endl;
				else if(result==STATUS_FILENAME_EXIST)
					cout<<"Error: NewName has existed..."<<endl;
			}
			
		}
		break;
		   };
	case 10:{
		string pattern=command.substr(subPos+1);
		pattern=trim(pattern);
		int subPos2=pattern.find_first_of(" ");
		if(subPos2==-1)
			cout<<"Error Pattern..."<<endl;
		else{
			string source=pattern.substr(0,subPos2);
			string target=pattern.substr(subPos2+1);
			target=trim(target);
			int subPos3=target.find_first_of(" ");
			if(subPos3!=-1)
				cout<<"Error pattern..."<<endl;
			else{
				char s[MAX_NAME_SIZE];
				strcpy(s,source.c_str());
				char curDir[MAX_NAME_SIZE];
				strcpy(curDir,target.c_str());
				int result=cp(s,curDir);
				if(result==STATUS_FILENAME_NOTEXIST)
					cout<<"Error: FileName doesn'tempInode exist..."<<endl;
				else if(result==STATUS_SOURCE_AND_TARGET_ERROR)
					cout<<"Error: SourceName overlap desName..."<<endl;
				else if(result==STATUS_NOT_FILE)
					cout<<"Error: Not a file..."<<endl;
				else if(result==STATUS_READONLY)
					cout<<"Error: Read only..."<<endl;
				else if(result==STATUS_BEYOND_RIGHT)
					cout<<"Error: Beyond your right..."<<endl;
				else if(result==STATUS_NO_FREE_BLOCK)
					cout<<"Error: No free block..."<<endl;
				else if(result==STATUS_NO_FREE_INODE)
					cout<<"Error: No free inode..."<<endl;
			}
		}
		break;
			};
	case 11:{
		string fileName=command.substr(subPos+1);
		fileName=trim(fileName);
		int subPos2=fileName.find_first_of(" ");
		if(subPos2!=-1)
			cout<<"Error FileName..."<<endl;
		else{
			char name[MAX_NAME_SIZE];
			strcpy(name,fileName.c_str());
			int result=rm(name);
			if(result==STATUS_NOT_FILE)
				cout<<"Error: Not a file..."<<endl;
			else if(result==STATUS_BEYOND_RIGHT)
				cout<<"Error: Beyond your right..."<<endl;
			else if(result==STATUS_FILENAME_NOTEXIST)
				cout<<"Error: FileName doesn'tempInode exist..."<<endl;
		}
		break;
			};
	case 12:{
		string pattern=command.substr(subPos+1);
		pattern=trim(pattern);
		int subPos2=pattern.find_first_of(" ");
		if(subPos2==-1)
			cout<<"Error Pattern..."<<endl;
		else{
			string source=pattern.substr(0,subPos2);
			string target=pattern.substr(subPos2+1);
			target=trim(target);
			int subPos3=target.find_first_of(" ");
			if(subPos3!=-1)
				cout<<"Error pattern..."<<endl;
			else{
				char s[MAX_NAME_SIZE];
				strcpy(s,source.c_str());
				char curDir[MAX_NAME_SIZE];
				strcpy(curDir,target.c_str());
				int result=ln(s,curDir);
				if(result==STATUS_BEYOND_RIGHT)
					cout<<"Error: Beyond your right..."<<endl;
				else if(result==STATUS_FILENAME_NOTEXIST)
					cout<<"Error: FileName doesn'tempInode exist..."<<endl;
				else if(result==STATUS_TYPE_NOT_MATCH)
					cout<<"Error: Source Type doesn'tempInode match target type..."<<endl;
				else if(result==STATUS_SOURCE_AND_TARGET_ERROR)
					cout<<"Error: SourceName overlap desName..."<<endl;
			}
		};
		break;
			}
	case 13:{
		string fileName=command.substr(subPos+1);
		fileName=trim(fileName);
		int subPos2=fileName.find_first_of(" ");
		if(subPos2!=-1)
			cout<<"Error FileName..."<<endl;
		else{
			char name[MAX_NAME_SIZE];
			strcpy(name,fileName.c_str());
			int result=cat(name);
			if(result==STATUS_BEYOND_RIGHT)
				cout<<"Error: Beyond your right..."<<endl;
			else if(result==STATUS_NOT_FILE)
				cout<<"Error: Not a file..."<<endl;
			else if(result==STATUS_WRITEONLY)
				cout<<"Error: Write only..."<<endl;
			else if(result==STATUS_FILENAME_NOTEXIST)
				cout<<"Error: FileName doesn't exist..."<<endl;
			else if(result==STATUS_FILE_OPEN_ERROR)
				cout<<"Error: File open error..."<<endl;
		}
		break;
			}
	case 14:{
		int result=passwd();
		if(result==STATUS_PASSWORD_WRONG)
			cout<<"Error: Pint_blockssword is wrong..."<<endl;
		else if(result==STATUS_REPSWD_WRONG)
			cout<<"Error: Confirm pint_blockssword is wrong..."<<endl;
		else if(result==STATUS_ERROR)
			cout<<"Error: Unexpected error..."<<endl;
		break;
			};
	case 15:{
		string fileName=command.substr(subPos+1);
		fileName=trim(fileName);
		if(fileName.find_first_of(" ")!=-1)
			cout<<"Error FileName..."<<endl;
		else{
			char name[MAX_NAME_SIZE];
			strcpy(name,fileName.c_str());
			int result=touch(curInode,name);
			if(result==STATUS_NO_FREE_BLOCK)
				cout<<"Error: No free block..."<<endl;
			else if(result==STATUS_NO_FREE_INODE)
				cout<<"Error: No free inode..."<<endl;
			else if(result==STATUS_BEYOND_DIRECT_NUM)
				cout<<"Error: Beyond direct num..."<<endl;
			else if(result==STATUS_CANNOT_WRITE)
				cout<<"Error: The dir can not be written..."<<endl;
			else if(result==STATUS_ERROR)
				cout<<"Error: Unexpected..."<<endl;
			else if(result==STATUS_FILENAME_EXIST)
				cout<<"Error: FileName has existed..."<<endl;
			else if(result==STATUS_BEYOND_RIGHT)
				cout<<"Error: Beyond Your Right..."<<endl;
		}
		break;
			};
	case 16:{
		string fileName=command.substr(subPos+1);
		fileName=trim(fileName);
		if(fileName.find_first_of(" ")!=-1)
			cout<<"Error FileName..."<<endl;
		else{
			char name[MAX_NAME_SIZE];
			strcpy(name,fileName.c_str());
			int result=textAppend(name);
			if(result==STATUS_BEYOND_RIGHT)
				cout<<"Error: Byond your right..."<<endl;
			else if(result==STATUS_NOT_FILE)
				cout<<"Error: Not a file..."<<endl;
			else if(result==STATUS_CANNOT_WRITE)
				cout<<"Error: The file can not be written..."<<endl;
			else if(result==STATUS_FILENAME_NOTEXIST)
				cout<<"Error: FileName doesn't exist..."<<endl;
			else if(result==STATUS_BEYOND_SIZE)
				cout<<"Error: Beyond file's max size..."<<endl;
			else if(result==STATUS_FILE_OPEN_ERROR)
				cout<<"Error: File open error..."<<endl;
		}
		break;
			};
	case 17:{
		cout<<"FreeBlockNum:"<<super->freeBlockNum<<endl;
		cout<<"NextFreeBlock:"<<super->nextFreeBlock<<endl;
		cout<<"freeBlock:"<<endl;
		for(int i=0;i<=super->nextFreeBlock;i++)
			cout<<"["<<i<<"]:"<<super->freeBlock[i]<<" ";
		cout<<"FreeInodeNum:"<<super->freeInodeNum<<endl;
		cout<<"NextFreeInode:"<<super->nextFreeInode<<endl;
		cout<<"FreeInode:"<<endl;
		for(int j=0;j<=super->nextFreeInode;j++)
			cout<<"["<<j<<"]:"<<super->freeInode[j]<<" ";
		cout<<endl;
		break;
			}
	case 18:{
		string fileName=command.substr(subPos+1);
		fileName=trim(fileName);
		int subPos2=fileName.find_first_of(" ");
		if(subPos2!=-1)
			cout<<"Error FileName..."<<endl;
		else{
			char name[MAX_NAME_SIZE];
			strcpy(name,fileName.c_str());
			int result=textAppend(name);
			if(result==STATUS_BEYOND_RIGHT)
				cout<<"Error: Byond your right..."<<endl;
			else if(result==STATUS_NOT_FILE)
				cout<<"Error: Not a file..."<<endl;
			else if(result==STATUS_CANNOT_WRITE)
				cout<<"Error: The file can not be written..."<<endl;
			else if(result==STATUS_FILENAME_NOTEXIST)
				cout<<"Error: FileName doesn'tempInode exist..."<<endl;
			else if(result==STATUS_BEYOND_SIZE)
				cout<<"Error: Beyond file's max size..."<<endl;
			else if(result==STATUS_FILE_OPEN_ERROR)
				cout<<"Error: File open error..."<<endl;
		}
		break;
			};
	case 19:{
		clear();
		break;
			}
	case 20:{
		exit();
		break;
			}
	case 21:{
		string cmd=command.substr(subPos+1);
		cout<<cmd<<endl;
		cmd=trim(cmd);
		help(cmd);
		break;
			}
	default:{
		cout<<command<<": command not found"<<endl;
		break;
			};
	}
	choose();
}

void menu(){
	cout<<"1、ls		显示目录文件\n";
	cout<<"2、chmod	改变文件权限\n";
	cout<<"3、chown	改变文件拥有者\n";
	cout<<"4、chgrp	改变文件所属组\n";
	cout<<"5、pwd		显示当前目录\n";
	cout<<"6、cd		改变当前目录\n";
	cout<<"7、mkdir	创建子目录\n";
	cout<<"8、rmdir	删除子目录\n";
	cout<<"9、mv		改变文件名\n";
	cout<<"10、cp		文件拷贝\n";
	cout<<"11、rm		文件删除\n";
	cout<<"12、ln		建立文件联接\n";
	cout<<"13、cat		连接显示文件内容\n";
	cout<<"14、passwd	修改用户口令\n";
	cout<<"15、touch	创建文件\n";
	cout<<"16、>>		文本内容追加\n";
	cout<<"17、super	查看超级块状态\n";
	cout<<"18、vi		编辑文件\n";
	cout<<"19、clear	清屏\n";
	cout<<"20、exit	注销\n";
	cout<<"21、help	帮助\n";
}