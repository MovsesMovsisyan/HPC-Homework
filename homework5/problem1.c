#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define SIZE 20000

typedef struct
{
    int request_id;
    int user_id;
    int response_time_ms;
} Log;

Log logs[SIZE];
int category[SIZE];

int main()
{
    int fast = 0;
    int medium = 0;
    int slow = 0;

    #pragma omp parallel num_threads(4)
    {
        #pragma omp single
        {
            for(int i = 0; i < SIZE; i++)
            {
                logs[i].request_id = i;
                logs[i].user_id = rand() % 1000;
                logs[i].response_time_ms = rand() % 500;
            }
        }

        #pragma omp barrier

        #pragma omp for
        for(int i = 0; i < SIZE; i++)
        {
            if(logs[i].response_time_ms < 100)
                category[i] = 0;

            else if(logs[i].response_time_ms <= 300)
                category[i] = 1;

            else
                category[i] = 2;
        }

        #pragma omp barrier

        #pragma omp single
        {
            for(int i = 0; i < SIZE; i++)
            {
                if(category[i] == 0)
                    fast++;

                else if(category[i] == 1)
                    medium++;

                else
                    slow++;
            }

            printf("FAST: %d\n", fast);
            printf("MEDIUM: %d\n", medium);
            printf("SLOW: %d\n", slow);
        }
    }

    return 0;
}
