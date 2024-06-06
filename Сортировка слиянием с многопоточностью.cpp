#include <iostream>
#include <vector>
#include <future>
#include <thread>

// Порог, до которого не используется многопоточность
const size_t THRESHOLD = 1000;

// Функция для слияния двух отсортированных подмассивов
void merge(std::vector<int>& arr, size_t left, size_t mid, size_t right) {
    size_t n1 = mid - left + 1;
    size_t n2 = right - mid;

    std::vector<int> L(n1);
    std::vector<int> R(n2);

    for (size_t i = 0; i < n1; ++i)
        L[i] = arr[left + i];
    for (size_t j = 0; j < n2; ++j)
        R[j] = arr[mid + 1 + j];

    size_t i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            ++i;
        }
        else {
            arr[k] = R[j];
            ++j;
        }
        ++k;
    }

    while (i < n1) {
        arr[k] = L[i];
        ++i;
        ++k;
    }

    while (j < n2) {
        arr[k] = R[j];
        ++j;
        ++k;
    }
}

// Функция для многопоточной сортировки слиянием
void mergeSort(std::vector<int>& arr, size_t left, size_t right, size_t depth) {
    if (left < right) {
        size_t mid = left + (right - left) / 2;

        // Условие для распараллеливания
        if (depth < std::thread::hardware_concurrency() && (right - left) > THRESHOLD) {
            // Использование std::async для запуска потоков
            auto left_future = std::async(std::launch::async, mergeSort, std::ref(arr), left, mid, depth + 1);
            auto right_future = std::async(std::launch::async, mergeSort, std::ref(arr), mid + 1, right, depth + 1);

            // Синхронизация результатов работы потоков с использованием std::future
            left_future.get();
            right_future.get();
        }
        else {
            // Обычная рекурсия без распараллеливания
            mergeSort(arr, left, mid, depth + 1);
            mergeSort(arr, mid + 1, right, depth + 1);
        }

        // Слияние отсортированных подмассивов
        merge(arr, left, mid, right);
    }
}

int main() {
    std::vector<int> arr = { 38, 27, 43, 3, 9, 82, 10 };

    // Запуск сортировки
    mergeSort(arr, 0, arr.size() - 1, 0);

    // Вывод отсортированного массива
    for (int num : arr) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    return 0;
}