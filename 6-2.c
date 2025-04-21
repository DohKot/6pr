#include <stdio.h>
#include <pthread.h>

#define MAX_SIZE 100

// Структура для хранения результатов
typedef struct {
    int indices[MAX_SIZE];
    int count;
} SearchResults;

// Глобальные переменные
SearchResults main_results = {0};
SearchResults worker_results = {0};
pthread_barrier_t barrier;

// Функция поиска (используется обоими потоками)
void search_section(int* array, int start, int end, int target, SearchResults* res) {
    for (int i = start; i < end; i++) {
        if (array[i] == target) {
            res->indices[res->count++] = i;
        }
    }
}
    
// Функция рабочего потока
void* worker_func(void* arg) {
    int* params = (int*)arg;
    int* array = &params[2];
    int target = params[0];
    int size = params[1];
    
    // Поиск во второй половине массива
    search_section(array, size/2, size, target, &worker_results);
    
    // Ожидание основного потока
    pthread_barrier_wait(&barrier);
    return NULL;
}

int main() {
    int array[] = {1, 2, 3, 3, 2, 2, 4, 5, 6, 6, 6, 66, 11, 22};
    int size = sizeof(array)/sizeof(array[0]);
    int target;
    
    printf("Массив: ");
    for (int i = 0; i < size; i++) printf("%d ", array[i]);
    printf("\nВведите элемент для поиска: ");
    scanf("%d", &target);
    
    // Инициализация барьера (2 потока)
    pthread_barrier_init(&barrier, NULL, 2);
    
    // Подготовка данных для потока [target, size, элементы...]
    int thread_data[size+2];
    thread_data[0] = target;
    thread_data[1] = size;
    for (int i = 0; i < size; i++) thread_data[i+2] = array[i];
    
    pthread_t worker;
    pthread_create(&worker, NULL, worker_func, thread_data);
    
    // Основной поток обрабатывает первую половину массива
    search_section(array, 0, size/2, target, &main_results);
    
    // Ожидание рабочего потока
    pthread_barrier_wait(&barrier);
    
    // Объединение результатов
    int total_results[MAX_SIZE];
    int total_count = 0;
    
    // Копируем результаты из основного потока
    for (int i = 0; i < main_results.count; i++) {
        total_results[total_count++] = main_results.indices[i];
    }
    
    // Копируем результаты из рабочего потока
    for (int i = 0; i < worker_results.count; i++) {
        total_results[total_count++] = worker_results.indices[i];
    }
    
    // Сортировка
    for (int i = 0; i < total_count-1; i++) {
        for (int j = 0; j < total_count-i-1; j++) {
            if (total_results[j] > total_results[j+1]) {
                int temp = total_results[j];
                total_results[j] = total_results[j+1];
                total_results[j+1] = temp;
            }
        }
    }
    
    // Вывод результатов
    if (total_count == 0) {
        printf("Элемент не найден\n");
    } else {
        printf("Найдено %d вхождений:\n", total_count);
        for (int i = 0; i < total_count; i++) {
            printf("%d ", total_results[i]);
        }
        printf("\n");
    }
    
    // Очистка
    pthread_join(worker, NULL);
    pthread_barrier_destroy(&barrier);
    
    return 0;
}
