#include <stdio.h>

#include "matrix.h"

enum {
    ARGC_ERROR = 5,
    FILE_ERROR,
    READ_ERROR
};

int do_program(int argc, char** argv);
int open_files(int argc, char** argv, FILE** files);
int read_matrices(FILE** files, Matrix* matrices, size_t count);
int fill_matrix(FILE* file, Matrix* matr);
int multiply(Matrix* matrices, size_t count, Matrix* result_matr);
int free_matrices(Matrix* matrices, size_t count);
void close_files(FILE** files, size_t count);

//
// Program receives file names from console. The files contain matrices (rectangular).
// One file contains one matrix. The format of these matrices is:
// 1st line: count of rows;
// 2st line: count of columns;
// next lines: matrix elements. Each file line is a matrix row.
// Matrices must suit multiplication rules.
//

int main(int argc, char** argv) {
    int error_code = 0;

    if ((error_code = do_program(argc, argv))) {
        return error_code;
    }

    return 0;
}

int do_program(int argc, char** argv) {
    int error_code = 0;

    size_t count = (size_t)argc - 1;
    FILE* files[count];
    if ((error_code = open_files(argc, argv, files))) {
        return error_code;
    }

    Matrix matrices[count];
    if ((error_code = read_matrices(files, matrices, count))) {
        return error_code;
    }

    close_files(files, count);

    Matrix result_matr;
    result_matr.buffer = NULL;
    if ((error_code = multiply(matrices, count, &result_matr))) {
        return error_code;
    }
    if ((error_code = print_matrix(&result_matr))) {
        return error_code;
    }

    if ((error_code = free_matrices(matrices, count))) {
        return error_code;
    }

    return 0;
}

int open_files(int argc, char** argv, FILE** files) {
    if (argc < 3) {
        printf("At least two matrices must be read. Try again.");
        return ARGC_ERROR;
    }
    for (size_t i = 1; i < argc; i++) {
        files[i - 1] = fopen(argv[i], "r");
        if (!files[i - 1]) {
            printf("File error. Try again.");
            return FILE_ERROR;
        }
    }

    return 0;
}

int read_matrices(FILE** files, Matrix* matrices, size_t count) {
    for (size_t i = 0; i < count; i++) {
        matrices[i].buffer = NULL;
    }
    size_t rows_count = 0;
    size_t cols_count = 0;
    int error_code = 0;
    for (size_t i = 0; i < count; i++) {
        if (fscanf(files[i], "%zu %zu", &rows_count, &cols_count) != 2) {
            printf("Read error while reading matrix parameters. Try again.");
            return READ_ERROR;
        }
        if ((error_code = create_matrix(&matrices[i], rows_count, cols_count))) {
            return error_code;
        }
        if ((error_code = fill_matrix(files[i], &matrices[i]))) {
            return error_code;
        }
    }

    return 0;
}


int fill_matrix(FILE* file, Matrix* matr) {
    double elem = 0.0;
    for (size_t i = 0; i < matr->rows_number; i++) {
        for (size_t j = 0; j < matr->cols_number; j++) {
            if (fscanf(file, "%lf", &elem) != 1) {
                printf("Read error while filling the matrix. Try again.");
                return READ_ERROR;
            }
            if (elem) {
                set_elem(matr, i, j, elem);
            }
        }
    }

    return 0;
}

int multiply(Matrix* matrices, size_t count, Matrix* result_matr) {
    int multiply_result = 0;
    Matrix* prev = &matrices[0];

    for (size_t i = 1; i < count; i++) {
        if ((multiply_result = multiply_matrices(prev, &matrices[i], result_matr))) {
            return multiply_result;
        }
        *prev = *result_matr;
    }

    return 0;
}






int free_matrices(Matrix* matrices, size_t count) {
    int error_code = 0;
    for (size_t i = 0; i < count; i++) {
        if ((error_code = free_matrix(&matrices[i]))) {
            return error_code;
        }
    }

    return 0;
}

void close_files(FILE** files, size_t count) {
    for (size_t i = 0; i < count; i++) {
        fclose(files[i]);
    }
}