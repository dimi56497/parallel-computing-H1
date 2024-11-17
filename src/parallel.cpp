#include <iostream>

#ifndef N_SIZE
#define N_SIZE 16
#endif //N_SIZE
void matTranspose(float** M, float** T);
bool checkSym(float** M);

int main() {
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

    checkSym(M);
    matTranspose(M,T);

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