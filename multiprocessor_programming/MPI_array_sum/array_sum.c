#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 1000

int main(int argc, char* argv[])
{
	int i, workers_number, sum;
	int myrank, size;
	int array[N];
	MPI_Status Status;

	sum = 0;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
        MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	if (myrank == 0) {
		for (i = 0; i < N; ++i)
			array[i] = i;

		for (i = 1; i < size; i++) {
			MPI_Send(&array[i*(N/size)], N/size, MPI_INT, i, i, MPI_COMM_WORLD);
		}
		for (i = 0; i < N/size; i++) 
			sum += array[i]; 
		for (i =  (N/size)*size; i < N; i++)
			sum += array[i];
			
		printf("my rank is 0, my sum is %d \n", sum);		
		int proc_sum;
		for ( i = 1; i < size; i++) {
			MPI_Recv(&proc_sum, 1, MPI_INT, i, i, MPI_COMM_WORLD, &Status);			
			sum += proc_sum;
		}

		printf("\nsum from the processes is  %d \n", sum);
				
		int sum_0 = 0;
		for ( i = 0; i < N; i++)
			sum_0 +=array[i];
		printf("\nsum form the 0 process is %d \n", sum_0);

		(sum == sum_0) ? (printf("\nsum_0 == sum\n")) : (printf("\nsum_0 != sum :(((\n"));	  

	} else {
		MPI_Recv( array, N/size, MPI_INT, 0, myrank, MPI_COMM_WORLD, &Status);

		for (i = 0; i < N/size; ++i) 
			sum += array[i];

		MPI_Send(&sum, 1, MPI_INT, 0, myrank, MPI_COMM_WORLD);

		printf("my rank is %d, my sum is %d\n", myrank, sum);
	}

	MPI_Finalize();

	return 0;
}
