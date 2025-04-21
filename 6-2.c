#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_SIZE 100
#define MAX_THREADS 4

typedef struct {
    int* array;
    int start;
    int end;
    int target;
    int* local_results;
    int* local_count;
    pthread_barrier_t* barrier;
} ThreadData;

void* search_thread(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    
    // Поиск элементов в своей части массива
    for (int i = data->start; i < data->end; i++) {
        if (data->array[i] == data->target) {
            data->local_results[(*data->local_count)++] = i;
        }
    }

    // Ожидание у барьера
    pthread_barrier_wait(data->barrier);
    return NULL;
}

void bubble_sort(int* arr, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int main() {
    int a[] = {1, 2, 3, 3, 2, 2, 4, 5, 6, 6, 6, 66, 11, 22};
    int size = sizeof(a) / sizeof(a[0]);
    int target;
    int k=-1;
    int num_threads;
    printf("Введите число потоков(меньше 4)\t");
    scanf("%d", &num_threads);
    if(num_threads>MAX_THREADS)
    {
        printf("ERROR\n");
        exit;
    }
    else{
        printf("Массив: ");
        for (int i = 0; i < size; i++) printf("%d ", a[i]);
        printf("\nВведите элемент для поиска: ");
        scanf("%d", &target);

        // Инициализация барьера
        pthread_barrier_t barrier;
        pthread_barrier_init(&barrier, NULL, num_threads + 1); // +1 для основного потока

        // Выделение памяти для результатов каждого потока
        int all_results[MAX_THREADS][MAX_SIZE];
        int counts[MAX_THREADS] = {0};
        int final_results[MAX_SIZE];
        int final_count = 0;

        pthread_t threads[MAX_THREADS];
        ThreadData thread_data[MAX_THREADS];

        // Создание потоков
        int chunk_size = size / num_threads;
        for (int i = 0; i < num_threads; i++) {
            thread_data[i].array = a;
            thread_data[i].start = i * chunk_size;
            thread_data[i].end = (i == num_threads - 1) ? size : (i + 1) * chunk_size;
            thread_data[i].target = target;
            thread_data[i].local_results = all_results[i];
            thread_data[i].local_count = &counts[i];
            thread_data[i].barrier = &barrier;

            pthread_create(&threads[i], NULL, search_thread, &thread_data[i]);
        }

        // Ожидание завершения поиска
        pthread_barrier_wait(&barrier);

        // Сбор результатов из всех потоков
        for (int i = 0; i < num_threads; i++) {
            for (int j = 0; j < counts[i]; j++) {
                final_results[final_count++] = all_results[i][j];
            }
        }

        // Сортировка результатов
        bubble_sort(final_results, final_count);

        // Вывод результатов
        if (final_count == 0) {
            printf("Элемент не найден %d\n", k);
        } else {
            printf("Найдено %d вхождений:\n", final_count);
            for (int i = 0; i < final_count; i++) {
                printf("Индекс: %d\n", final_results[i]);
            }
        }

        // Очистка ресурсов
        for (int i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
        }
        pthread_barrier_destroy(&barrier);

        return 0;
    }
}
