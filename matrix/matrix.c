#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// Global matrix size - set to 20 as required
int MAX;
int** matA;
int** matB;
int** matSumResult;
int** matDiffResult;
int** matProductResult;

// Fill a matrix with random values between 1 and 10
void fillMatrix(int** matrix) {
    for(int i = 0; i < MAX; i++) {
        for(int j = 0; j < MAX; j++) {
            matrix[i][j] = rand() % 10 + 1;
        }
    }
}

// Print a matrix in a formatted grid
void printMatrix(int** matrix) {
    for(int i = 0; i < MAX; i++) {
        for(int j = 0; j < MAX; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Thread argument structure to pass data to each thread
typedef struct {
    int thread_id;          // ID of the thread (0-9)
    int rows_per_thread;    // Number of rows assigned to this thread
} thread_args_t;

// Fetches the appropriate coordinates from the argument, and sets
// the cell of matSumResult at the coordinates to the sum of the
// values at the coordinates of matA and matB.
void* computeSum(void* args) {
    thread_args_t* arg = (thread_args_t*)args;
    int start_row = arg->thread_id * arg->rows_per_thread;
    int end_row = start_row + arg->rows_per_thread;
    
    // Each thread processes its assigned rows
    for(int i = start_row; i < end_row && i < MAX; i++) {
        for(int j = 0; j < MAX; j++) {
            matSumResult[i][j] = matA[i][j] + matB[i][j];
        }
    }
    return NULL;
}

// Fetches the appropriate coordinates from the argument, and sets
// the cell of matDiffResult at the coordinates to the difference of the
// values at the coordinates of matA and matB.
void* computeDiff(void* args) {
    thread_args_t* arg = (thread_args_t*)args;
    int start_row = arg->thread_id * arg->rows_per_thread;
    int end_row = start_row + arg->rows_per_thread;
    
    // Each thread processes its assigned rows
    for(int i = start_row; i < end_row && i < MAX; i++) {
        for(int j = 0; j < MAX; j++) {
            matDiffResult[i][j] = matA[i][j] - matB[i][j];
        }
    }
    return NULL;
}

// Fetches the appropriate coordinates from the argument, and sets
// the cell of matProductResult at the coordinates to the inner product
// of matA and matB.
void* computeProduct(void* args) {
    thread_args_t* arg = (thread_args_t*)args;
    int start_row = arg->thread_id * arg->rows_per_thread;
    int end_row = start_row + arg->rows_per_thread;
    
    // Each thread processes its assigned rows using matrix multiplication
    for(int i = start_row; i < end_row && i < MAX; i++) {
        for(int j = 0; j < MAX; j++) {
            matProductResult[i][j] = 0;
            // Compute dot product of row i of matA and column j of matB
            for(int k = 0; k < MAX; k++) {
                matProductResult[i][j] += matA[i][k] * matB[k][j];
            }
        }
    }
    return NULL;
}

// Spawn a thread to fill each cell in each result matrix.
// Uses 10 threads for each matrix operation (sum, difference, product).
int main() {
    srand(time(0));  // Do Not Remove. Just ignore and continue below.
    
    // 0. Get the matrix size from the command line and assign it to MAX
    MAX = 20;  // Set matrix size to 20 as required
    
    // 1. Fill the matrices (matA and matB) with random values.
    // Allocate memory for matrices
    matA = (int**)malloc(MAX * sizeof(int*));
    matB = (int**)malloc(MAX * sizeof(int*));
    matSumResult = (int**)malloc(MAX * sizeof(int*));
    matDiffResult = (int**)malloc(MAX * sizeof(int*));
    matProductResult = (int**)malloc(MAX * sizeof(int*));
    
    // Allocate memory for each row
    for(int i = 0; i < MAX; i++) {
        matA[i] = (int*)malloc(MAX * sizeof(int));
        matB[i] = (int*)malloc(MAX * sizeof(int));
        matSumResult[i] = (int*)malloc(MAX * sizeof(int));
        matDiffResult[i] = (int*)malloc(MAX * sizeof(int));
        matProductResult[i] = (int*)malloc(MAX * sizeof(int));
    }
    
    fillMatrix(matA);
    fillMatrix(matB);
    
    // 2. Print the initial matrices.
    printf("Matrix A:\n");
    printMatrix(matA);
    printf("Matrix B:\n");
    printMatrix(matB);
    
    // 3. Create pthread_t objects for our threads.
    int num_threads = 10;  // Total of 10 threads as required
    pthread_t threads[num_threads];
    int rows_per_thread = (MAX + num_threads - 1) / num_threads; // Ceiling division: 20/10 = 2 rows per thread
    
    // 4. Create a thread for each cell of each matrix operation.
    // Allocate thread arguments structure
    thread_args_t* thread_args = (thread_args_t*)malloc(num_threads * sizeof(thread_args_t));
    for(int i = 0; i < num_threads; i++) {
        thread_args[i].thread_id = i;
        thread_args[i].rows_per_thread = rows_per_thread;
    }
    
    // Create and execute threads for sum operation
    for(int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, computeSum, &thread_args[i]);
    }
    // 5. Wait for all threads to finish.
    for(int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Create and execute threads for difference operation
    for(int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, computeDiff, &thread_args[i]);
    }
    // Wait for all threads to finish
    for(int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Create and execute threads for product operation
    for(int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, computeProduct, &thread_args[i]);
    }
    // Wait for all threads to finish
    for(int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // 6. Print the results.
    printf("Results:\n");
    printf("Sum:\n");
    printMatrix(matSumResult);
    printf("Difference:\n");
    printMatrix(matDiffResult);
    printf("Product:\n");
    printMatrix(matProductResult);
    
    // Free all dynamically allocated memory
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
