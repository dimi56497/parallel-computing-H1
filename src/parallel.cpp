#include <iostream>
#include <fstream>

#ifndef N_SIZE
#define N_SIZE 16
#endif //N_SIZE

#define FILE_NAME "serial.csv"

void matTranspose(float** M, float** T);
bool checkSym(float** M);
bool checkMat(float **M, float **T);

int main() {
    struct timespec start_ts, end_ts;

    std::fstream file;
    file.open(FILE_NAME, std::ios::in);
    if (!file.is_open()) {
        file.open(FILE_NAME, std::ios::out);
        file << "MatSize,Time,Valid\n";
    }
    file.close();

    file.open(FILE_NAME, std::ios::out | std::ios::app);
    
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

    clock_gettime(CLOCK_MONOTONIC, &start_ts);
    matTranspose(M,T);
    clock_gettime(CLOCK_MONOTONIC, &end_ts);

    elapsed_time = (end_ts.tv_sec - start_ts.tv_sec) + (end_ts.tv_nsec - start_ts.tv_nsec) * 1e-9;
    file << N_SIZE << "," << elapsed_time << "," << checkMat(M,T) << "\n";

    file.close();

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

    for(size_t i = 0; i < 0; i++) {
        for(size_t j = 0; j < 0; j++) {
            if(M[i][j] != T[j][i]) {
                same = false;
                break;
            }
        }
    }
    return same;
}