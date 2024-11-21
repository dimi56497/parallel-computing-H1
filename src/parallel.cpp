#include <iostream>
#include <fstream>

#ifndef N_SIZE
#define N_SIZE 16
#endif //N_SIZE

#ifndef FILE_NAME
#define FILE_NAME "serial"
#endif //FILE_NAME

#define TRANSPOSE_FILE_NAME FILE_NAME "Transpose.csv"
#define SYMM_FILE_NAME FILE_NAME "Symm.csv"


void matTranspose(float** M, float** T);
bool checkSym(float** M);
bool checkMat(float **M, float **T);

int main() {
    struct timespec start_ts, end_ts;

    // log file creation
    std::fstream tr_file;
    std::fstream sym_file;
    tr_file.open(std::string(TRANSPOSE_FILE_NAME), std::ios::in);
    sym_file.open(std::string(SYMM_FILE_NAME), std::ios::in);
    if (!tr_file.is_open()) {
        tr_file.open(std::string(TRANSPOSE_FILE_NAME), std::ios::out);
        tr_file << "MatSize,Time,Valid\n";
    }

    if (!sym_file.is_open()) {
        sym_file.open(std::string(SYMM_FILE_NAME), std::ios::out);
        sym_file << "MatSize,Time,Valid\n";
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
    clock_gettime(CLOCK_MONOTONIC, &start_ts);
    checkSym(M);
    clock_gettime(CLOCK_MONOTONIC, &end_ts);
    double elapsed_time = (end_ts.tv_sec - start_ts.tv_sec) + (end_ts.tv_nsec - start_ts.tv_nsec) * 1e-9;
    sym_file << N_SIZE << "," << elapsed_time << "," << checkMat(M,T) << "\n";

    clock_gettime(CLOCK_MONOTONIC, &start_ts);
    matTranspose(M,T);
    clock_gettime(CLOCK_MONOTONIC, &end_ts);

    elapsed_time = (end_ts.tv_sec - start_ts.tv_sec) + (end_ts.tv_nsec - start_ts.tv_nsec) * 1e-9;
    tr_file << N_SIZE << "," << elapsed_time << "," << checkMat(M,T) << "\n";

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

// function created to check if the matrix transposition has been done correctly
bool checkMat(float **M, float **T) {
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