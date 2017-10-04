#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>

enum {
    MATRIX_SUCCESS = 0,
    MATRIX_MALLOC_ERROR,
    MATRIX_REALLOC_ERROR,
    MATRIX_WRONG_ARGUMENT,
    MATRIX_BUFFER_ERROR
};

typedef struct MatrixItem {
    double data;
    size_t row;
    size_t col;
} MatrixItem;

typedef struct Matrix {
    size_t rows_number;
    size_t cols_number;
    size_t items_count;
    size_t buf_size;
    MatrixItem* buffer;
} Matrix;

int create_matrix(Matrix* matr, size_t rows, size_t cols);
// const keyword usability
double get_elem(Matrix* matr, size_t row, size_t col);
int set_elem(Matrix* matr, size_t row, size_t col, double data);
int print_matrix(Matrix* matr);
int free_matrix(Matrix* matr);

int multiply_matrices(Matrix* left, Matrix* right, Matrix* result);

#endif
