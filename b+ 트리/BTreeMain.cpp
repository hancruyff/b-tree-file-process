// BTreeMain.c
/* 명령 유형
i : (키,값)을 가지는 레코드 삽입
d : 키를 가지는 레코드 삭제
r : 키를 가지는 레코드 검색
s : 순차탐색
? : 헤더정보보기
a : 모든 페이지 보기(1페이지부터)
*/
#include "BTree.h"
#define PAGE_SIZE 64
int main() {
	FILE *fp, *fp_backup;
	BTreeRecord record;
	char command;
	BOOL file_not_Exist;
	fp = fopen("data.txt", "r");   //트리저장 화일 존재하는지 체크
	if (fp == NULL) {
		file_not_Exist = TRUE;     //없으면 참
		fp = fopen("data.txt", "w"); //화일 새로 생성
	}
	else
		file_not_Exist =FALSE;
	fclose(fp);
	 	initBTree("data.txt", PAGE_SIZE, file_not_Exist); //새로 만들었으면 초기화. 헤더페이지와 루트페이지 생성
	//fp_backup = fopen("backup.txt", "w");
	while (1) {
		fscanf(stdin, "%c", &command);
		if (command == 'x' || command == 'X') break;
		switch (command) {
		case 'i':    //(키,값)을 가지는 레코드 삽입
			fscanf(stdin, "%i %s", &record.key, &record.value);
			if (insertRecord(&record)) {
				printf("insert (%d, %s) : success\n", record.key, record.value);
			}
			else {
				printf("Insert (%d, %s) : fail\n", record.key, record.value);
			}
			break;
		case 'd' :     //키를 가지는 레코드 삭제
			fscanf(stdin, "%i", &record.key);
			if (deleteRecord(record.key)) {
				printf("Delete (%d) : success\n", record.key, record.value);
			} else {
				printf("Delete (%d) : fail\n", record.key);
			}
			break;
		case 'r':    //키를 가지는 레코드 검색
			fscanf(stdin, "%i", &record.key);
			if (retrieveRecord(record.key, & record)) {
				printf(
					"Retrive (%d, %s) : success\n",
					record.key,
					record.value);
			} else {
				printf("Retrive (%d) : fail\n", record.key);
			}
			break;
		case 'a':    //모든페이지 검색
			retrieveAllPages();
			break;
		case 'h':    //헤더정보 보기
			Get_Header();
			break;
		case 's':    //순차검색
			
			Sequential_Search();
			break;
		case 'o':    //백업화일 가져오기
			closeBTree();
			fp_backup=fopen("backup.txt", "r");
			if (fp_backup == NULL) {
				file_not_Exist = TRUE;
				printf("백업파일이 없습니다\n");
				fp = fopen("data.txt", "r");   //트리저장 화일 존재하는지 체크
				fclose(fp);
				initBTree("data.txt", PAGE_SIZE, FALSE);
				break;
			}
			else

			fp = fopen("data.txt", "w");
			initBTree("data.txt", PAGE_SIZE, TRUE);
			printf("백업파일 가져오기 성공\n");
			fclose(fp);
			int ret;
			while (1) {

				ret = fscanf(fp_backup, "%d %s", &record.key, &record.value);
				if (ret == EOF) //fscanf 함수는 파일끝에 도달하거나,오류가 발생시 EOF를 반환한다                  // 15행
					break;
				insertRecord(&record);
			}
			fclose(fp_backup);
			break;
		case 'b':    //순차택스트화일(backup.txt)로 백업하기
			Sequential_Backup();
			break;
		case 'n':    //데이터화일 초기화
			closeBTree();
			char strPath[] = { "data.txt" };
			int nResult=remove(strPath);
			if (nResult == 0)
			{
				printf("데이터 파일 초기화 성공\n");
			}
			else if (nResult == -1)
			{
				perror("데이터 파일 초기화 실패\n");
			}
			fclose(fp);
			fp = fopen("data.txt", "r");   //트리저장 화일 존재하는지 체크
			
			file_not_Exist = TRUE;     //없으면 참
			fp = fopen("data.txt", "w"); //화일 새로 생성
				
			
			
			
			fclose(fp);
			initBTree("data.txt", PAGE_SIZE, file_not_Exist);
			fp = fopen("data.txt", "r");
			break;

		}
	}
	closeBTree();
	return 0;
}