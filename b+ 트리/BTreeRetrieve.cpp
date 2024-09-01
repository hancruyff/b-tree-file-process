#include "BTree.h"
extern BufferManager *bufferManager;
extern BTreeHeader *bTreeHeader;
BOOL retrieveRecord(Key key, BTreeRecord *record) {
	/*인덱스 셋으로 부터 레코드 검색*/
	BTreePagePtr page=(BTreePagePtr)malloc(bufferManager->pageSize);
	int i=0;
	BOOL found=FALSE;
	if(findRecord(key, page)) {
		i=peek()->index;
		copyRecord(RECORDPTR(page)+i, record);
		found=TRUE;
	}
	free(page);
	return found;
}
BOOL findRecord(Key key, BTreePagePtr page) {
	/*Key를 가진 Record를 찾아가는 경로를 스택에 push하고
			도달한 Leaf를 page에 저장한다*/
	int i=0, targetPage=bTreeHeader->rootPage;
	bTreeHeader->stackPtr=0;
	readBTreePage(targetPage, page);
	while(ISLEAF(page) == FALSE) {
		for (i = 0; (i < KEYCNT(page)) && (KEY(page, i) < key); i++) {
			;
		}
		push(PAGENO(page), i);
		targetPage=CHILD(page, i);
		readBTreePage(targetPage, page);
	}
	for (i = 0; (i < KEYCNT(page)) && (RECORD(page, i).key < key); i++) {
		;
		}
	

	push(PAGENO(page), i);

	if((i < KEYCNT(page)) && (key == RECORD(page, i).key)) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

void Sequential_Backup(void) {
	char strPath[] = { "backup.txt" };
	int nResult = remove(strPath);
	FILE  *fp_backup;
	fp_backup = fopen("backup.txt", "w");
	BTreePagePtr page = (BTreePagePtr)malloc(bufferManager->pageSize);
	int i = 1;
	while (readBTreePage(i, page)) {
		if (ISLEAF(page) == FALSE) {

		}
		else {


			for (int j = 0; (j < KEYCNT(page)); j++) {
				fprintf(fp_backup, "%d %s\n", RECORD(page, j).key, RECORD(page, j).value);

			}

		}
		i++;
	}
	printf("백업 성공\n");
	fclose(fp_backup);
}
void Sequential_Search(void) {
	BTreePagePtr page = (BTreePagePtr)malloc(bufferManager->pageSize);
	int i = 1;
	int Searchkey;
	int r = 0;
	scanf("%d", &Searchkey);
	while (readBTreePage(i, page)) {
		if (ISLEAF(page) == FALSE) {
			
		}
		else {
			
			
			for (int j = 0; (j < KEYCNT(page)); j++) {
				if (Searchkey == RECORD(page, j).key) {
					printf("%d %s\n", RECORD(page, j).key, RECORD(page, j).value);
					r = 1;
					break;
				}
				
			}
			
		}
		i++;
	}
	if(r==0){
		printf("Sequential_Search (%d) : fail\n", Searchkey);
		}
}
void Get_Header(void) {
	/*헤더정보 가져오기*/
	printf("루트노드 페이지번호   : %d\n", bTreeHeader->rootPage);
	printf("리프노드 첫페이지번호 : %d\n", bTreeHeader->firstSequencePage);
	printf("B+ 트리의 차수        : %d\n", bTreeHeader->order); //(int)((pageSize - (정수4바이트) * 4) / (정수4바이트) * 2)) + 1;
	printf("내부노드 최소키 개수  : %d\n", bTreeHeader->minKey); //order / 2 - 1 + order % 2;
	printf("리프의 레코드 최대수  : %d\n", bTreeHeader->maxRecord); //pageSize - (정수4바이트) * 3) / (정수4바이트);
	printf("리프노드 최소키 개수  : %d\n", bTreeHeader->minRecord); // bTreeHeader->maxRecord / 2

}

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

