# include "BTreePage.h"
# include "BufferManager.h"
# include "BTree.h"
extern BufferManager	* bufferManager;
BTreeHeader *bTreeHeader;
void initBTreeHeader(int root, int firstSequence) {
	/*BTree 헤더를 초기화*/
	bTreeHeader= (BTreeHeader *)malloc(sizeof(BTreeHeader));
	bTreeHeader->rootPage= root;
	bTreeHeader->firstSequencePage= firstSequence;
	bTreeHeader->order=
		(int) ((bufferManager->pageSize-sizeof(int)*4)/(sizeof(int) *2))+1;
	bTreeHeader->minKey=
		bTreeHeader->order / 2-1+bTreeHeader->order % 2;
	bTreeHeader->maxRecord=
		(bufferManager->pageSize-sizeof(int) *3) / (sizeof(BTreeRecord));
	bTreeHeader->minRecord=bTreeHeader->maxRecord /2;
	bTreeHeader->stack= (STACK *)malloc(sizeof(STACK) *5);
	bTreeHeader->stackTop= 4;
	bTreeHeader->stackPtr= 0;
	

}
	void initBTreePage(BTreePagePtr page, int pageNo, BOOL leaf){
		/*BTreePage를 초기화 */
		PAGENO(page)= pageNo;
		NEXT(page)= leaf ? 0 : -1;    //리프노드에는 차위페이지 인덱스를 가짐. 초기화 시에는 0값을 부여
		KEYCNT(page)=0;
	}
	BOOL readBTreePage(int pageNo, BTreePagePtr page){
		/*pageNo번째 페이지를 page에 읽어 들임.
		  참고 : 페이지는 1번부터 시작(0번은 헤더 페이지)*/
		BOOL ret;
		ret= readPage(pageNo, page);
		return ret;
	}
	BOOL writeBTreePage(int pageNo, BTreePagePtr page){
		/*page의 내용을 (pageNo)번째 페이지에 씀
		  참고 : 페이지는 1번부터 시작( 0번은 헤더페이지) */
		BOOL ret;
		ret=writePage(pageNo,page);
		return ret;
	}
	int newBTreePage(){
		/* 비어있는 새로운 B+ - Tree 페이지의 번호를 반환함 */
		return newPage();
	}
	void freeBTreePage(BTreePagePtr page){
		/*페이지 번호가 victimHashPageNo인 것을 자유 페이지 리스트에 추가*/
		if(PAGENO(page)==bTreeHeader->firstSequencePage){
			bTreeHeader->firstSequencePage=NEXT(page);
		}
		freePage(PAGENO(page));
	}

BOOL addKey(BTreePagePtr page, Key key, int rightPage, int index) {
	int j = 0;
	for(j = KEYCNT(page); j>index; j--) {
		KEY(page, j) = KEY(page, j-1);
		CHILD(page, j+1) = CHILD(page, j);
	}
	KEY(page, index) = key;
	CHILD(page, index+1) = rightPage;
	KEYCNT(page)++;
	return TRUE;
}
BOOL addRecord(BTreePagePtr page, BTreeRecord *newRecord, int index) {
	int j = 0;
	for(j = KEYCNT(page); j > index; j--)
		copyRecord(RECORDPTR(page)+j-1, RECORDPTR(page)+j);
	copyRecord(newRecord, RECORDPTR(page)+index);
	KEYCNT(page)++;
	return TRUE;
}
BOOL removeRecord(BTreePagePtr page, int index) {
	int j = 0;
	for(j = index; j < KEYCNT(page)-1; j++)
		copyRecord(RECORDPTR(page)+j+1, RECORDPTR(page)+j);
	KEYCNT(page)--;
	return TRUE;
}
BOOL removeKey(BTreePagePtr page, int index) {
	int j = 0;
	for(j=index; j < KEYCNT(page)-1; j++) {
		KEY(page, j) = KEY(page, j+1);
		CHILD(page, j+1) = CHILD(page, j+2);
	}
	KEYCNT(page)--;

 return TRUE;
}
BOOL isFull(BTreePagePtr targetPage){
 /*페이지가 가득 찼는지 검사*/
 if(ISLEAF(targetPage)){
  if(KEYCNT(targetPage)==(bTreeHeader->maxRecord)) return TRUE;
  else return FALSE;
 }else{
   if (KEYCNT(targetPage)==(bTreeHeader->order-1)) return TRUE;
   else return FALSE;
  }
}
void copyNode(BTreePagePtr sourcePage, BTreePagePtr targetPage, int from, int count){
/*sourcePage의 from부터의 정보를 targetPage에 저장, 이전 targetPage의 내용은 모두 지워진다.*/
 int i=0;
 KEYCNT(targetPage)=0;
 if(ISLEAF(sourcePage)){
  for(i=0; i<count; i++){
   copyRecord(RECORDPTR(sourcePage)+(from+i),RECORDPTR(targetPage)+i);
   KEYCNT(targetPage)++;
  }
 }else{
  for(i=0; i<count; i++){
   CHILD(targetPage, i)=CHILD(sourcePage, from+i);
   KEY(targetPage, i)=KEY(sourcePage, from+i);
   KEYCNT(targetPage)++;
  }
  CHILD(targetPage, i)=CHILD(sourcePage, from+i);
 }
}
void copyRecord(BTreeRecord *sourceRecord, BTreeRecord *targetRecord){
 /*source에서 target으로 레코드를 복사*/
 int i=0;
 targetRecord->key=sourceRecord->key;

	for (i= 0; i < sizeof(sourceRecord->value); i++)
		targetRecord->value[i]= sourceRecord->value[i];
}
void copyKey(Key *sourceKey, Key *targetKey) {
	/*source에서 target으로 키와 키의 오른쪽 자식 노드 번호를 복사*/
	int i= 0;
	*targetKey= *sourceKey;
	*(targetKey+1)= *(sourceKey+1);
}
void loadBTreeHeaderPage() {
	  /* 헤더 페이지의 내용을 디스크에서 읽어들임 */
	BYTE *header= (BYTE *)malloc(bufferManager->pageSize);
	loadHeaderPage(header);
	bufferManager->pageSize= *((int *)header);
	bufferManager->maxPageNo= *((int *)header+1);
	bufferManager->lastFreePageNo= *((int *)header+2);
	initBTreeHeader(* ((int *)header+3), *((int *)header+4));
	free(header);
}

void saveBTreeHeaderPage() {
	 /* 헤더 페이지의 내용을 디스크에 저장 */
	BYTE *header= (BYTE *)malloc(bufferManager->pageSize);
	*((int *)header)    = bufferManager->pageSize;
	*((int *)header+1) = bufferManager->maxPageNo;
	*((int *)header+2) = bufferManager->lastFreePageNo;
	*((int *)header+3) = bTreeHeader->rootPage;
	*((int *)header+4) = bTreeHeader->firstSequencePage;
	saveHeaderPage(header);
	free(header);
}
