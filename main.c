#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sort_compare.h"

#define MAX_SIZE 20000
#define DATA_SEED 2026u
#define PIVOT_SEED 12026u
#define REPEAT 400

static int original[MAX_SIZE];
static int work[MAX_SIZE];

static void generate_data(int a[], int n)
{
    srand(DATA_SEED);
    for (int i = 0; i < n; i++)
    {
        a[i] = rand() % 100000;
    }
}

int main(void)
{
    int sizes[] = {1000, 2000, 4000, 8000, 16000, 20000};
    const char *names[] = {
        "Merge Sort", "Quick (First)", "Quick (Random)", "Quick (Median3)"};

    printf("Data seed : %u\n", DATA_SEED);
    printf("Pivot seed: %u\n", PIVOT_SEED);
    printf("Repeat : %d\n", REPEAT);
    printf("Timer : clock() CPU time\n");
    printf("Memory : estimated (temporary array + recursion/variable model)\n\n");

    for (int i = 0; i < 6; i++)
    {
        int n = sizes[i];
        generate_data(original, n);

        printf("Data Size : %d\n", n);
        printf("%-17s %14s %12s %15s\n",
               "Algorithm", "Comparisons", "Time(ms)", "Memory(Bytes)");
        printf("--------------------------------------------------------------\n");

        for (int method = 0; method < 4; method++)
        {
            SortResult result;
            double total_ms = 0.0;

            /* 같은 정렬을 반복하여 평균 실행시간을 구한다. */
            for (int repeat = 0; repeat < REPEAT; repeat++)
            {
                memcpy(work, original, (size_t)n * sizeof(int));
                srand(PIVOT_SEED);

                /* 복사와 난수 초기화는 시간 측정에서 제외한다. */
                clock_t start = clock();
                if (start == (clock_t)-1)
                {
                    fprintf(stderr, "CPU timer unavailable.\n");
                    return EXIT_FAILURE;
                }

                if (method == 0)
                {
                    merge_sort(work, n, &result);
                }
                else
                {
                    quick_sort(work, n, (PivotMode)(method - 1), &result);
                }

                clock_t end = clock();
                if (end == (clock_t)-1 || end < start)
                {
                    fprintf(stderr, "CPU timer error.\n");
                    return EXIT_FAILURE;
                }
                total_ms += (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;

                /* 검증 시간은 정렬 시간에 포함하지 않는다. */
                if (!is_sorted(work, n))
                {
                    fprintf(stderr, "Sorting verification failed.\n");
                    return EXIT_FAILURE;
                }
            }

            printf("%-17s %14llu %12.6f %15zu\n",
                   names[method], result.comparisons,
                   total_ms / REPEAT, result.memory_bytes);
        }
        printf("Verification : PASS\n\n");
    }
    return 0;
}
