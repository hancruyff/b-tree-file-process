#include "BufferManager.h"
BufferManager *bufferManager;
//BufferManager를 초기화
void initBufferManager(char *name, int size){
	bufferManager = new BufferManager();
	bufferManager->fp=fopen(name, "rb+");
	if(bufferManager->fp==NULL){
		creat(name, 0644);
		initBufferManager(name, 0644);
		bufferManager->fp=fopen(name,"rb+");
	}
	bufferManager ->pageSize= size;
	bufferManager ->maxPageNo=0;
	bufferManager ->lastFreePageNo=-1;
}
BOOL readPage(int pageNo, BYTE *buffer){
	//pageNo번째 페이지를 buffer에 읽어들임
	int ret;
	fseek(bufferManager->fp, pageNo *bufferManager->pageSize, SEEK_SET);
	ret= (int)fread(buffer, sizeof(BYTE),bufferManager->pageSize,bufferManager->fp);
	rewind(bufferManager->fp);
//	printf("읽은버퍼량) : %d", ret);
	if(ret > 0)
		return TRUE;
	else
		return FALSE;
}
BOOL writePage(int pageNo, BYTE *buffer){
	//buffer의 내용을 pageNo번째 페이지에 씀
	int ret;
	fseek(bufferManager->fp, pageNo *bufferManager->pageSize, SEEK_SET);
	ret= (int)fwrite(buffer, sizeof(BYTE), bufferManager->pageSize, bufferManager->fp);
	rewind(bufferManager->fp);
	if(ret>0)
		return TRUE;
	else
		return FALSE;
}
int newPage(){
	//비어있는 새로운 페이지의 번호를 반환하고, lastFreePageNo를 조정함
	int freePageNo, nextPageNo, offset;
	BYTE *buffer= new BYTE(sizeof(bufferManager->pageSize)); ////////////////////////////////////
	//BYTE *buffer= (BYTE*)malloc(sizeof(bufferManager->pageSize));
	//자유 페이지 리스트에 페이지가 없으면 새 페이지를 할당해야 함
	if(bufferManager->lastFreePageNo == -1){
		//현재 가장 마지막 페이지의 다음 번호를 할당
		freePageNo = ++(bufferManager->maxPageNo);
	}
	else{//자유 페이지 리스트에 피이지가 있으면 그 페이지를 이용
		//자유 페이지 리스트의 첫 번째 페이지 번호를 얻음
		freePageNo= bufferManager->lastFreePageNo;
		//그 페이지의 nextPageNo값을 lastFreePageNo값으로 지정
		offset=freePageNo *bufferManager->pageSize;
		fseek(bufferManager->fp, offset, SEEK_SET);
		fread(& nextPageNo, sizeof(int), 1, bufferManager->fp);
		if(nextPageNo==-1)
			bufferManager->lastFreePageNo=-1;
		else
			bufferManager->lastFreePageNo=nextPageNo+1;
	}
	free(buffer);
	return freePageNo;
}
void freePage(int pageNo){
	//페이지를 초기화한 다음, 자유 페이지 리스트에 추가
	int offset;
	BYTE *buffer= (BYTE*)malloc(bufferManager->pageSize);	
	//BYTE *buffer= new BYTE(bufferManager->pageSize);
	//BYTE *buffer= malloc(bufferManager->pageSize);////////////////////////////////////
	//pageNo번째 페이지의 다음 페이지 값으로 현재lastFreePageNo를 갖게 함
	offset= pageNo *bufferManager->pageSize;
	fseek(bufferManager->fp, offset, SEEK_SET);
	fwrite(&(bufferManager->lastFreePageNo), sizeof(int), 1, bufferManager->fp);
	//lastFreepageNo를 pageNo 값으로 설정
	bufferManager->lastFreePageNo = pageNo;
	free(buffer);
}
void loadHeaderPage(BYTE *header){
	//0번 디스크 페이지에서header를 읽어옴
	readPage(0, header);
}
void saveHeaderPage(BYTE *header){
	//0번 디스크 페이지에 header를 저장
	writePage(0, header);
}

void creat(char *name,int n){
   name = (char*)malloc(sizeof(char)*n); 
}