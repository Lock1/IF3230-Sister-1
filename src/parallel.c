#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "serial_lib.c"

// Static variables
int kernel_row, kernel_col, target_row, target_col, num_targets;
int rank;
int world_size;
int container_size;
Matrix kernel;
Matrix *target_container;
FILE *fptr;


void sanity_check() {
    printf("<%d> kernel %d %d | target %d %d | num_targets %d %d | cont %d\n",
        rank,
        kernel_row, kernel_col,
        target_row, target_col,
        num_targets, num_targets/world_size,
        container_size
    );
}


void process_convolution() {
    // #pragma omp parallel num_threads(8)
    // {
        //     int nthreads, tid;
        //     nthreads = omp_get_num_threads();
        //     tid = omp_get_thread_num();
        //     printf("Hello world from processor %s, rank %d out of %d processors, from thread %d out of %d threads\n", processor_name, world_rank, world_size, tid, nthreads);
        // }
    // reads kernel's row and column and initalize kernel matrix from input

    // reads number of target matrices and their dimensions.
    // initialize array of matrices and array of data ranges (int)
    // scanf("%d %d %d", &num_targets, &target_row, &target_col);
    // Matrix* arr_mat = (Matrix*) malloc(num_targets * sizeof(Matrix));
    // int arr_range[num_targets];
    //
    // // read each target matrix, compute their convolution matrices, and compute their data ranges
    // for (int i = 0; i < num_targets; i++) {
    //     arr_mat[i] = input_matrix(target_row, target_col);
    //     arr_mat[i] = convolution(&kernel, &arr_mat[i]);
    //     arr_range[i] = get_matrix_datarange(&arr_mat[i]);
    // }
    //
    // // sort the data range array
    // merge_sort(arr_range, 0, num_targets - 1);
    //
    // int median = get_median(arr_range, num_targets);
    // int floored_mean = get_floored_mean(arr_range, num_targets);
    //
    // // print the min, max, median, and floored mean of data range array
    // printf("%d\n%d\n%d\n%d\n",
    //     arr_range[0],
    //     arr_range[num_targets - 1],
    //     median,
    //     floored_mean);
}

void init_broadcast_routine() {
    MPI_Bcast(&kernel_row, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&kernel_col, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&target_row, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&target_col, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&num_targets, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank != 0)
        init_matrix(&kernel, kernel_row, kernel_col);

    // Broadcasting kernel
    for (int i = 0; i < kernel_row; i++)
        MPI_Bcast(&(kernel.mat[i]), kernel_col, MPI_INT, 0, MPI_COMM_WORLD);
}

void distribute_target_matrix() {
    if (rank == 0) {
        for (int i = 0; i < num_targets; i++) {
            Matrix temp = input_matrix(fptr, target_row, target_col);
            int rank_target = i / (num_targets / world_size);

            if (rank_target == 0)
                target_container[i] = temp;
            else {
                if (rank_target == world_size)
                    rank_target -= 1;

                for (int j = 0; j < target_row; j++)
                    MPI_Send(&(temp.mat[j]), target_col, MPI_INT, rank_target, 0, MPI_COMM_WORLD);
            }
        }
    }
    else {
        for (int i = 0; i < container_size; i++) {
            init_matrix(&(target_container[i]), target_row, target_col);
            for (int j = 0; j < target_row; j++) {
                MPI_Status retcode;
                MPI_Recv(&(target_container[i].mat[j]), target_col, MPI_INT, 0, 0, MPI_COMM_WORLD, &retcode);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(NULL, NULL);

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        // Baca file test case
        fptr = fopen(argv[1], "r");
        if (!fptr) {
            printf("Cannot open file\n");
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }

        fscanf(fptr, "%d %d", &kernel_row, &kernel_col);
        kernel = input_matrix(fptr, kernel_row, kernel_col);
        fscanf(fptr, "%d %d %d", &num_targets, &target_row, &target_col);
    }

    // Broadcasting metadata dan matriks kernel
    init_broadcast_routine();

    // Asumsi : Banyak matriks target >= proses
    if (rank != world_size - 1)
        container_size = num_targets / world_size;
    else
        container_size = num_targets - (world_size - 1) * (num_targets / world_size);
    target_container = (Matrix*) malloc(container_size * sizeof(Matrix));

    distribute_target_matrix();

    // process_convolution();


    MPI_Finalize();
    return 0;
}
