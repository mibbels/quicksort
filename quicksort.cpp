#include <iostream>
#include <iomanip>
#include <chrono>
#include <stdlib.h> //qsort()

#include <omp.h>

void swap(int* x, int* y) {
    int t = *x;

    *x = *y;
    *y = t;
}

int cmp(const void* a, const void* b) {
    int x = *(int*)a;
    int y = *(int*)b;

    return x - y;
}
//	========================================================
//
// 		parallel Quicksort
//
//  ========================================================

int neutralize(int* left, int* right, int p, int blocksize)
{
    int i = 0,j = 0; //correct ??

    while(i < blocksize && j < blocksize)
    {
        for (i = 0; i < blocksize; i++)
            if (left[i] > p ) break;

        for (j = 0; j < blocksize; j++)
            if (left[j] < p ) break;

        if(i == blocksize || j == blocksize) break;

        swap(&left[i], &right[j]);

        i++;
        j++;
    }

    if (i == blocksize && j == blocksize) return 2;
    if (j == blocksize) return 1; // changed correct?
    return 0;
}


////  TODO check for end condition
int* get_block_right(int* a, int blocksize, int right, int size)
{
    int* block = (int*) malloc(blocksize * sizeof(int));
    for (int i = size - right * blocksize; i > size - (right + 1) * blocksize; i--)
    {
        block[i] = a[i];
    }
    return block;
}

////  TODO check for end condition
int* get_block_left(int* a, int blocksize, int left)
{
    int* block = (int*) malloc(blocksize * sizeof(int));
    for (int i = left * blocksize; i < (left + 1) * blocksize; i++)
    {
        block[i] = a[i];
    }
    return block;
}

void partition_parallel(int* a, int &l, int &r, int &left, int &right, int &sum_l, int &sum_r, int &blocksize, int &size, int &p, int** remaining)
{
    int thread_id = omp_get_thread_num();
    int tmp_left, tmp_right;
    int* leftblock;
    int* rightblock;

    if(thread_id == 0)
    {
        p = a[l];
    }
    #pragma omp barrier

    #pragma omp critical
    {
        tmp_left = left;
        left++;
    };
    leftblock = get_block_left(a, blocksize, tmp_left);

    #pragma omp critical
    {
        tmp_right = right;
        right++;
    };
    rightblock = get_block_right(a, blocksize, tmp_right, size);

     while (leftblock != NULL && rightblock != NULL) {
         int res = neutralize(leftblock, rightblock, p, blocksize);
         if (res == 1 || res == 2)
         {
            #pragma omp critical
             {
                 tmp_left = left + 1;
                 left++;
             };
             leftblock = get_block_left(a, blocksize, tmp_left);
         }
         if (res == 0 || res == 2) {
            #pragma omp critical
             {
                 tmp_right = right + 1;
                 right++;
             };
             rightblock = get_block_right(a, blocksize, tmp_right, size);
         }
     }

     if (leftblock != NULL)
        remaining[thread_id] = leftblock;

     if (rightblock != NULL)
        remaining[thread_id] = rightblock;

     sum_l += left * blocksize;
     sum_r += right * blocksize;
}

int qs_parallel(int* a, int l, int r, int size)
{
    int left = 0, right = 0, sum_l = 0, sum_r = 0, p = 0;
    int blocksize = 1024;
    int n_threads = 4;
    int** remaining = (int**)malloc(n_threads * sizeof(int*));

    #pragma omp parallel shared(left, right, sum_l, sum_r, a , l, r, blocksize, size, p, remaining) default(none) num_threads(n_threads)
    {
        partition_parallel(a, l, r, left, right, sum_l, sum_r, blocksize, size, p, remaining);
    }

    return 0;
}

//	========================================================
//
// 		3 Way Split Quicksort
//
//  ========================================================
void partition_3way(int* a, int l, int r, int* ml, int* mr) {
    int p = a[l], i = l + 1, j = r;
    int r2 = r, l2 = l + 1;
    do
    {
        while (i <= r2 && a[i] < p) i++;
        while (j > l2 && a[j] > p)  j--;

        if (i < j)
        {
            if (a[i] == p && a[j] == p)
            {
                swap(&a[i], &a[l2]);
                l2++;
                swap(&a[j], &a[r2]);
                r2--;
            }
            else if (a[i] == p && a[j] < p)
            {
                swap(&a[i], &a[j]); // move j left
                // i now at pos. j
                swap(&a[j], &a[r2]);
                r2--;
            }
            else if (a[i] > p && a[j] == p)
            {
                swap(&a[j], &a[i]); // move i right
                // j now at pos. i
                swap(&a[i], &a[l2]);
                l2++;
            }
            else
            {
                swap(&a[i], &a[j]);
            }
            i++;
            j--;
        }

    } while (i < j);

    if (i == j)
    {
        if (a[i] > p)	    j--;
        else if (a[i] < p)	i++;
        else if (a[i] == p)
        {
            j--;
            i++;
        }
    }

    for (int k = l; k < l2; k++)
    {
        swap(&a[k], &a[j]);
        j--;
    }

    for (int k = r; k > r2; k--)
    {
        swap(&a[k], &a[i]);
        i++;
    }

    *ml = j;
    *mr = i;
}

void qs_3way(int* a, int l, int r) {
    if (l < r)
    {
        int ml, mr;
        partition_3way(a, l, r, &ml, &mr);
        qs_3way(a, l, ml);
        qs_3way(a, mr, r);
    }
}

//	========================================================
//
// 		Quicksort
//
//  ========================================================

int partition(int* a, int l, int r) {
    int p = a[l], i = l + 1, j = r;
    do
    {
        while (i < r && a[i] <= p) i++;
        while (j > l && a[j] >= p) j--;

        if (i < j) swap(&a[i], &a[j]);
    } while (j > i);

    swap(&a[l], &a[j]);

    return j;
}

void qs(int* a, int l, int r) {
    int m;
    if (l < r)
    {
        m = partition(a, l, r);
        qs(a, l, m - 1);
        qs(a, m + 1, r);

    }
}

// -------------------------------------------------------------
int main(void)
{
    std::chrono::duration<double> diff_lib, diff_qs, diff_3way, diff_parallel;
    int n, *arr_1, *arr_2, *arr_3, *arr_4;

    std::cout << std::setw(10) << "n" << std::setw(10) << "lib" << std::setw(10) << "qs" << std::setw(10) << "3way" << std::endl;

    for (int i = 10; i <= 27; i++) {
        n = 1 << i;

        arr_1 = (int*)malloc(n * sizeof(int));
        arr_2 = (int*)malloc(n * sizeof(int));
        arr_3 = (int*)malloc(n * sizeof(int));
        arr_4 = (int*)malloc(n * sizeof(int));

        if (!arr_1 || !arr_2 || !arr_3)
        {
            std::cout << "error allocating memory" << std::endl;
            return -1;
        }

        for (int j = 0; j < n; j++) {
            //arr_1[j] = (rand() << 15) + rand();
            arr_1[j] = (int) ((rand() << 15) + rand()) %100000;
            arr_2[j] = arr_1[j];
            arr_3[j] = arr_1[j];
            arr_4[j] = arr_1[j];
        }

        auto time_start = std::chrono::system_clock::now();
        //qsort(arr_1, n, sizeof(int), cmp);
        auto time_end = std::chrono::system_clock::now();
        diff_lib = time_end - time_start;

        time_start = std::chrono::system_clock::now();
        //qs(arr_2, 0, n - 1);
        time_end = std::chrono::system_clock::now();
        diff_qs = time_end - time_start;

        time_start = std::chrono::system_clock::now();
        //qs_3way(arr_3, 0, n - 1);
        time_end = std::chrono::system_clock::now();
        diff_3way = time_end - time_start;

        time_start = std::chrono::system_clock::now();
        qs_parallel(arr_4, 0, n - 1, n);
        time_end = std::chrono::system_clock::now();
        diff_parallel = time_end - time_start;


        std::cout << std::setprecision(3) << std::setw(10) << "2^" << i << std::setw(10) << diff_lib.count() << std::setw(10) <<
                  diff_qs.count() << std::setw(10) << diff_3way.count() << std::endl;

        /*
        for (int i = 1; i < n; i++)
        {
            std::cout << arr_3[i] << " ";
        }
        */

        free(arr_1);
        free(arr_2);
        free(arr_3);
    }

    return 0;
}
