#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <immintrin.h>
#include <string.h>

#define N 10
#define SIZE 200000000

pthread_t threads[N];
pthread_t threads_simd[N];

double now_sec(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

char *create_buffer()
{
    char *buffer = (char *)malloc(SIZE);

    if (buffer == NULL)
    {
        perror("memory allocation failed");
        exit(1);
    }

    for (uint64_t i = 0; i < SIZE; i++)
    {
        int r = rand() % 5;

        if (r == 0)
            buffer[i] = 'a' + rand() % 26;

        else if (r == 1)
            buffer[i] = 'A' + rand() % 26;

        else if (r == 2)
            buffer[i] = '0' + rand() % 10;

        else if (r == 3)
            buffer[i] = ' ';

        else
            buffer[i] = '!';
    }

    return buffer;
}

void *multithread_uppercase(void *ptr)
{
    char *start = (char *)ptr;

    for (uint64_t i = 0; i < SIZE / N; i++)
    {
        if (start[i] >= 'a' && start[i] <= 'z')
        {
            start[i] -= 32;
        }
    }

    return NULL;
}

void SIMD_uppercase(unsigned char *buf, size_t n)
{
    size_t i = 0;

    const __m256i a = _mm256_set1_epi8('a' - 1);
    const __m256i z = _mm256_set1_epi8('z' + 1);
    const __m256i diff = _mm256_set1_epi8(32);

    size_t limit = n & ~(size_t)31;

    for (; i < limit; i += 32)
    {
        __m256i v = _mm256_loadu_si256((__m256i *)(buf + i));

        __m256i ge_a = _mm256_cmpgt_epi8(v, a);
        __m256i le_z = _mm256_cmpgt_epi8(z, v);

        __m256i mask = _mm256_and_si256(ge_a, le_z);

        __m256i sub = _mm256_and_si256(mask, diff);

        v = _mm256_sub_epi8(v, sub);

        _mm256_storeu_si256((__m256i *)(buf + i), v);
    }

    for (; i < n; i++)
    {
        if (buf[i] >= 'a' && buf[i] <= 'z')
        {
            buf[i] -= 32;
        }
    }
}

typedef struct
{
    unsigned char *buf;
    size_t n;

} simd_args;

void *simd_thread_function(void *ptr)
{
    simd_args *args = (simd_args *)ptr;

    SIMD_uppercase(args->buf, args->n);

    return NULL;
}

int main()
{
    char *buffer_original = create_buffer();

    char *buffer_mt = malloc(SIZE);
    if (buffer_mt == NULL) {
	    	fprintf(stderr, "Memory allocation failed!\n");
        	exit(1);
    	}
    char *buffer_simd = malloc(SIZE);
    if (buffer_simd == NULL) {
	    	fprintf(stderr, "Memory allocation failed!\n");
        	exit(1);
    	}
    char *buffer_simd_mt = malloc(SIZE);
    if (buffer_simd_mt == NULL) {
	    	fprintf(stderr, "Memory allocation failed!\n");
        	exit(1);
    	}

    memcpy(buffer_mt, buffer_original, SIZE);
    memcpy(buffer_simd, buffer_original, SIZE);
    memcpy(buffer_simd_mt, buffer_original, SIZE);

    double start;
    double end;

    printf("Multithreading\n");

    start = now_sec();

    for (int i = 0; i < N; i++)
    {
        pthread_create(&threads[i], NULL,
                       multithread_uppercase,
                       buffer_mt + i * (SIZE / N));
    }

    if (SIZE % N != 0)
    {
        for (uint64_t i = N * (SIZE / N); i < SIZE; i++)
        {
            if (buffer_mt[i] >= 'a' && buffer_mt[i] <= 'z')
            {
                buffer_mt[i] -= 32;
            }
        }
    }

    for (int i = 0; i < N; i++)
    {
        pthread_join(threads[i], NULL);
    }

    end = now_sec();

    printf("time: %f\n", end - start);


    printf("SIMD\n");

    start = now_sec();

    SIMD_uppercase((unsigned char *)buffer_simd, SIZE);

    end = now_sec();

    printf("time: %f\n", end - start);


    printf("SIMD + Multithreading\n");

    start = now_sec();

    for (int i = 0; i < N; i++)
    {
        simd_args *args = malloc(sizeof(simd_args));
	if (args == NULL) {
		fprintf(stderr, "Memory allocation failed!\n");
        	exit(1);
    	}

        args->buf = (unsigned char *)(buffer_simd_mt + i * (SIZE / N));
        args->n = SIZE / N;

        pthread_create(&threads_simd[i],
                       NULL,
                       simd_thread_function,
                       args);
	free(args)
    }

    if (SIZE % N != 0)
    {
        for (uint64_t i = N * (SIZE / N); i < SIZE; i++)
        {
            if (buffer_simd_mt[i] >= 'a' && buffer_simd_mt[i] <= 'z')
            {
                buffer_simd_mt[i] -= 32;
            }
        }
    }

    for (int i = 0; i < N; i++)
    {
        pthread_join(threads_simd[i], NULL);
    }

    end = now_sec();

    printf("time: %f\n", end - start);


    free(buffer_original);
    free(buffer_mt);
    free(buffer_simd);
    free(buffer_simd_mt);

    return 0;
}
