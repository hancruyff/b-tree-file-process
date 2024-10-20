## 🗉목차
[1. 개요](#1-개요)

[2. B+ 트리](#2-B-트리)

[3. 주요 기능](#3-주요-기능)

[4. 결과](#4-결과)

## 1. 🌳개요

파일 시스템에서 빠르게 데이터를 처리하고 검색할 수 있는 B+ 트리 구조를 구현. 

B+ 트리는 데이터베이스 및 파일 시스템에서 일반적으로 사용되는 균형 트리의 일종으로, 높은 성능과 빠른 검색 속도가 특징

<img src="https://img.shields.io/badge/c++-00599C?style=flat-square&logo=c%2B%2B&logoColor=white">

## 2. B+ 트리
<img src="https://github.com/user-attachments/assets/23c7fe29-dddf-4b57-abe7-489a6ebda450" width="440" height="160"/>

B+ 트리는 다음과 같은 특징을 가지고 있습니다:
- **균형 트리 구조**: 모든 리프 노드가 동일한 깊이를 가지며, 효율적인 검색을 가능하게 함
- **모든 키가 리프 노드에 존재**: 검색 시 항상 리프 노드에서 데이터를 찾을 수 있음.
- **연결 리스트**: 리프 노드는 서로 연결되어 있어 순차적인 데이터 검색이 가능.

## 3. 🔧주요 기능

i : (키,값)을 가지는 레코드 삽입

<details>
<summary>자세히</summary>

레코드 중복 검사: 삽입하려는 키가 이미 존재하는지 확인. 존재할 경우 삽입을 중단.

노드 탐색: 삽입 위치를 찾기 위해 스택을 사용하여 트리를 탐색. 스택이 비어 있으면 새로운 루트를 생성.

노드 분할 처리: 현재 페이지가 가득 차면 노드를 분할, 새로운 키를 부모 노드에 삽입.

레코드 추가: 노드에 공간이 있다면 새로운 레코드를 추가.

디스크 기록: 모든 작업이 완료된 후, 변경된 페이지를 디스크에 기록.
  <details>
  <summary>코드 펼치기</summary>

```
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
```
  </details>
</details>

d: 키를 가지는 레코드 삭제
<details>
<summary>자세히</summary>
  
레코드 존재 확인: 삭제하려는 키가 존재하는지 확인합니다. 존재하지 않으면 함수가 종료됩니다.
  
노드 탐색: 삭제할 키가 있는 노드를 찾기 위해 스택을 사용하여 트리를 탐색합니다.

레코드 삭제: 키가 있는 노드에서 레코드를 삭제합니다.

언더플로우 처리: 노드에서 레코드를 삭제한 후, 노드의 키 수가 최소 허용치를 밑돌면 형제 노드와 합병하거나 키를 재분배합니다.

루트 노드 처리: 루트 노드가 비게 되면 새로운 루트를 설정합니다
  <details>
  <summary>코드 펼치기</summary>
    
    ```
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
    ```
  </details>
</details>
r: 키를 가지는 레코드 검색
<details>
<summary>자세히</summary>
  
사용자가 입력한 키를 통해 레코드를 검색하고, 그 결과를 출력하는 메인 로직

특정 키를 가진 레코드를 실제로 검색하는 retrieveRecord 함수
<details>
<summary>코드 펼치기</summary>
  
  ```
  case 'r':    // 키를 가지는 레코드 검색
    fscanf(stdin, "%i", &record.key);  // 사용자로부터 키 입력 받기
    if (retrieveRecord(record.key, &record)) {
        printf("Retrieve (%d, %s) : success\n", record.key, record.value);  // 성공 메시지 출력
    } else {
        printf("Retrieve (%d) : fail\n", record.key);  // 실패 메시지 출력
    }
    break;

  ```
키 입력: fscanf를 사용하여 사용자로부터 검색할 키를 입력받음.

레코드 검색: retrieveRecord 함수를 호출하여 해당 키에 대한 레코드를 검색.

결과 출력: 검색 성공 여부에 따라 성공 메시지 또는 실패 메시지를 출력.
```
BOOL retrieveRecord(Key key, BTreeRecord *record) {
    /* 인덱스 셋으로부터 레코드 검색 */
    BTreePagePtr page = (BTreePagePtr)malloc(bufferManager->pageSize);
    int i = 0;
    BOOL found = FALSE;

    if (findRecord(key, page)) {  // 키가 있는 페이지 찾기
        i = peek()->index;  // 키의 인덱스 얻기
        copyRecord(RECORDPTR(page) + i, record);  // 레코드 복사
        found = TRUE;  // 레코드 발견
    }

    free(page);  // 할당된 페이지 메모리 해제
    return found;  // 레코드 발견 여부 반환
}
```
페이지 할당: BTreePagePtr page를 사용하여 검색할 페이지에 대한 메모리를 할당.

레코드 검색: findRecord(key, page)를 호출하여 지정된 키가 존재하는 페이지를 찾음.

인덱스 찾기: 키가 발견되면, peek()->index를 통해 해당 키의 인덱스를 얻음.

레코드 복사: copyRecord 함수를 사용하여 페이지에서 레코드를 복사.

메모리 해제: 사용이 끝난 페이지 메모리를 해제.

결과 반환: 레코드가 발견되었는지를 나타내는 불리언 값을 반환.
</details>
</details>
a : 모든 페이지 검색
<details>
<summary>자세히</summary>
  
헤더 페이지 제외: 페이지 번호 0은 헤더 페이지(검색에서 제외), 1부터 시작.
  
페이지 읽기: 각 페이지를 순차적으로 읽어들이며, 페이지가 존재하는 동안 반복.

페이지 정보 출력: 읽어들인 페이지가 내부 노드인지 리프 노드인지에 따라 적절한 정보를 출력.

내부 노드: 페이지 번호, 다음 페이지, 키 개수, 키 리스트를 출력.

리프 노드: 페이지 번호, 레코드 개수, 레코드 정보를 출력.

페이지 종료: 모든 페이지를 읽은 후 종료.
<details>
<summary>코드 펼치기</summary>
  
  ```
  void retrieveAllPages() {
	/*모든 페이지 검색*/
	/*0 페이지는 헤더페이지이므로 제외 1페이지부터 순차 검색*/

	BTreePagePtr page = (BTreePagePtr)malloc(bufferManager->pageSize);
	int i = 1;
	while (readBTreePage(i, page)) {
		if (ISLEAF(page) == FALSE) {
			printf("페이지번호   : %d (내부노드)\n", PAGENO(page));
			printf("다음페이지   : %d\n", NEXT(page));
			printf("키의개수     : %d\n", KEYCNT(page));
			printf("키리스트     : ");
			for (int j = 0; (j < KEYCNT(page)); j++) {
				printf("%d, (%d), ", CHILD(page, j), KEY(page, j));
			}
			printf("%d\n", CHILD(page, KEYCNT(page)));
			printf("--------------------------------------\n");

		}
		else {
			printf("페이지번호 : %d (리프노드)\n", PAGENO(page));
			printf("레코드개수 : %d\n", KEYCNT(page));
			for (int j = 0; (j < KEYCNT(page)); j++) {
				printf("%d %s\n", RECORD(page, j).key, RECORD(page, j).value);
			}
			printf("다음페이지 : %d\n", NEXT(page));
			printf("--------------------------------------\n");

		}
		i++;
	}
}
  ```
</details>
</details>

h : 헤더 정보 보기

s : 순차 검색
<details>
<summary>자세히</summary>
특정 키를 순차적으로 검색하는 기능을 수행

사용자가 입력한 키를 기반으로 리프 노드에서 해당 키를 찾아 그 값을 출력
<details>
<summary>코드 펼치기</summary>

```
void Sequential_Search(void) {
    BTreePagePtr page = (BTreePagePtr)malloc(bufferManager->pageSize);  // 페이지 포인터 할당
    int i = 1;  // 페이지 번호 초기화
    int Searchkey;  // 검색할 키
    int r = 0;  // 검색 결과 플래그 초기화
    scanf("%d", &Searchkey);  // 사용자로부터 검색할 키 입력 받기

    while (readBTreePage(i, page)) {  // 페이지를 순차적으로 읽기
        if (ISLEAF(page) == FALSE) {
            // 내부 노드인 경우 처리 (현재는 비워둠)
        } else {
            // 리프 노드인 경우
            for (int j = 0; j < KEYCNT(page); j++) {
                // 현재 페이지의 레코드에서 키 검색
                if (Searchkey == RECORD(page, j).key) {
                    printf("%d %s\n", RECORD(page, j).key, RECORD(page, j).value);  // 키와 값 출력
                    r = 1;  // 키를 찾았음을 표시
                    break;  // 키를 찾았으므로 루프 종료
                }
            }
        }
        i++;  // 다음 페이지로 이동
    }

    if (r == 0) {
        printf("Sequential_Search (%d) : fail\n", Searchkey);  // 키를 찾지 못한 경우 메시지 출력
    }
}

```
페이지 포인터 할당: BTreePagePtr page를 사용하여 검색할 페이지에 대한 메모리를 할당.

페이지 번호 초기화: int i = 1;로 페이지 번호를 초기화하여 1번 페이지부터 검색을 시작.

사용자 입력: scanf("%d", &Searchkey);를 통해 사용자가 검색할 키를 입력받음.

페이지 읽기 루프: while (readBTreePage(i, page))를 통해 페이지를 순차적으로 읽기 시작.

리프 노드 처리: 리프 노드에 도달하면, 페이지의 모든 레코드를 순회하며 입력된 키와 비교.

키 검색: 각 레코드의 키와 입력된 Searchkey를 비교하여 일치하는 경우, 해당 레코드의 키와 값을 출력.

결과 플래그: 키를 찾으면 r 변수를 1로 설정하고, 루프를 종료.

검색 결과 출력: 모든 페이지를 검색한 후, 키를 찾지 못한 경우 실패 메시지를 출력.
</details>
</details>
o : 백업 파일 가져오기

b : 순차 텍스트 파일로 백업

n : 데이터 파일 초기화

## 4. 📊결과

![btreea](https://github.com/user-attachments/assets/81146467-1a97-41d6-9bd4-b3db8c75255b)
