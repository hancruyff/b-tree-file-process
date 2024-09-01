#ifndef __BTREE_
#define __BTREE_
#include "BaseHeader.h"
#include "BufferManager.h"
#include "BTreePage.h"
void initBTree(char *name, int pageSize, BOOL newStart);
	/*B+ -트리를 초기화*/
BOOL insertRecord(BTreeRecord* newRecord);
	/*(key, value)의 레코드를 삽입*/
BOOL deleteRecord(Key key);
	/*key를 가진 레코드를 삭제*/
BOOL retrieveRecord(Key key, BTreeRecord* record);
	/*key를 가진 레코드를 인덱스셋을 통해 검색하고, record에 결과를 저장한다*/
BOOL findRecord(Key key, BTreePagePtr page);
	/*key를 가진 레코드가 포함된 노드를 page에 저장한다*/
void closeBTree();
	/*B+-트리 헤더를 디스크에 저장하고 화일을 닫는다 */
void Sequential_Search(void);
	/*B+-트리 헤더를 디스크에 저장하고 화일을 닫는다 */
void Get_Header(void);
	/*B+-트리 헤더를 디스크에 저장하고 화일을 닫는다 */
void retrieveAllPages();
/*B+-트리 전체 페이지 읽기 */
void Sequential_Backup();
/*B+-트리 전체 택스트 백업 */
void push(int pageNo, int index);
	/*스택에 pageNo와 index를 push*/
STACK* pop(); /*스택의 top을 pop*/
STACK* peek(); /*스택의 top을 반환함*/
#endif

