#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define S 4
#define T 5

double temperatures[S];
pthread_barrier_t barrier;

double compute_average() {
    double sum = 0.0;
    for (int i = 0; i < S; i++) {
        sum += temperatures[i];
    }
    return sum / S;
}

void* sensor(void* arg) {
    int id = *(int*)arg;

    for (int t = 0; t < T; t++) {

        usleep((rand() % 500) * 1000);

        double temp = 10.0 + (rand() % 100) / 10.0;
        temperatures[id] = temp;

        printf("Timestep %d,  Sensor %d collected: %.2f\n", t, id, temp);

        int status = pthread_barrier_wait(&barrier);

        if (status == PTHREAD_BARRIER_SERIAL_THREAD) {
            double avg = compute_average();
            printf("Timestep %d | >>> Average temperature: %.2f\n\n", t, avg);
        }

        pthread_barrier_wait(&barrier);
    }

    return NULL;
}

int main() {
    pthread_t threads[S];
    int ids[S];

    srand(time(NULL));

    pthread_barrier_init(&barrier, NULL, S);

    for (int i = 0; i < S; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, sensor, &ids[i]);
    }

    for (int i = 0; i < S; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&barrier);

    return 0;
}
