#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int MAX;
int** matA;
int** matB;
int** matSumResult;
int** matDiffResult;
int** matProductResult;

void fillMatrix(int** matrix) {
    for(int i = 0; i < MAX; i++) {
        for(int j = 0; j < MAX; j++) {
            matrix[i][j] = rand() % 10 + 1;
        }
    }
}

void printMatrix(int** matrix) {
    for(int i = 0; i < MAX; i++) {
        for(int j = 0; j < MAX; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Thread argument structure
typedef struct {
    int thread_id;
    int rows_per_thread;
} thread_args_t;

// Compute sum for assigned rows
void* computeSum(void* args) {
    thread_args_t* arg = (thread_args_t*)args;
    int start_row = arg->thread_id * arg->rows_per_thread;
    int end_row = start_row + arg->rows_per_thread;
    
    for(int i = start_row; i < end_row && i < MAX; i++) {
        for(int j = 0; j < MAX; j++) {
            matSumResult[i][j] = matA[i][j] + matB[i][j];
        }
    }
    return NULL;
}

// Compute difference for assigned rows
void* computeDiff(void* args) {
    thread_args_t* arg = (thread_args_t*)args;
    int start_row = arg->thread_id * arg->rows_per_thread;
    int end_row = start_row + arg->rows_per_thread;
    
    for(int i = start_row; i < end_row && i < MAX; i++) {
        for(int j = 0; j < MAX; j++) {
            matDiffResult[i][j] = matA[i][j] - matB[i][j];
        }
    }
    return NULL;
}

// Compute product for assigned rows
void* computeProduct(void* args) {
    thread_args_t* arg = (thread_args_t*)args;
    int start_row = arg->thread_id * arg->rows_per_thread;
    int end_row = start_row + arg->rows_per_thread;
    
    for(int i = start_row; i < end_row && i < MAX; i++) {
        for(int j = 0; j < MAX; j++) {
            matProductResult[i][j] = 0;
            for(int k = 0; k < MAX; k++) {
                matProductResult[i][j] += matA[i][k] * matB[k][j];
            }
        }
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    srand(time(0));
    
    // Get matrix size from command line
    if(argc != 2) {
        printf("Usage: %s <matrix_size>\n", argv[0]);
        return 1;
    }
    MAX = atoi(argv[1]);
    
    // Allocate memory for matrices
    matA = (int**)malloc(MAX * sizeof(int*));
    matB = (int**)malloc(MAX * sizeof(int*));
    matSumResult = (int**)malloc(MAX * sizeof(int*));
    matDiffResult = (int**)malloc(MAX * sizeof(int*));
    matProductResult = (int**)malloc(MAX * sizeof(int*));
    
    for(int i = 0; i < MAX; i++) {
        matA[i] = (int*)malloc(MAX * sizeof(int));
        matB[i] = (int*)malloc(MAX * sizeof(int));
        matSumResult[i] = (int*)malloc(MAX * sizeof(int));
        matDiffResult[i] = (int*)malloc(MAX * sizeof(int));
        matProductResult[i] = (int*)malloc(MAX * sizeof(int));
    }
    
    // Fill matrices with random values
    fillMatrix(matA);
    fillMatrix(matB);
    
    // Print initial matrices
    printf("Matrix A:\n");
    printMatrix(matA);
    printf("Matrix B:\n");
    printMatrix(matB);
    
    // Create 10 threads
    int num_threads = 10;
    pthread_t threads[num_threads];
    int rows_per_thread = (MAX + num_threads - 1) / num_threads; // Ceiling division
    
    // Allocate thread arguments
    thread_args_t* thread_args = (thread_args_t*)malloc(num_threads * sizeof(thread_args_t));
    for(int i = 0; i < num_threads; i++) {
        thread_args[i].thread_id = i;
        thread_args[i].rows_per_thread = rows_per_thread;
    }
    
    // Create threads for sum
    for(int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, computeSum, &thread_args[i]);
    }
    // Wait for sum threads
    for(int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Create threads for difference
    for(int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, computeDiff, &thread_args[i]);
    }
    // Wait for difference threads
    for(int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Create threads for product
    for(int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, computeProduct, &thread_args[i]);
    }
    // Wait for product threads
    for(int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Print results
    printf("Results:\n");
    printf("Sum:\n");
    printMatrix(matSumResult);
    printf("Difference:\n");
    printMatrix(matDiffResult);
    printf("Product:\n");
    printMatrix(matProductResult);
    
    // Free memory
    for(int i = 0; i < MAX; i++) {
        free(matA[i]);
        free(matB[i]);
        free(matSumResult[i]);
        free(matDiffResult[i]);
        free(matProductResult[i]);
    }
    free(matA);
    free(matB);
    free(matSumResult);
    free(matDiffResult);
    free(matProductResult);
    free(thread_args);
    
    return 0;
}
