#include <iostream>
#include <random>

#define N 2048
#define M 10

// Функция для умножения двух матриц
void multiplyMatrix(float* matrix1, float* matrix2, float* result) { 
    for (int i = 0; i < N * N; i++)
        result[i] = 0.0;

    for (int k = 0; k < N; k++) // Начало цикла по переменной k, которая представляет собой индекс столбца в матрице 1 и индекс строки в матрице 2.
        for (int i = 0; i < N; i++) // Начало цикла по переменной i, представляющей индекс строки в матрице 1
            for (int j = 0; j < N; j++) // Начало цикла по переменной j, представляющей индекс столбца в матрице 2.
// result является суммой произведений элементов из соответствующей строки матрицы 1 и столбца матрицы 2.
                result[i * N + j] += matrix1[i * N + k] * matrix2[k * N + j];
}  

// Функция для сложения двух матриц
void addMatrix(float* matrix1, float* matrix2, float* result) {
    for (int i = 0; i < N * N; i++)
        result[i] = matrix1[i] + matrix2[i];
}

// Функция для расчета матрицы B на основе матрицы A. Этот расчет включает в себя нахождение максимальной суммы элементов в строках и столбцах матрицы A.
void calculateMatrixB(float* matrixA, float* matrixB) {
    int i, j;

    int maxRowSum = 0;
    int maxColSum = 0;

    for (i = 0; i < N; i++) {
        int rowSum = 0;
        int colSum = 0;
        for (j = 0; j < N; j++) {
            rowSum += matrixA[i * N + j];
            colSum += matrixA[j * N + i];
        }
        if (rowSum > maxRowSum)
            maxRowSum = rowSum;
        if (colSum > maxColSum)
            maxColSum = colSum;
    }
    float maxSum = maxRowSum * maxColSum;
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            matrixB[i * N + j] = (matrixA[j * N + i] / (maxSum));
}

// Функция для расчета матрицы matrixR на основе матрицы matrixI, matrixA и matrixB.
void calculateMatrixR(float* matrixI, float* matrixA, float* matrixB, float* matrixR) {
    float* result = new float [N * N]; // выделяем память под одномерный массив из N×N элементов типа float
    multiplyMatrix(matrixB, matrixA, result);
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            matrixR[i * N + j] = matrixI[i * N + j] - result[i * N + j];
    delete[] result;
}
// Функция, окончательно завершающая вычисления, включая итерационное сложение и умножение матриц.
void finalizeCalculation(float* matrixI, float* matrixR, float* tempResult) {
    float* nextR = new float[N * N];
    float* result = new float [N * N]; // создаем два новых массива для временного хранения результатов.

    for (int i = 0; i < N * N; i++){
        nextR[i] = matrixR[i];
        tempResult[i] = matrixI[i];
    }

    for (int i = 0; i < M; i++) {
        addMatrix(tempResult, nextR, tempResult);
        multiplyMatrix(matrixR, nextR, result);
        // Перемноженный результат становится новым nextR для следующей итерации
        for (int j = 0; j < N * N; j++)
            nextR[j] = result[j];
    }
    delete[] nextR;
    delete[] result;
}

int main() {
    timespec start, end;
    double totalTime = 0.0;
    clock_gettime(CLOCK_MONOTONIC, &start);

    float* matrixA = new float[N * N];
    float* matrixB = new float[N * N];
    float* matrixI = new float[N * N];
    float* matrixR = new float[N * N];
    float* tempResult = new float [N * N];
    float* finalResult = new float[N * N];

// Инициализация генератора случайных чисел с равномерным распределением в интервале [0, 5].
    std::random_device rd; // Создаем объект rd который используется для генерации случайных чисел из некоторого источника.
    std::mt19937 gen(rd()); // Создаем генератор случайных чисел типа std::mt19937 инициализируем его значением из объекта rd. Этот генератор будет использоваться для создания случайных чисел.
    std::uniform_int_distribution<> dis(0, 5); // Создаем объект распределения dis, который генерирует случайные целые числа в диапазоне от 0 до 5 

// Инициализируем наши матрицы и генерируем каждый элемент
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrixR[i * N + j] = 0;
            tempResult[i * N + j] = 0;
            finalResult[i * N + j] = 0;
            matrixA[i * N + j] = dis(gen);
            matrixB[i * N + j] = matrixA[j * N + i]; // Элементы матрицы B устанавливаются равными транспонированным элементам матрицы A.
            if (i == j)
// Элементы матрицы matrixI устанавливаются в 1 на главной диагонали и в 0 в остальных случаях, создавая единичную матрицу.
                matrixI[i * N + j] = 1;
            else
                matrixI[i * N + j] = 0;
        }
    }

    calculateMatrixB(matrixA, matrixB);
    calculateMatrixR(matrixI, matrixA, matrixB, matrixR);
    finalizeCalculation(matrixI, matrixR, tempResult);
    multiplyMatrix(tempResult, matrixB, finalResult);

    delete[] matrixA;
    delete[] matrixB;
    delete[] matrixI;
    delete[] matrixR;
    delete[] tempResult;
    delete[] finalResult;

    clock_gettime(CLOCK_MONOTONIC, &end);
    totalTime = (end.tv_sec - start.tv_sec) + ((end.tv_nsec - start.tv_nsec) * 0.000000001);

    std::cout << "Total Time: " << totalTime << " sec" << std::endl;

    return 0;
}
