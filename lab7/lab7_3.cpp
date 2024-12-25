#include <iostream>
#include <random>
#include <cblas.h>

#define N 2048
#define M 10

void multiplyMatrix(float* matrix1, float* matrix2, float* result) {
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, N, N, N, 1.0, matrix1, N, matrix2, N, 0.0, result, N); // Оптимизированное умножение матриц.
}

void addMatrix(float* matrix1, float* matrix2) {
    cblas_saxpy(N * N, 1.0, matrix1, 1, matrix2, 1);
}

void multiplyMatrixScalar(float* matrix, float scalar, float* result) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            result[i * N + j] = matrix[j * N + i] * scalar;
}

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
    float scalar = 1.0 / (maxRowSum * maxColSum);
    multiplyMatrixScalar(matrixA, scalar, matrixB);
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
        addMatrix(nextR, tempResult);
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
