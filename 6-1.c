#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
int result = -1;                  // Результат поиска
pthread_barrier_t search_barrier;

void* find_number(void* arg) {
    int* data = (int*)arg;         // Преобразуем аргумент
    int target = data[0];           // Первый элемент - что ищем
    int size = data[1];             // Второй элемент - размер массива
    int* array = &data[2];          // Остальное - сам массив
    for (int i = 0; i < size; i++) {
        if (array[i] == target) {
            result = i;       // Запоминаем индекс
            break;                 // Выходим после первого найденного
        }
    }

    // Ждём у барьера основной поток
    pthread_barrier_wait(&search_barrier);
    return NULL;
}


int main() 
{
    int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int size = sizeof(a) / sizeof(a[0]);
    int target;
    
    // Выводим массив для наглядности
    printf("Массив: ");
    for (int i = 0; i < size; i++) 
        printf("%d ", a[i]);
    printf("\n");
    
    // Ввод искомого элемента
    printf("Введите искомый элемент: ");s
    scanf("%d", &target);
    int thread_data[size + 2];
    thread_data[0] = target;
    thread_data[1] = size;
    for (int i = 0; i < size; i++) {
        thread_data[i+2] = a[i];
    }
    // Инициализируем барьер на 2 потока
    pthread_barrier_init(&search_barrier, NULL, 2);

    pthread_t worker;                  // Создаём поток
    pthread_create(&worker, NULL, find_number, thread_data);

    printf("Идёт поиск...\n");

    // Ждём у барьера рабочий поток
    pthread_barrier_wait(&search_barrier);

    // Вывод результата
    if (result != -1) {
        printf("Число найдено на позиции: %d\n", result);
    } else {
        printf("Число не найдено %d\n", result);
    }

    // Очистка
    pthread_join(worker, NULL);
    pthread_barrier_destroy(&search_barrier);
    return 0;
}

