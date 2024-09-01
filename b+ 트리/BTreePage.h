#ifndef __HASH_PAGE_
#define __HASH_PAGE_
#include "BaseHeader.h"
typedef int Key;
typedef char Value[12];
typedef struct _STACK STACK;
typedef char *BTreePagePtr;
typedef struct _BTreeRecord BTreeRecord;
typedef struct _BTreeHeader BTreeHeader;
/*Stack 구조체*/
struct _STACK {
	int pageNo;
	int index;
};
/*B+-트리 레코드 구조체*/
struct _BTreeRecord {
	Key key;
	Value value;
};
/*B+-트리 헤더 구조체*/
struct _BTreeHeader {
	int rootPage;
	int firstSequencePage;
	int order;
	int minKey, minRecord;
	int maxRecord;
	STACK *stack;
	int stackTop, stackPtr;
};
/*
	[Internal Node]
		(int) 페이지 번호
		(int) 다음 페이지 번호 = -1
		(int) 저장된 키의 개수
		(int) 0번째 자식페이지 번호
		(int) 0번째 키

		(int) order-1번째 자식페이지 번호
		(int) order-1번째 키
		(int) order 번째 자식페이지 번호

	[Leaf Node]
		(int) 페이지 번호
		(int) 다음 페이지 번호 >= 0
		(int) 저장된 레코드의 개수
		(BTreeRecord) 0번째 레코드
		(BTreeRecord) 1번째 레코드

		(BTreeRecord) maxRecord-1번째 레코드
*/

#define PAGENO(p) (*(int *) (p)) /* 페이지의 번호 */
#define NEXT(p) (*(int *) (p+sizeof(int))) /* 다음 페이지 번호 */
#define KEYCNT(p) (*(int *) (p+sizeof(int) *2)) /* 저장된 키의 갯수=저장된 레코드의 갯수 */
#define CHILD(p, i) (*(CHILDPTR(p, i))) /* i번째 자식의 페이지 번호 */
#define KEY(p, i) (*(KEYPTR(p, i)))  /* i번째 키 */
#define RECORD(p, i) (*(RECORDPTR(p)+(i))) /* i번째 레코드 */
#define ISLEAF(p) ((NEXT(p) != -1) ? TRUE : FALSE) /* 리프인지를 판별 */
#define MIN(p) (ISLEAF(p) ? bTreeHeader->minRecord : bTreeHeader->minKey) /* 최소 키(레코드)의 수를 구함*/
#define CHILDPTR(p, i) ((int *) (p+sizeof(int) *3) +(i) *2) 
#define KEYPTR(p, i) ((int *) (p+sizeof(int) *4) +(i) *2)
#define RECORDPTR(p) ((BTreeRecord *) (p+sizeof(int) *3))

void initBTreeHeader(int root, int firstSequence); 
	/* B+ 트리 Header를 초기화 */
void initBTreePage(BTreePagePtr page, int pageNo, BOOL leaf);  
	/* B+ 트리 Page를 초기화*/

BOOL readBTreePage(int pageNo, BTreePagePtr page);
	/*(pageNo)번째 페이지를 page에 읽어들임.
	참고 : 페이지는 1번부터 시작 (0번은 헤더 페이지 */
BOOL writeBTreePage(int pageNo,BTreePagePtr page);
	/*page의 내용을 (pageNo)번째 페이지에 씀*/
int newBTreePage();
	/* 비어있는 새로운 페이지의 번호를 반환함 */
void freeBTreePage(BTreePagePtr page);
	/* B+트리 페이지를 반환함 */
BOOL addKey(BTreePagePtr page, Key key, int rightPage, int index);
	/* BTreePage의 index번째 자리에 (key, child)를 추가 */
BOOL addRecord(BTreePagePtr page, BTreeRecord *record, int index);
	/* BTreePage의 index번째 자리에 (key, value)를 갖는 레코드 추가 */
BOOL removeRecord(BTreePagePtr page, int index);
	/* index번째 레코드를 제거 */
BOOL removeKey(BTreePagePtr page, int index);
	/* index번째의 키와 오른쪽 자식 페이지 번호를 제거 */
BOOL isFull(BTreePagePtr targetPage);
	/* targetPage가 꽉 찼는지 검사 */
void copyNode(BTreePagePtr sourcePage, BTreePagePtr targetPage, int from, int count);
	/* node내용의 복사 */
void copyRecord(BTreeRecord *sourceRecord, BTreeRecord *targetRecord);
	/* sourceRecord를 targetRecord로 복사 */
void copyKey(Key *sourceKey, Key *targetKey);
	/* sourceKey의 키와 오른쪽 자식 페이지 번호를 targetKey에 복사 */
void loadBTreeHeaderPage();
	/* 헤더 페이지를 로드 */
void saveBTreeHeaderPage();
	/* 헤더 페이지에 저장 */
#endif
