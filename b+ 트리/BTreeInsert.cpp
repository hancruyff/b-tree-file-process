# include "stdio.h"
# include "string.h"
# include "BTree.h"

extern BufferManager *bufferManager;
extern BTreeHeader *bTreeHeader;
Key splitNode(BTreePagePtr page, Key key, int newPage, int index);
 /*리프가 아닌 노드를 분할*/
Key splitLeaf(BTreePagePtr page, BTreeRecord *record, int index);
 /*리프노드를 분할*/
BOOL insertRecord(BTreeRecord *newRecord){
	/*(key, value)를 갖는 레코드 추가 */
	int index = 0, leftPage = 0, rightPage = 0;
	BOOL finished = FALSE, ret;
	STACK * stack;
	Key key = newRecord -> key;
	BTreePagePtr page = (BTreePagePtr)malloc(bufferManager -> pageSize);
	if(findRecord(key, page)) return FALSE;
	while(finished == FALSE){
		stack = pop();
		if(stack == NULL){
			/*stack 이 비었다면 새로운 루트 생성 : 트리 높이 1 증가 */
			leftPage = bTreeHeader -> rootPage;
			bTreeHeader -> rootPage = newBTreePage();
			initBTreePage(page, bTreeHeader -> rootPage, FALSE);
			CHILD(page, 0) = leftPage;
			index = 0;
		} else {
			/*삽입이 일어날 노드를 읽어 온다 */
			index = stack -> index;
			if(rightPage != 0){
				readBTreePage(stack -> pageNo, page);
			}
		}
		if(isFull(page)){
			/*노드가 꽉 찼다면 split하고, 부모 노드에 삽입할 키를 얻는다*/
			if(ISLEAF(page)){
				key = splitLeaf(page, newRecord, index);
			}else{
				key = splitNode(page, key, rightPage, index);
			}

		rightPage= PAGENO(page);
		} else {
			/* 노드에 공간에 있다면 레코드를 추가 */
			if (ISLEAF(page)) {
				ret= addRecord(page, newRecord, index);
			} else {
				ret= addKey(page, key, rightPage, index);
			}
			finished= TRUE;
		}
	}
	/* 레코드가 제대로 추가되었으면, 실제로 디스크에 기록 */
	if (ret) writeBTreePage(PAGENO(page), page);
	free(page);
	return ret;
}
Key splitNode(BTreePagePtr page, Key newKey, int rightPage, int index) {
	/* 리프가 아닌 노드를 분할 */
	BTreePagePtr bigPage=
		(BTreePagePtr)malloc(bufferManager->pageSize+sizeof(Key)+sizeof(int));
	int key= 0, midIndex= 0;
	initBTreePage(bigPage, -2, FALSE);
	/* 기존 키와 새로운 키를 복사 */
	copyNode(page, bigPage, 0, KEYCNT(page));
	addKey(bigPage, newKey, rightPage, index);
	/* 상위 노드에 삽입될 키를 설정 */
	midIndex= KEYCNT(bigPage) / 2;
	key= KEY(bigPage, midIndex);
	/* Overflow가 일어난 노드에 분배 */
	copyNode(bigPage, page, 0, midIndex);
	index= newBTreePage();
	writeBTreePage(PAGENO(page), page);
	/* 새로 할당된 노드에 분배 */
	initBTreePage(page, index, FALSE);
	copyNode(bigPage, page, midIndex+1, KEYCNT(bigPage)-midIndex-1);
	writeBTreePage(PAGENO(page), page);
	free(bigPage);

	return key;
}
Key splitLeaf(BTreePagePtr page, BTreeRecord * newRecord, int index) {
		/*리프노드를 분할*/
	BTreePagePtr bigPage=
		(BTreePagePtr)malloc(bufferManager->pageSize+sizeof(BTreeRecord));
	int key= 0, midIndex= 0;
		/*기존 레코드와 새로운 레코드를 복사*/
	initBTreePage(bigPage, -2, TRUE);
	copyNode(page, bigPage, 0, KEYCNT(page));
	addRecord(bigPage, newRecord, index);
		/*상위노드에 삽입될 키를 설정, ceiling(bigNode의 key개수/2)+1*/
	midIndex= KEYCNT(bigPage) / 2-1+KEYCNT(bigPage) % 2;
	key= RECORD(bigPage, midIndex).key;
		/*Overflow가 일어난 노드에 분배*/
	copyNode(bigPage, page, 0, midIndex+1);
	index= NEXT(page);
	NEXT(page)= newBTreePage();
	writeBTreePage(PAGENO(page), page);
		/*새로 할당된 노드에 분배*/
	initBTreePage(page, NEXT(page), TRUE);
	NEXT(page)= index;
	copyNode(bigPage, page, midIndex+1, KEYCNT(bigPage)-midIndex-1);
	writeBTreePage(PAGENO(page), page);
	free(bigPage);
	return key;
}
