#include "BTree.h"
extern BufferManager * bufferManager;
extern BTreeHeader * bTreeHeader;
void mergeNode(BTreePagePtr child, BTreePagePtr sibling, BTreePagePtr parent, STACK * stack);
	/*리프가 아닌 노드를 합병*/
void mergeLeaf(BTreePagePtr child, BTreePagePtr sibling, BTreePagePtr parent, STACK * stack); 
	/*리프 노드를 합병*/
BOOL redistributeLeaf(BTreePagePtr child, BTreePagePtr sibling, BTreePagePtr parent, int i);
	/*리프가 아닌 노드를 재분배*/
BOOL redistributeNode(BTreePagePtr child, BTreePagePtr sibling, BTreePagePtr parent, int i);
	/*리프 노드를 재분배*/ 
int selectSibling(BTreePagePtr sibling, BTreePagePtr parent, STACK * stack);
	/*재분배에 참여하는 형제노드를 선택한다*/
BOOL deleteRecord(Key key){
	/*key를 갖는 레코드 삭제*/
	int i= 0;
	BOOL finished= FALSE, ret;
	STACK * stack;
	BTreePagePtr child 	= (BTreePagePtr)malloc(bufferManager-> pageSize); 
	BTreePagePtr sibling 	= (BTreePagePtr)malloc(bufferManager-> pageSize); 
	BTreePagePtr parent 	= (BTreePagePtr)malloc(bufferManager-> pageSize); 
	BTreePagePtr temp;
	if (findRecord(key, child) ==FALSE) return FALSE;
	while (finished == FALSE) {
		stack= pop(); 
		if (ISLEAF(child)) {
			ret= removeRecord(child, stack->index);
		} else {
			ret= removeKey(child, stack->index);
		}
		if (stack-> pageNo == bTreeHeader-> rootPage) {
			if (KEYCNT(child) == 0 && ISLEAF(child)== FALSE) {
				/*루트가 비게면 0번째 자식을 새로운 루트로 삼는다 */ 				
				freeBTreePage(child);
				bTreeHeader-> rootPage= CHILD(child, 0);
				return TRUE;
			}		
			finished= TRUE;
		} else if (KEYCNT(child) < MIN(child)) { 
			stack= peek();
			
			i=selectSibling(sibling, parent, stack);
	if (i == -1) {
		if(ISLEAF(child)){
			mergeLeaf(child, sibling, parent, stack);
		}else{
			mergeNode(child, sibling, parent, stack);
		}
	}else{
		if(ISLEAF(child)){
		redistributeLeaf(child, sibling, parent,i);
		}else{
		redistributeNode(child, sibling, parent, i);
		}
		finished=TRUE;
	}
	temp=child;
	child=parent;
	parent=temp;
}else{
	finished=TRUE;
  }
}
	writeBTreePage(PAGENO(child),child);
	free(child);
	free(sibling);
	free(parent);
	return TRUE;
}

void mergeNode(BTreePagePtr child, BTreePagePtr sibling, BTreePagePtr parent, STACK * stack){
	/*리프가 아닌 노드를 합병*/
	int j=0;
	BTreePagePtr temp;
	if(stack->index == KEYCNT(parent)){
		temp=sibling;
		sibling=child;
		child=temp;

		stack-> index--;
		readBTreePage(CHILD(parent, stack-> index), child);
	} else {
		readBTreePage(CHILD(parent, stack-> index+1), sibling);
	}
	KEY(child, KEYCNT(child))= KEY(parent, stack-> index);
	CHILD(child, KEYCNT(child)+1)= CHILD(sibling, 0);
	KEYCNT(child) ++;
	for(j= 0; j < KEYCNT(sibling); j++) {
		copyKey(KEYPTR(sibling, j), KEYPTR(child, KEYCNT(child)));
		KEYCNT(child) ++;
	}
	writeBTreePage(PAGENO(child), child);
	freeBTreePage(sibling);
}
void mergeLeaf(BTreePagePtr child, BTreePagePtr sibling,
		BTreePagePtr parent, STACK * stack) {
	/*리프 노드를 합병*/
	int j= 0;
	BTreePagePtr temp;
	if (stack-> index == KEYCNT(parent)) {
		temp= sibling;
		sibling= child;
		child= temp;
		stack->index--;
		readBTreePage(CHILD(parent, stack-> index), child);
	} else {
		readBTreePage(CHILD(parent, stack-> index+1), sibling);
	}
	for (j= 0; j < KEYCNT(sibling); j++) {
		copyRecord(RECORDPTR(sibling)+j, RECORDPTR(child)+KEYCNT(child));
		KEYCNT(child) ++;
	}
	NEXT(child)= NEXT(sibling);
	writeBTreePage(PAGENO(child), child);
	freeBTreePage(sibling);
}

BOOL redistributeLeaf(BTreePagePtr child,BTreePagePtr sibling,BTreePagePtr parent,int i){
	/*리프노드를 재분배*/
	int moveCount = (KEYCNT(child)+KEYCNT(sibling)) / 2-KEYCNT(child);
	int j=0;
	if(RECORD(child,0).key < RECORD(sibling,0).key){
		for(j=0;j<moveCount;j++){
			copyRecord(RECORDPTR(sibling)+j,RECORDPTR(child)+KEYCNT(child)+j);
		}
		KEYCNT(child) += moveCount;
		KEYCNT(sibling) -= moveCount;
		/*왼쪽으로 이동*/
		for(j=0;j<KEYCNT(sibling);j++){
			copyRecord(RECORDPTR(sibling)+moveCount+j,RECORDPTR(sibling)+j);
		}
		KEY(parent,i)=RECORD(child,KEYCNT(child)-1).key;
	}
	else{
		/*오른쪽으로 이동*/
		for(j=KEYCNT(child);j>0;j--){
			copyRecord(RECORDPTR(child)+j-1,RECORDPTR(child)+moveCount+j-1);
		}
		KEYCNT(child) += moveCount;
		KEYCNT(sibling) -= moveCount;
		for(j=0;j<moveCount;j++){
			copyRecord(RECORDPTR(sibling)+KEYCNT(sibling)+j,RECORDPTR(child)+j);
		}
		KEY(parent,i)=RECORD(sibling,KEYCNT(sibling)-1).key;
	}
	writeBTreePage(PAGENO(child),child);
	writeBTreePage(PAGENO(sibling),sibling);
	return TRUE;
}

BOOL redistributeNode(BTreePagePtr child, BTreePagePtr sibling, BTreePagePtr parent, int i)
{
	/*리프 노드를 재분배*/
	int moveCount= (KEYCNT(child)+KEYCNT(sibling)) / 2-KEYCNT(child);
	int j= 0;
	if (KEY(child, 0) < KEY(sibling, 0))
	{
		/*Underflow가 일어난 노드를 채운다*/
		KEY(child, KEYCNT(child))= KEY(parent, i);
		CHILD(child, KEYCNT(child)+1)= CHILD(sibling, 0);
		for (j= 0; j < moveCount-1; j++)
		{
			copyKey(KEYPTR(sibling, j), KEYPTR(child, KEYCNT(child)+j+1));
		}
		KEYCNT(child) += moveCount;
			/*부모 노드로 중간 키 값을 복사*/
		KEY(parent, i)= KEY(sibling, moveCount-1);
			/*재분배에 참여한 sibling을 정리*/
		KEYCNT(sibling) -= moveCount;
		CHILD(sibling, 0)= CHILD(sibling, moveCount);
		for (j=0; j < KEYCNT(sibling); j++)
		{
			copyKey(KEYPTR(sibling, moveCount+j), KEYPTR(sibling, j));
		}
	}
	else
	{
		/*Underflow가 일어난 노드를 정리*/
		for (j= KEYCNT(child); j > 0; j--)
		{
			copyKey(KEYPTR(child, j-1), KEYPTR(child, j-1+moveCount));
		}
		CHILD(child, moveCount)= CHILD(child, 0);
		KEYCNT(child) += moveCount;
			/*Underflow가 일어난 노드를 채운다*/
		KEYCNT(sibling) -= moveCount;
		KEY(child, moveCount-1)= KEY(parent, i);
		for (j= 0; j < moveCount-1; j++)
		{
			copyKey(KEYPTR(sibling, KEYCNT(sibling)+j+1), KEYPTR(child,j));
		}
		CHILD(child, 0)= CHILD(sibling, KEYCNT(sibling)+1);
			/*부모 노드로 중간 키 값을 복사*/
		KEY(parent, i)= KEY(sibling, KEYCNT(sibling));
			

}
 writeBTreePage(PAGENO(child), child);
 writeBTreePage(PAGENO(sibling), sibling);
 return TRUE;
}
int selectSibling(BTreePagePtr sibling, BTreePagePtr parent, STACK *stack){
	/*재분배에 참여하는 형제 노드를 선택한다*/
	int i= -1;
	readBTreePage(stack->pageNo, parent);
	if(stack->index == 0){
		readBTreePage(CHILD(parent, 1), sibling);
		if (KEYCNT(sibling)
		>(ISLEAF(sibling)? bTreeHeader->minRecord:bTreeHeader->minKey)){
			i= stack->index;
		}
	}else if (stack->index == KEYCNT(parent)){
		readBTreePage(CHILD(parent, stack->index-1), sibling);
		if(KEYCNT(sibling)
			> (ISLEAF(sibling) ?bTreeHeader->minRecord:bTreeHeader -> minKey)){
				i= stack -> index-1;
		}
	}else {
		readBTreePage(CHILD(parent, stack->index+1), sibling);
		if(KEYCNT(sibling)
			> (ISLEAF(sibling)
			? bTreeHeader-> minRecord
			: bTreeHeader-> minKey)){
				i= stack-> index;
		}else{
			readBTreePage(CHILD(parent, stack->index-1), sibling);
			if(KEYCNT(sibling)
				>(ISLEAF(sibling)
				? bTreeHeader-> minRecord
				: bTreeHeader-> minKey)){
					i= stack->index-1;
			}
		}
	}
	return i;
}
