#include <iostream>
#include <fstream>
#include <cmath>

#ifdef _OPENMP
#include <omp.h>
#endif //_OPENMP

#ifndef N_SIZE
#define N_SIZE 16
#endif //N_SIZE

#ifndef FILE_NAME
#define FILE_NAME "serial"
#endif //FILE_NAME

#define TRANSPOSE_FILE_NAME "logs/" FILE_NAME "Transpose.csv"
#define SYMM_FILE_NAME "logs/" FILE_NAME "Symm.csv"


void matTranspose(float** M, float** T);
void matTransposeImp(float** M, float** T);
void matTransposeImpBlock(float** M, float** T);
void matTransposeOMP(float** M, float** T);
bool checkSym(float** M);
bool checkSymImp(float** M);
bool checkSymImpBlock(float** M);
bool checkSymOMP(float** M);
bool checkMat(float** M, float** T);
bool checkMatSym(float** M);

int main() {
    struct timespec start_ts, end_ts;
    double start_t, end_t;

    // log file creation
    std::fstream tr_file;
    std::fstream sym_file;
    tr_file.open(std::string(TRANSPOSE_FILE_NAME), std::ios::in);
    sym_file.open(std::string(SYMM_FILE_NAME), std::ios::in);
    if (!tr_file.is_open()) {
        tr_file.open(std::string(TRANSPOSE_FILE_NAME), std::ios::out);
        #ifndef _OPENMP
        tr_file << "MatSize,Time,Valid\n";
        #else
        tr_file << "MatSize,Time,ThreadNum,Valid\n";
        #endif //_OPENMP
    }

    if (!sym_file.is_open()) {
        sym_file.open(std::string(SYMM_FILE_NAME), std::ios::out);
        #ifndef OMP
        sym_file << "MatSize,Time,Valid\n";
        #else
        sym_file << "MatSize,Time,ThreadNum,Valid\n";
        #endif //_OPENMP
    }

    tr_file.close();
    sym_file.close();

    tr_file.open(std::string(TRANSPOSE_FILE_NAME), std::ios::out | std::ios::app);
    sym_file.open(std::string(SYMM_FILE_NAME), std::ios::out | std::ios::app);
    
    //create matrices
    float **M = new(std::nothrow) float*[N_SIZE];
    float **T = new(std::nothrow) float*[N_SIZE];

    if (M == nullptr || T == nullptr) {
        std::cerr << "Memory allocation failed!" << std::endl;
        return 1;
    }

    for(int i = 0; i < N_SIZE; i++) {
        M[i] = new(std::nothrow) float[N_SIZE];
        T[i] = new(std::nothrow) float[N_SIZE];

        if (M[i] == nullptr || T[i] == nullptr) {
            std::cerr << "Memory allocation failed!" << std::endl;
            return 1;
        }
    }

    // init matrix
    srand(time(NULL));
    for(size_t i = 0; i < N_SIZE; i++) {
        for(size_t j = 0; j < N_SIZE; j++) {
            M[i][j] = (rand() % 1000) / 10.0f;
        }
    }

    // time measurement
    #ifdef SERIAL
    start_t = omp_get_wtime();
    bool symmetric = checkSym(M);
    end_t = omp_get_wtime();

    double elapsed_time = (end_t - start_t);
    sym_file << N_SIZE << "," << elapsed_time << "," << omp_get_max_threads() << "," << (symmetric == checkMatSym(M)) << "\n";

    start_t = omp_get_wtime();
    matTranspose(M,T);
    end_t = omp_get_wtime();

    elapsed_time = (end_t - start_t);
    tr_file << N_SIZE << "," << elapsed_time << "," << omp_get_max_threads() << "," << checkMat(M,T) << "\n";
    #endif //SERIAL

    #ifdef IMPLICIT
    clock_gettime(CLOCK_MONOTONIC, &start_ts);
    #ifndef BLOCK
    bool symmetric = checkSymImp(M);
    #else
    bool symmetric = checkSymImpBlock(M);
    #endif //BLOCK
    clock_gettime(CLOCK_MONOTONIC, &end_ts);
    double elapsed_time = (end_ts.tv_sec - start_ts.tv_sec) + (end_ts.tv_nsec - start_ts.tv_nsec) * 1e-9;
    sym_file << N_SIZE << "," << elapsed_time << "," << (symmetric == checkMatSym(M)) << "\n";

    clock_gettime(CLOCK_MONOTONIC, &start_ts);
    #ifndef BLOCK
    matTransposeImp(M,T);
    #else
    matTransposeImpBlock(M,T);
    #endif //BLOCK
    clock_gettime(CLOCK_MONOTONIC, &end_ts);

    elapsed_time = (end_ts.tv_sec - start_ts.tv_sec) + (end_ts.tv_nsec - start_ts.tv_nsec) * 1e-9;
    tr_file << N_SIZE << "," << elapsed_time << "," << checkMat(M,T) << "\n";
    #endif //IMPLICIT

    #ifdef OMP
    start_t = omp_get_wtime();
    bool symmetric = checkSymOMP(M);
    end_t = omp_get_wtime();

    double elapsed_time = (end_t - start_t);
    sym_file << N_SIZE << "," << elapsed_time << "," << omp_get_max_threads() << "," << (symmetric == checkMatSym(M)) << "\n";

    start_t = omp_get_wtime();
    matTransposeOMP(M,T);
    end_t = omp_get_wtime();

    elapsed_time = (end_t - start_t);
    tr_file << N_SIZE << "," << elapsed_time << "," << omp_get_max_threads() << "," << checkMat(M,T) << "\n";
    #endif //OMP

    tr_file.close();
    sym_file.close();

    //delete matrices
    for(int i = 0; i < N_SIZE; i++) {
        delete[] M[i];
        delete[] T[i];
    }

    delete[] M;
    delete[] T;

    return 0;
}

void matTranspose(float** M, float** T) {
    for(size_t i = 0; i < N_SIZE; i++) {
        for (size_t j = 0; j < N_SIZE; j++) {
            T[j][i] = M[i][j];
        }
    }
}

void matTransposeImp(float** M, float** T) {
    for(size_t i = 0; i < N_SIZE; i++) {
        for (size_t j = 0; j < N_SIZE; j++) {
            T[j][i] = M[i][j];
        }
    }
}

void matTransposeImpBlock(float** M, float** T) {
    for(size_t i = 0; i < N_SIZE; i += 2) {
        for (size_t j = 0; j < N_SIZE; j += 8) {
            T[j][i] = M[i][j];
            T[j + 1][i] = M[i][j + 1];
            T[j + 2][i] = M[i][j + 2];
            T[j + 3][i] = M[i][j + 3];
            T[j + 4][i] = M[i][j + 4];
            T[j + 5][i] = M[i][j + 5];
            T[j + 6][i] = M[i][j + 6];
            T[j + 7][i] = M[i][j + 7];

            T[j][i + 1] = M[i + 1][j];
            T[j + 1][i + 1] = M[i + 1][j + 1];
            T[j + 2][i + 1] = M[i + 1][j + 2];
            T[j + 3][i + 1] = M[i + 1][j + 3];
            T[j + 4][i + 1] = M[i + 1][j + 4];
            T[j + 5][i + 1] = M[i + 1][j + 5];
            T[j + 6][i + 1] = M[i + 1][j + 6];
            T[j + 7][i + 1] = M[i + 1][j + 7];
        }
    }
}

void matTransposeOMP(float** M, float** T) {
    #pragma omp parallel for schedule(static) collapse(2)
    for(size_t i = 0; i < N_SIZE; i++) {
        for (size_t j = 0; j < N_SIZE; j++) {
            T[j][i] = M[i][j];
        }
    }
}

bool checkSym(float** M) {
    bool symmetric = true;
    for (size_t i = 0; i < N_SIZE; i++) {
        for (size_t j = 0; j < N_SIZE; j++) {
            if(M[i][j] != M[j][i]) {
                symmetric = false;
            }
        }
    }
    return symmetric;
}

bool checkSymImp(float** M) {
    bool symmetric = true;
    for (size_t i = 0; i < N_SIZE; i++) {
        for (size_t j = 0; j < N_SIZE; j++) {
            if(M[i][j] != M[j][i]) {
                symmetric = false;
            }
        }
    }
    return symmetric;
}

bool checkSymImpBlock(float** M) {
    float sum = 0.0;
    for (size_t i = 0; i < N_SIZE; i += 2) {
        for (size_t j = 0; j < N_SIZE; j += 8) {
            sum += fabs(M[i][j] - M[j][i]);
            sum += fabs(M[i][j + 1] - M[j + 1][i]);
            sum += fabs(M[i][j + 2] - M[j + 2][i]);
            sum += fabs(M[i][j + 3] - M[j + 3][i]);
            sum += fabs(M[i][j + 4] - M[j + 4][i]);
            sum += fabs(M[i][j + 5] - M[j + 5][i]);
            sum += fabs(M[i][j + 6] - M[j + 6][i]);
            sum += fabs(M[i][j + 7] - M[j + 7][i]);

            sum += fabs(M[i + 1][j] - M[j][i + 1]);
            sum += fabs(M[i + 1][j + 1] - M[j + 1][i + 1]);
            sum += fabs(M[i + 1][j + 2] - M[j + 2][i + 1]);
            sum += fabs(M[i + 1][j + 3] - M[j + 3][i + 1]);
            sum += fabs(M[i + 1][j + 4] - M[j + 4][i + 1]);
            sum += fabs(M[i + 1][j + 5] - M[j + 5][i + 1]);
            sum += fabs(M[i + 1][j + 6] - M[j + 6][i + 1]);
            sum += fabs(M[i + 1][j + 7] - M[j + 7][i + 1]);
        }
    }
    return sum == 0.0;
}

bool checkSymOMP(float** M) {
    float sum = 0.0;
    #pragma omp parallel for schedule(static) collapse(2) reduction(+:sum)
    for (size_t i = 0; i < N_SIZE; i++) {
        for (size_t j = 0; j < N_SIZE; j++) {
            sum += fabs(M[i][j] - M[j][i]);
        }
    }

    return sum == 0.0;
}

// function created to check if the matrix transposition has been done correctly
bool checkMat(float** M, float** T) {
    bool same = true;

    for(size_t i = 0; i < N_SIZE; i++) {
        for(size_t j = 0; j < N_SIZE; j++) {
            if(M[i][j] != T[j][i]) {
                same = false;
                break;
            }
        }
    }
    return same;
}

bool checkMatSym(float** M) {
    bool symm = true;

    for(size_t i = 0; i < N_SIZE; i++) {
        for(size_t j = 0; j < N_SIZE; j++) {
            if(M[i][j] != M[j][i]) {
                symm = false;
                break;
            }
        }
    }
    return symm;
}