#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "matrix.h"

#define INITIAL_BUF_SIZE 8

int _realloc_matrix(Matrix* matr);
int _cmp_matrix_item(const void* a, const void* b);
void _sort_matrix(Matrix* matr);
double _calc_multiply_element(Matrix* left, Matrix* right, size_t left_row, size_t right_col);

int create_matrix(Matrix* matr, size_t rows, size_t cols) {
    if (!matr || rows <= 0 || cols <= 0) {
        printf("Wrong argument (-s). Cannot create matrix. Try again");
        return MATRIX_WRONG_ARGUMENT;
    }

    matr->rows_number = rows;
    matr->cols_number = cols;
    matr->items_count = 0;
    matr->buf_size = INITIAL_BUF_SIZE;
    matr->buffer = malloc(matr->buf_size * sizeof(MatrixItem));
    if (!matr->buffer) {
        printf("Memory allocation error. Cannot create matrix. Try again.");
        return MATRIX_MALLOC_ERROR;
    }

    return MATRIX_SUCCESS;
}

double get_elem(Matrix* matr, size_t row, size_t col) {
    if (!matr || row >= matr->rows_number || col >= matr->cols_number) {
        printf("Wrong argument (-s). Cannot get element. Try again.");
        return 0.0;
    }

    if (!matr->buffer) {
        printf("Matrix buffer error. Cannot get element. Try again.");
        return 0.0;
    }

    MatrixItem key = {row:row, col:col};
    MatrixItem* found = bsearch((void*) &key, matr->buffer, matr->items_count,
                                sizeof(MatrixItem), _cmp_matrix_item);

    return found ? found->data : 0.0;
}

int set_elem(Matrix* matr, size_t row, size_t col, double data) {
    if (data == 0.0) {
        return MATRIX_SUCCESS;
    }
    if (!matr || row >= matr->rows_number || col >= matr->cols_number) {
        printf("Wrong argument (-s). Cannot set element. Try again.");
        return MATRIX_WRONG_ARGUMENT;
    }
    if (!matr->buffer) {
        printf("Matrix buffer error. Cannot set element. Try again.");
        return MATRIX_BUFFER_ERROR;
    }

    MatrixItem key = {row:row, col:col};
    MatrixItem* found = bsearch((void*) &key, matr->buffer, matr->items_count,
                                sizeof(MatrixItem), _cmp_matrix_item);
    if (found) {
        found->data = data;
        return MATRIX_SUCCESS;
    }

    if (matr->items_count == matr->buf_size) {
        int realloc_result = _realloc_matrix(matr);
        if (realloc_result != MATRIX_SUCCESS)
            return realloc_result;
    }

    matr->buffer[matr->items_count].data = data;
    matr->buffer[matr->items_count].row = row;
    matr->buffer[matr->items_count].col = col;
    matr->items_count++;

    _sort_matrix(matr);

    return MATRIX_SUCCESS;
}

int print_matrix(Matrix* matr) {
    if (!(matr && matr->buffer)) {
        printf("Wrong argument. Cannot print matrix. Try again.");
        return MATRIX_WRONG_ARGUMENT;
    }

    double result = 0.0;
    for (size_t i = 0; i < matr->rows_number; i++) {
        for (size_t j = 0; j < matr->cols_number; j++) {
            result = get_elem(matr, i, j);
            printf("%lf ", result);
        }
        printf("\n");
    }

    return MATRIX_SUCCESS;
}

int free_matrix(Matrix* matr) {
    if (!matr) {
        printf("Wrong argument. Cannot free matrix. Try again.");
        return MATRIX_WRONG_ARGUMENT;
    }

    free(matr->buffer);
    matr->buffer = NULL;

    return MATRIX_SUCCESS;
}

int multiply_matrices(Matrix* left, Matrix* right, Matrix* result) {
    if (!left || !left->buffer || !right || !right->buffer || !result) {
        printf("Wrong argument (-s). Cannot multiply matrices. Try again.");
        return MATRIX_WRONG_ARGUMENT;
    }

    if (left->cols_number != right->rows_number) {
        printf("Wrong argument (-s). Cannot multiply matrices. Try again.");
        return MATRIX_WRONG_ARGUMENT;
    }

    int create_result = create_matrix(result, left->rows_number, right->cols_number);
    if (create_result != MATRIX_SUCCESS) {
        return create_result;
    }

    int set_result = 0;
    for (size_t i = 0; i < left->rows_number; i++) {
        for (size_t j = 0; j < right->cols_number; j++) {
            set_result = set_elem(result, i, j, _calc_multiply_element(left, right, i, j));

            if (set_result != MATRIX_SUCCESS) {
                free_matrix(result);
                return set_result;
            }
        }
    }

    return MATRIX_SUCCESS;
}

// ------------------------------------------------------------------------
// private
// ------------------------------------------------------------------------

int _realloc_matrix(Matrix* matr) {
    MatrixItem* temp_buffer = realloc(matr->buffer, matr->buf_size * 2 * sizeof(MatrixItem));
    if (!temp_buffer) {
        printf("Memory reallocation error. Try again.");
        free(matr->buffer);
        matr->buffer = NULL;
        return MATRIX_REALLOC_ERROR;
    }

    matr->buffer = temp_buffer;
    matr->buf_size *= 2;

    return MATRIX_SUCCESS;
}

int _cmp_matrix_item(const void* a, const void* b) {
    const MatrixItem* mi_a = (const MatrixItem*) a;
    const MatrixItem* mi_b = (const MatrixItem*) b;

    if (mi_a->row == mi_b->row && mi_a->col == mi_b->col) {
        return 0;
    }

    if (mi_a->row < mi_b->row || (mi_a->row == mi_b->row && mi_a->col < mi_b->col)) {
        return -1;
    }

    return 1;
}

void _sort_matrix(Matrix* matr) {
    qsort(matr->buffer, matr->items_count, sizeof(MatrixItem), _cmp_matrix_item);
}

double _calc_multiply_element(Matrix* left, Matrix* right, size_t left_row, size_t right_col) {
    double result = 0.0;
    double left_elem = 0.0;
    double right_elem = 0.0;
    for (size_t i = 0; i < left->cols_number; i++) {
        left_elem = get_elem(left, left_row, i);
        right_elem = get_elem(right, i, right_col);
        result += left_elem * right_elem;
    }

    return result;
}
