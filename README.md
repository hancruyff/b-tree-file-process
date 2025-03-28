# 🌳 B+ Tree File Processing System

**C++로 구현한 디스크 기반 B+ 트리 삽입/삭제/검색 시스템**  
파일 단위로 노드를 저장/로드하며, B+ 트리 구조를 기반으로 효율적인 검색과 데이터 삽입을 처리합니다.

---

## 📂 목차

- [1. 개요](#1-개요)
- [2. B+ 트리란?](#2-b-트리란)
- [3. 프로젝트 구조](#3-프로젝트-구조)
- [4. 기능 설명](#4-기능-설명)
- [5. 주요 코드 구성](#5-주요-코드-구성)
- [6. 실행 방법](#6-실행-방법)
- [7. 기술 스택](#7-기술-스택)

---

## 1. 📌 개요

이 프로젝트는 **파일 기반 B+ 트리 인덱스 구조**를 직접 구현한 C++ 프로그램입니다.  
`삽입`, `삭제`, `탐색`, `리프 노드 연결`, `디스크 저장 및 버퍼 관리`까지 포함된 **간단한 DB 인덱싱 시스템의 핵심 로직**을 담고 있습니다.

---

## 2. 🌲 B+ 트리란?

**B+ 트리(B+ Tree)** 는 데이터베이스에서 대량의 데이터를 디스크에 저장하며 검색할 때 사용되는 대표적인 **균형 인덱스 트리 구조**입니다.

- 모든 데이터는 **리프 노드**에만 저장됨
- **리프 노드들은 포인터로 연결**되어 있어 범위 검색이 빠름
- **삽입/삭제 시 자동 균형 유지**
- 파일 시스템, DBMS에서 **인덱스 구조**로 널리 사용

> 이 프로젝트는 B+ 트리 구조를 구현하고, 그 노드를 디스크 파일에 저장하여 입출력 효율을 테스트하는 목적입니다.

---

## 3. 🧱 프로젝트 구조
```
b+ 트리/
├── BTree.cpp / BTree.h           → 트리 전체 클래스 (삽입, 삭제, 탐색 등)
├── BTreeInsert.cpp               → 삽입 로직 분리 구현
├── BTreeDelete.cpp               → 삭제 로직 분리 구현
├── BTreeRetrieve.cpp             → 키 검색 로직
├── BTreePage.cpp / BTreePage.h   → 페이지 구조 (노드 표현)
├── BufferManager.cpp / .h        → 파일 IO / 페이지 캐싱 처리
├── BaseHeader.h                  → 공통 상수 및 타입 정의
├── BTreeMain.cpp                 → main 함수 및 메뉴 구성
├── data.txt                      → 저장될 레코드 파일
├── backup.txt                    → 트리 복사 백업
```

---

## 4. 🔧 기능 설명

| 기능          | 설명 |
|---------------|------|
| 키 삽입       | 새로운 키를 삽입하며 노드 분할 및 부모 갱신 처리 |
| 키 삭제       | 삭제 후 underflow 발생 시 병합 또는 차용 처리 |
| 키 검색       | 트리를 타고 내려가서 리프 노드에서 키 탐색 수행 |
| 범위 검색     | 리프 노드 연결 포인터를 이용해 연속적인 값 탐색 |
| 파일 입출력   | 각 페이지(노드)를 파일로 저장/불러오기 처리 |
| 버퍼 매니저   | 자주 접근되는 페이지를 캐시에 유지하여 디스크 접근 최소화 |

---

## 5. 🔍 주요 코드 구성

### B+ 트리 삽입

```cpp
void BTree::Insert(int key, int recordOffset) {
    PageId rootPid = header->rootPageId;
    KeyType promotedKey;
    PageId newChildPid;
    
    // 재귀 삽입 → 분할 발생 시 부모에게 승격
    bool split = InsertInternal(rootPid, key, recordOffset, promotedKey, newChildPid);

    if (split) {
        // 새 루트 생성
        PageId newRoot = AllocatePage();
        InternalPage root(newRoot, true);
        root.Init();
        root.Insert(promotedKey, rootPid);
        root.SetRightMostPageId(newChildPid);
        header->rootPageId = newRoot;
        WriteHeader();
    }
}
```

### 키 검색
```cpp
bool BTree::Search(int key, RecordId &rid) {
    PageId current = header->rootPageId;
    while (!IsLeaf(current)) {
        InternalPage page(current);
        current = page.LookupChild(key);
    }
    LeafPage leaf(current);
    return leaf.Lookup(key, rid);
}
```

노드 구조 정의 (BTreePage.h)
```cpp
struct LeafPage {
    int keys[MAX];
    int values[MAX];   // 레코드 위치
    int nextPageId;    // 다음 리프 노드 포인터
};

struct InternalPage {
    int keys[MAX];
    int children[MAX + 1]; // 자식 노드 페이지 ID
};
```

### 6. 🛠 기술 스택
<img src="https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white"/> <img src="https://img.shields.io/badge/Windows%20API-0078D7?style=for-the-badge&logo=windows&logoColor=white"/> <img src="https://img.shields.io/badge/File%20IO-%231970D6?style=for-the-badge"/>

C++: 전반적인 알고리즘 및 구조 구현

파일 입출력 (fstream): 페이지 단위 저장 및 불러오기

버퍼 매니저: 페이지 캐시 및 최소한의 디스크 접근을 위한 로직 포함

데이터 인덱싱: 직접 구현한 B+ Tree를 통해 정렬 및 탐색

### 7. 📌 학습 포인트

✔️ 트리 구조 재귀적 설계 및 노드 분할/병합 처리

✔️ 디스크 기반 자료구조 처리 방식 이해

✔️ Leaf 노드의 LinkedList 구조로 범위 검색 구현

✔️ 버퍼 관리 및 페이지 교체 전략 설계 경험
