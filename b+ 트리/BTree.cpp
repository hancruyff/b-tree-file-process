 #include "BTree.h"
 extern BufferManager *bufferManager;
 extern BTreeHeader *bTreeHeader;
 
 void initBTree(char *name, int pageSize, BOOL newStart) {
		/*B+-트리를 초기화*/
	if (newStart) {
		int rootPage;
		//bufferManager = (BufferManager*)malloc(sizeof(BufferManager));
		BTreePagePtr page= (char*)malloc(pageSize);///////////////////////////////////////
		initBufferManager(name, pageSize);
		rootPage= newBTreePage();
		initBTreePage(page, rootPage, TRUE);
		writeBTreePage(rootPage, page);
		initBTreeHeader(rootPage, rootPage);
		free(page);
	} else {
		initBufferManager(name, pageSize);
		loadBTreeHeaderPage();
	}
}
void closeBTree() {
		/*b+-트리 헤더를 디스크에 저장하고 화일을 닫는다 */
	saveBTreeHeaderPage();
	fclose(bufferManager->fp);
	free(bTreeHeader->stack);
	free(bTreeHeader);
	free(bufferManager);
}
void push(int pageNo, int index) {
		/*스택에 pageNo와 index를 push*/
	if (bTreeHeader->stackPtr < bTreeHeader->stackTop+1) {
		bTreeHeader->stack[bTreeHeader->stackPtr].pageNo= pageNo;
		bTreeHeader->stack[bTreeHeader->stackPtr].index= index;
		bTreeHeader->stackPtr++;
	} else {
		/*2배 크기의 스택으로 다시 할당해 준다.*/
		bTreeHeader->stack= (STACK *)realloc(bTreeHeader->stack,
			sizeof(STACK) *(bTreeHeader->stackTop+1) *2);
		bTreeHeader->stackTop= (bTreeHeader->stackTop+1) *2-1;

		/*push*/
		bTreeHeader->stack[bTreeHeader->stackPtr].pageNo= pageNo;
		bTreeHeader->stack[bTreeHeader->stackPtr].index= index;
		bTreeHeader->stackPtr++;
	}
}

STACK *pop() {
	/*스택의 top을 pop*/
	if (bTreeHeader->stackPtr == 0) {
		return NULL;
	} else {
		bTreeHeader->stackPtr--;
		return (bTreeHeader->stack+bTreeHeader->stackPtr);
	}
}
STACK *peek() {
	/*스택의 top을 반환함*/
	if (bTreeHeader->stackPtr == 0) return NULL;
	else return (bTreeHeader->stack+bTreeHeader->stackPtr-1);
}
