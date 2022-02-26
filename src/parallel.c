#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "serial_lib.c"

void process_convolution() {
    int kernel_row, kernel_col, target_row, target_col, num_targets;


    // reads kernel's row and column and initalize kernel matrix from input
    scanf("%d %d", &kernel_row, &kernel_col);
    Matrix kernel = input_matrix(kernel_row, kernel_col);

    // reads number of target matrices and their dimensions.
    // initialize array of matrices and array of data ranges (int)
    scanf("%d %d %d", &num_targets, &target_row, &target_col);
    Matrix* arr_mat = (Matrix*) malloc(num_targets * sizeof(Matrix));
    int arr_range[num_targets];

    // read each target matrix, compute their convolution matrices, and compute their data ranges
    for (int i = 0; i < num_targets; i++) {
        arr_mat[i] = input_matrix(target_row, target_col);
        arr_mat[i] = convolution(&kernel, &arr_mat[i]);
        arr_range[i] = get_matrix_datarange(&arr_mat[i]);
    }

    // sort the data range array
    merge_sort(arr_range, 0, num_targets - 1);

    int median = get_median(arr_range, num_targets);
    int floored_mean = get_floored_mean(arr_range, num_targets);

    // print the min, max, median, and floored mean of data range array
    printf("%d\n%d\n%d\n%d\n",
        arr_range[0],
        arr_range[num_targets - 1],
        median,
        floored_mean);
}

int main(int argc, char *argv[]) {
    MPI_Init(NULL, NULL);

    // Baca file test case
    MPI_File fhandle;
    if (MPI_File_open(MPI_COMM_WORLD, argv[1], MPI_MODE_RDONLY, MPI_INFO_NULL, &fhandle) != MPI_SUCCESS) {
        printf("Cannot open file\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }


    // int world_size;
    // MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    //
    // int rank;
    // MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    //
    // char processor_name[MPI_MAX_PROCESSOR_NAME];
    // int name_len;
    // MPI_Get_processor_name(processor_name, &name_len);

    // process_convolution();

    // #pragma omp parallel num_threads(8)
    // {
        //     int nthreads, tid;
        //     nthreads = omp_get_num_threads();
        //     tid = omp_get_thread_num();
        //     printf("Hello world from processor %s, rank %d out of %d processors, from thread %d out of %d threads\n", processor_name, world_rank, world_size, tid, nthreads);
        // }

    MPI_File_close(&fhandle);
    MPI_Finalize();

    return 0;
}
