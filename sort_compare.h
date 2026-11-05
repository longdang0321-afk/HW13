#ifndef SORT_COMPARE_H
#define SORT_COMPARE_H

#include <stddef.h>

/* 피벗 선택 방법 */
typedef enum
{
    FIRST,
    RANDOM,
    MEDIAN3
} PivotMode;

/* 한 번의 정렬 결과 */
typedef struct
{
    unsigned long long comparisons; /* 데이터 값 비교 횟수 */
    int max_depth;                  /* 최대 재귀 호출 깊이 */
    size_t memory_bytes;            /* 추가 메모리 추정값 */
} SortResult;

void merge_sort(int a[], int n, SortResult *result);
void quick_sort(int a[], int n, PivotMode mode, SortResult *result);
int is_sorted(const int a[], int n);

#endif
