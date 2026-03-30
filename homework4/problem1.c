#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <immintrin.h>

#define N 10
#define SIZE 1000000000

typedef struct {
	uint64_t A;
	uint64_t C;
	uint64_t G;
	uint64_t T;
} Counts;

Counts counts_global;
pthread_mutex_t mutex;

double now_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

char* create_buffer() {
	char* sequence = (char*) malloc(SIZE * sizeof(char));
	if(sequence == NULL) {
		perror("memory allocation failed"); 
		exit(1);
	}
	char* bases = "ACGT";
	for(uint64_t i = 0; i < SIZE; i++) {
		int n = rand() % 4;
		sequence[i] = bases[n];
	}
	return sequence;
}


void* nucleotide_counter(void* ptr) {
	char* start = (char*) ptr;
	Counts counts_local = {0};
	for(uint64_t i = 0; i < SIZE / N; i++) {
		counts_local.A += (start[i] == (unsigned char)'A');
		counts_local.C += (start[i] == (unsigned char)'C');
		counts_local.G += (start[i] == (unsigned char)'G');
		counts_local.T += (start[i] == (unsigned char)'T');
	}
	pthread_mutex_lock(&mutex);
	counts_global.A += counts_local.A;
	counts_global.C += counts_local.C;
	counts_global.G += counts_local.G;
	counts_global.T += counts_local.T;
	pthread_mutex_unlock(&mutex);
	return NULL;
}

pthread_t threads[N];
pthread_t threads_simd[N];

void sequential_counter(char* start, Counts* counts) {
	for(uint64_t i = 0; i < SIZE; i++) {
		counts->A += (start[i] == (unsigned char)'A');
		counts->C += (start[i] == (unsigned char)'C');
		counts->G += (start[i] == (unsigned char)'G');
		counts->T += (start[i] == (unsigned char)'T');
	}
}

void SIMD_counter(const unsigned char* buf, size_t n, Counts* counts) {
 	const __m256i target_A = _mm256_set1_epi8('A');
 	const __m256i target_C = _mm256_set1_epi8('C');
 	const __m256i target_G = _mm256_set1_epi8('G');
 	const __m256i target_T = _mm256_set1_epi8('T');

    	size_t i = 0;
    	size_t limit = n & ~(size_t)31;

    	for (; i < limit; i += 32) {
        	__m256i v = _mm256_loadu_si256((const __m256i*)(buf + i));
        	__m256i eq_A = _mm256_cmpeq_epi8(v, target_A);
        	__m256i eq_C = _mm256_cmpeq_epi8(v, target_C);
        	__m256i eq_G = _mm256_cmpeq_epi8(v, target_G);
        	__m256i eq_T = _mm256_cmpeq_epi8(v, target_T);

        	unsigned mask_A = (unsigned)_mm256_movemask_epi8(eq_A);
        	unsigned mask_C = (unsigned)_mm256_movemask_epi8(eq_C);
        	unsigned mask_G = (unsigned)_mm256_movemask_epi8(eq_G);
        	unsigned mask_T = (unsigned)_mm256_movemask_epi8(eq_T);
        	counts->A += (uint64_t)__builtin_popcount(mask_A);
        	counts->C += (uint64_t)__builtin_popcount(mask_C);
        	counts->G += (uint64_t)__builtin_popcount(mask_G);
        	counts->T += (uint64_t)__builtin_popcount(mask_T);
	}

	for(; i < n; i++) {
		counts->A += (buf[i] == (unsigned char)'A');
		counts->C += (buf[i] == (unsigned char)'C');
		counts->G += (buf[i] == (unsigned char)'G');
		counts->T += (buf[i] == (unsigned char)'T');
	}
}

typedef struct {
	unsigned char* buf;
	size_t n;
	Counts* counts;
} simd_args;

Counts counts_global_simd;
void* simd_thread_function(void* ptr) {
	simd_args* args = (simd_args*) ptr;
	SIMD_counter(args->buf, args->n, args->counts);
	pthread_mutex_lock(&mutex);
	counts_global_simd.A += (args->counts)->A;
	counts_global_simd.C += args->counts->C;
	counts_global_simd.G += args->counts->G;
	counts_global_simd.T += args->counts->T;
	pthread_mutex_unlock(&mutex);
	return NULL;
}

int main() {
	pthread_mutex_init(&mutex, NULL);
	char* nucleotides = create_buffer();

	double start, end;

	start = now_sec();
	for(int i = 0; i < N; i++) {
		pthread_create(&threads[i], NULL, nucleotide_counter, nucleotides + i * (SIZE / N));
	}
	if(SIZE % N != 0) {
		for(uint64_t i = N * (SIZE / N); i < SIZE; i++) {
			counts_global.A += (nucleotides[i] == (unsigned char)'A');
			counts_global.C += (nucleotides[i] == (unsigned char)'C');
			counts_global.G += (nucleotides[i] == (unsigned char)'G');
			counts_global.T += (nucleotides[i] == (unsigned char)'T');
		}
	}
	printf("Multithreading\n");
	for(int i = 0; i < N; i++) {
		pthread_join(threads[i], NULL);
	}

	end = now_sec();
	
	printf("A: %ld\nC: %ld\nG: %ld\nT: %ld\n", counts_global.A, counts_global.C, counts_global.G, counts_global.T);
	printf("time: %f\n", end - start);

	printf("Sequential\n");
	Counts counts_seq = {0};
	start = now_sec();
	sequential_counter(nucleotides, (void*) &counts_seq);
	end = now_sec();
	printf("A: %ld\nC: %ld\nG: %ld\nT: %ld\n", counts_seq.A, counts_seq.C, counts_seq.G, counts_seq.T);
	printf("time: %f\n", end - start);


	
	printf("SIMD\n");
	Counts counts_simd = {0};
	start = now_sec();
	SIMD_counter(nucleotides, SIZE, &counts_simd);
	end = now_sec();
	printf("A: %ld\nC: %ld\nG: %ld\nT: %ld\n", counts_simd.A, counts_simd.C, counts_simd.G, counts_simd.T);
	printf("time: %f\n", end - start);


	start = now_sec();
	for(int i = 0; i < N; i++) {
		simd_args* args = malloc(sizeof(simd_args));
		if(args == NULL){
			perror("memory allocation failed");
			exit(1);
		}
		args->buf = nucleotides + i * (SIZE / N);
		args->n = SIZE / N;
		args->counts = calloc(1, sizeof(Counts));
		if(args->counts == NULL) {
			perror("memory allocation failed");
			exit(1);
		}
		pthread_create(&threads_simd[i], NULL, simd_thread_function, (void*) args);
	}
	if(SIZE % N != 0) {
		for(uint64_t i = N * (SIZE / N); i < SIZE; i++) {
			counts_global_simd.A += (nucleotides[i] == (unsigned char)'A');
			counts_global_simd.C += (nucleotides[i] == (unsigned char)'C');
			counts_global_simd.G += (nucleotides[i] == (unsigned char)'G');
			counts_global_simd.T += (nucleotides[i] == (unsigned char)'T');
		}
	}
	printf("SIMD + Multithreading\n");
	for(int i = 0; i < N; i++) {
		pthread_join(threads_simd[i], NULL);
	}

	end = now_sec();
	printf("A: %ld\nC: %ld\nG: %ld\nT: %ld\n", counts_global_simd.A, counts_global_simd.C, counts_global_simd.G, counts_global_simd.T);
	printf("time: %f\n", end - start);


	pthread_mutex_destroy(&mutex);
	free(nucleotides);
	return 0;
}
