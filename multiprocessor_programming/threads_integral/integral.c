#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <math.h>
#include <time.h>

#define NUM_POINTS 1000000000

sem_t sem;
double our_points_num = 0.; 
double y_limit = 1, x_limit = M_PI, z_limit = M_PI;
 
void* thread_func(void* numb)
{
	int i;
	float x, y, z;
	 
	int local_num = *(int*)numb;
	unsigned int t = time(NULL);
	int* local_sum = (int*)calloc(1, sizeof(int));
	
	for (i = 0; i < local_num; ++i) {
		
		x = ((float)rand_r(&t)/RAND_MAX) * x_limit;
		y = ((float)rand_r(&t)/RAND_MAX) * y_limit;
		z = ((float)rand_r(&t)/RAND_MAX) * z_limit;

		if ((x <= x_limit) && (y <= sinf(x)) && (z <= x*y))
			++local_sum[0]; 
	}
	pthread_exit((void*)local_sum);
}

void integral(int thread_number)
{
	int i, rc;
	struct timespec end, begin;
	double elapsed;

	pthread_t* threads = (pthread_t*)malloc(thread_number*sizeof(pthread_t));
	int points_num_thread = NUM_POINTS/thread_number;

	clock_gettime(CLOCK_REALTIME, &begin);
	for (i = 0; i < thread_number; ++i) {
		rc = pthread_create( threads + i, NULL, thread_func, (void*)&points_num_thread);
		if (rc)
			printf("ERROR. return code from pthread_create is %d\n", rc);
	}
 
	int* num_points;
	for (i = 0; i < thread_number; ++i) {
		
		rc = pthread_join( threads[i], (void**)&num_points);
		if (rc)
			printf("ERROR. return code from pthread_join is %d\n", rc);
		our_points_num += *num_points;
	}
	clock_gettime(CLOCK_REALTIME, &end);
	
	elapsed = end.tv_sec - begin.tv_sec;
	elapsed += (end.tv_nsec - begin.tv_nsec)/1000000000.0;

	double res = our_points_num * x_limit * y_limit * z_limit / NUM_POINTS;
	
	printf("%d\t%lf\t%lf\n", thread_number, elapsed, res);
	
	our_points_num = 0;
	free(threads);
}

int main()
{
	int i; 	

	srand(time(NULL));
	sem_init(&sem, 0, 1);
	printf("threadsNumber\tTime\trResult\n");

	for (i = 8; i < 15; i++) 
		integral(i);

	sem_destroy(&sem); 

	return 0;
}
