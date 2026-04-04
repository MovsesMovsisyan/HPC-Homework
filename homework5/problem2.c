#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define SIZE 10000
#define THREADS 4

typedef struct
{
    int order_id;
    int distance_km;
    int priority;
} Order;

Order orders[SIZE];
int thread_high_count[THREADS];

int main()
{
    int threshold;

    for(int i = 0; i < THREADS; i++)
        thread_high_count[i] = 0;

    #pragma omp parallel num_threads(THREADS)
    {
        int tid = omp_get_thread_num();

        #pragma omp single
        {
            threshold = 20;

            for(int i = 0; i < SIZE; i++)
            {
                orders[i].order_id = i;
                orders[i].distance_km = rand() % 50;
                orders[i].priority = 0;
            }
        }

        #pragma omp for
        for(int i = 0; i < SIZE; i++)
        {
            if(orders[i].distance_km < threshold)
                orders[i].priority = 1;

            else
                orders[i].priority = 0;
        }

        #pragma omp barrier

        #pragma omp single
        {
            printf("priority assignment finished\n");
        }

        #pragma omp for
        for(int i = 0; i < SIZE; i++)
        {
            if(orders[i].priority == 1)
                thread_high_count[tid]++;
        }

        #pragma omp barrier

        #pragma omp single
        {
            int total = 0;

            for(int i = 0; i < THREADS; i++)
            {
                printf("thread %d HIGH: %d\n", i, thread_high_count[i]);
                total += thread_high_count[i];
            }

            printf("total HIGH priority orders: %d\n", total);
        }
    }

    return 0;
}
