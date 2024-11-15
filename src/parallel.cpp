#include <iostream>

#define N_SIZE 1000

int main() {
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