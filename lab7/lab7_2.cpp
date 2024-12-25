#include <iostream>
#include <random>
#include <emmintrin.h> // Заголовочный файл для использования SSE

#define N 2048
#define M 10

void multiplyMatrix(float* matrix1, float* matrix2, float* result) {
    for (int i = 0; i < N * N; i++)
        result[i] = 0.0;

    for (int k = 0; k < N; k++) {
        for (int i = 0; i < N; i++) {
// _m128 - это структура данных, представляющая четыре значения типа float, которые могут обрабатываться SIMD инструкциями.
//устанавливает все четыре значения в регистре SIMD в одно и то же значение
            __m128 m1 = _mm_set1_ps(matrix1[i * N + k]); // Загружаем элемент из первой матрицы в регистр SIMD
            for (int j = 0; j < N; j += 4) {
                __m128 m2 = _mm_loadu_ps(&matrix2[k * N + j]); // Загружаем 4 элемента из второй матрицы в регистр SIMD

                __m128 mul = _mm_mul_ps(m1, m2); // Умножаем элемент первой матрицы на 4 элемента второй матрицы
                __m128 res = _mm_loadu_ps(&result[i * N + j]); // Загружаем текущее значение из результирующей матрицы
                res = _mm_add_ps(res, mul); // Добавляем результат умножения к текущему значению

                _mm_storeu_ps(&result[i * N + j], res); // Сохраняем результат в результирующую матрицу
            }
        }
    }
}

void addMatrix(float* matrix1, float* matrix2, float* result) {
    for (int i = 0; i < N * N; i += 4) {
        __m128 m1 = _mm_loadu_ps(&matrix1[i]);
        __m128 m2 = _mm_loadu_ps(&matrix2[i]);
        __m128 sum = _mm_add_ps(m1, m2);
        _mm_storeu_ps(&result[i], sum);
    }
}

void multiplyMatrixScalar(float* matrix, float scalar, float* result) {
    __m128 s = _mm_set1_ps(scalar);
    for (int i = 0; i < N * N; i += 4) {
        __m128 m = _mm_loadu_ps(&matrix[i]);
        __m128 mul = _mm_mul_ps(m, s);
        _mm_storeu_ps(&result[i], mul);
    }
}

void calculateMatrixB(float* matrixA, float* matrixB) {
    int maxRowSum = 0;
    int maxColSum = 0;

    for (int i = 0; i < N; i++) {
        int rowSum = 0;
        int colSum = 0;
        for (int j = 0; j < N; j++) {
            rowSum += matrixA[i * N + j];
            colSum += matrixA[j * N + i];
        }
        if (rowSum > maxRowSum)
            maxRowSum = rowSum;
        if (colSum > maxColSum)
            maxColSum = colSum;
    }
    float scalar = (1.0 / (maxRowSum * maxColSum));
    multiplyMatrixScalar(matrixB, scalar, matrixB);
}

void calculateMatrixR(float* matrixI, float* matrixA, float* matrixB, float* matrixR) {
    float* result = new float [N * N];
    multiplyMatrix(matrixB, matrixA, result);
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            matrixR[i * N + j] = matrixI[i * N + j] - result[i * N + j];
    delete[] result;
}

void finalizeCalculation(float* matrixI, float* matrixR, float* tempResult) {
    float* nextR = new float[N * N];
    float* result = new float [N * N];

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
    float* tempResult = new float[N * N];
    float* finalResult = new float[N * N];

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 3);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrixR[i * N + j] = 0;
            tempResult[i * N + j] = 0;
            finalResult[i * N + j] = 0;
            matrixA[i * N + j] = dis(gen);
            matrixB[i * N + j] = matrixA[j * N + i];
            if (i == j)
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
