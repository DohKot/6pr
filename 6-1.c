#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_THREADS 4

typedef struct {
    int* array;
    int start;
    int end;
    int target;
    int* result;
    pthread_barrier_t* barrier;
} ThreadData;

void* search_thread(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    
    for (int i = data->start; i < data->end; i++) {
        if (data->array[i] == data->target) {
            *data->result = i;
            break;
        }
    }

    pthread_barrier_wait(data->barrier);
    return NULL;
}

int main() {
    int array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10};
    int size = sizeof(array) / sizeof(array[0]);
    int target;
    int result = -1;
    int num_threads;

    printf("Введите число потоков (меньше 4) ");
    scanf("%d", &num_threads);

    if (num_threads < 1 || num_threads > MAX_THREADS) {
        printf("Ошибка: число потоков должно быть от 1 до 4.\n");
        return 1;
    }

    printf("Массив: ");
    for (int i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    printf("\nВведите элемент для поиска: ");
    scanf("%d", &target);

    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, num_threads + 1);

    pthread_t threads[MAX_THREADS];
    ThreadData thread_data[MAX_THREADS];

    int chunk_size = size / num_threads;
    for (int i = 0; i < num_threads; i++) {
        thread_data[i].array = array;
        thread_data[i].start = i * chunk_size;
        
        if (i == num_threads - 1) {
            thread_data[i].end = size;
        } else {
            thread_data[i].end = (i + 1) * chunk_size;
        }

        thread_data[i].target = target;
        thread_data[i].result = &result;
        thread_data[i].barrier = &barrier;

        pthread_create(&threads[i], NULL, search_thread, &thread_data[i]);
    }

    pthread_barrier_wait(&barrier);

    if (result != -1) {
        printf("Первое вхождение элемента на позиции: %d\n", result);
    } else {
        printf("Элемент не найден.%d\n", result);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&barrier);
    return 0;
}
