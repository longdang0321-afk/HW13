#include <stdio.h>
#include <stdlib.h>
#include "sort_compare.h"

/* 두 값을 교환한다. */
static void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

/* 값 비교 횟수를 증가시킨 뒤 비교 결과를 반환한다. */
static int less_than(int a, int b, SortResult *result)
{
    result->comparisons++;
    return a < b;
}

static int choose_pivot(int a[], int left, int right,
                        PivotMode mode, SortResult *result)
{
    if (mode == FIRST)
    {
        return left;
    }
    if (mode == RANDOM)
    {
        return left + rand() % (right - left + 1);
    }

    /* 세 인덱스를 해당 값의 크기순으로 정렬한다. */
    int first = left;
    int middle = left + (right - left) / 2;
    int last = right;

    if (less_than(a[middle], a[first], result))
    {
        swap(&middle, &first);
    }
    if (less_than(a[last], a[middle], result))
    {
        swap(&last, &middle);
    }
    if (less_than(a[middle], a[first], result))
    {
        swap(&middle, &first);
    }
    return middle;
}

static int partition(int a[], int left, int right,
                     PivotMode mode, SortResult *result)
{
    int pivot_index = choose_pivot(a, left, right, mode, result);
    swap(&a[pivot_index], &a[right]);

    int pivot = a[right];
    int small = left;

    for (int j = left; j < right; j++)
    {
        if (less_than(a[j], pivot, result))
        {
            swap(&a[small], &a[j]);
            small++;
        }
    }
    swap(&a[small], &a[right]);
    return small;
}

static void quick_recursive(int a[], int left, int right,
                            PivotMode mode, SortResult *result, int depth)
{
    if (depth > result->max_depth)
    {
        result->max_depth = depth;
    }
    if (left >= right)
    {
        return;
    }

    int p = partition(a, left, right, mode, result);
    quick_recursive(a, left, p - 1, mode, result, depth + 1);
    quick_recursive(a, p + 1, right, mode, result, depth + 1);
}

/* 정렬된 두 구간을 임시 배열에 병합한다. */
static void merge(int a[], int temp[], int left, int middle,
                  int right, SortResult *result)
{
    int i = left;
    int j = middle + 1;
    int k = left;

    while (i <= middle && j <= right)
    {
        result->comparisons++;
        if (a[i] <= a[j])
        {
            temp[k++] = a[i++];
        }
        else
        {
            temp[k++] = a[j++];
        }
    }
    while (i <= middle)
    {
        temp[k++] = a[i++];
    }
    while (j <= right)
    {
        temp[k++] = a[j++];
    }
    for (int index = left; index <= right; index++)
    {
        a[index] = temp[index];
    }
}

static void merge_recursive(int a[], int temp[], int left, int right,
                            SortResult *result, int depth)
{
    if (depth > result->max_depth)
    {
        result->max_depth = depth;
    }
    if (left >= right)
    {
        return;
    }

    int middle = left + (right - left) / 2;
    merge_recursive(a, temp, left, middle, result, depth + 1);
    merge_recursive(a, temp, middle + 1, right, result, depth + 1);
    merge(a, temp, left, middle, right, result);
}

/* 메모리는 실제 RAM 측정값이 아니라 아래의 변수 크기 모델이다.
 * 재귀 공간 = 최대 깊이 * 재귀 함수 1회의 매개변수/지역 변수 크기.
 * 여기에 진입 함수와 보조 함수의 변수 공간도 더한다.
 * 보조 함수 공간은 최대로 중첩 가능한 경로의 추정값을 사용한다.
 * 반환 주소, 레지스터 저장, 패딩, 라이브러리 내부 공간은 제외한다.
 * 실제로는 최대 깊이와 보조 함수 최대 공간의 발생 시점이 다를 수 있다.
 * 따라서 아래 값은 계산 모델의 추정값이며 실제 최대 스택과 같지 않다. */
void quick_sort(int a[], int n, PivotMode mode, SortResult *result)
{
    result->comparisons = 0;
    result->max_depth = 0;
    result->memory_bytes = 0;

    quick_recursive(a, 0, n - 1, mode, result, 1);

    /* 재귀 1회: a/result 포인터, left/right/depth/p 정수, mode */
    size_t recursive_bytes = sizeof(int *) + sizeof(SortResult *) + 4 * sizeof(int) + sizeof(PivotMode);

    /* partition -> choose_pivot -> swap 경로의 변수 공간 */
    size_t partition_bytes = sizeof(int *) + sizeof(SortResult *) + 6 * sizeof(int) + sizeof(PivotMode);
    size_t pivot_bytes = sizeof(int *) + sizeof(SortResult *) + 5 * sizeof(int) + sizeof(PivotMode);
    size_t swap_bytes = 2 * sizeof(int *) + sizeof(int);

    /* 진입 함수의 매개변수와 메모리 계산용 지역 변수 */
    size_t wrapper_bytes = sizeof(int *) + sizeof(SortResult *) + sizeof(int) + sizeof(PivotMode) + 5 * sizeof(size_t);
    result->memory_bytes = wrapper_bytes + (size_t)result->max_depth * recursive_bytes + partition_bytes + pivot_bytes + swap_bytes;
}

void merge_sort(int a[], int n, SortResult *result)
{
    result->comparisons = 0;
    result->max_depth = 0;
    result->memory_bytes = 0;

    if (n <= 0)
    {
        return;
    }

    int *temp = malloc((size_t)n * sizeof(int));
    if (temp == NULL)
    {
        fprintf(stderr, "Merge Sort: memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    merge_recursive(a, temp, 0, n - 1, result, 1);

    /* 재귀 1회: a/temp/result 포인터, left/right/depth/middle 정수 */
    size_t recursive_bytes = 2 * sizeof(int *) + sizeof(SortResult *) + 4 * sizeof(int);
    /* merge: a/temp/result 포인터와 정수 7개 */
    size_t merge_bytes = 2 * sizeof(int *) + sizeof(SortResult *) + 7 * sizeof(int);
    size_t wrapper_bytes = 2 * sizeof(int *) + sizeof(SortResult *) + sizeof(int) + 3 * sizeof(size_t);

    result->memory_bytes = (size_t)n * sizeof(int) + wrapper_bytes + (size_t)result->max_depth * recursive_bytes + merge_bytes;
    free(temp);
}

int is_sorted(const int a[], int n)
{
    for (int i = 1; i < n; i++)
    {
        if (a[i - 1] > a[i])
        {
            return 0;
        }
    }
    return 1;
}
