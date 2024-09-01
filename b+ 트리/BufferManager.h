#ifndef _BUFFER_MANAGER_
#define _BUFFER_MANAGER_
#include "BaseHeader.h"
typedef struct _BufferManager BufferManager;
struct _BufferManager{
	FILE* fp;
	int pageSize;
	int maxPageNo;
	int lastFreePageNo;
};
void initBufferManager(char *FILEName,int pageSize);
//BufferManager를 초기화
BOOL readPage(int pageNo, BYTE *buffer);
//pageNo번째 페이지를 buffer에 읽어 들임
BOOL writePage(int pageNo, BYTE *buffer);
//buffer의 내용을 pageNo번째 페이지에 씀
int newPage();
//비어있는 새로운 페이지의 번호를 반환함
void freePage(int pageNo);
//페이지를 초기화한 다음, 자유 페이지 리스트에 추가
int newHashPage();
//비어있는 새로운 페이지의 번호를 반환하고, lastFreePageNo를 조정함
void freeHashPage(int pageNo);
//해시 페이지를 반환함
void saveHeaderPage(BYTE*);
//bufferManager의 내용을 헤더 페이지에 씀(수정)
void loadHeaderPage(BYTE*);
//헤더 페이지의 내용을 bufferManager에 읽어 들임(수정)
void creat(char*, int);
//추가
#endif
