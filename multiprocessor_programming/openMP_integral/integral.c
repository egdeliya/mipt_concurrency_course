#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

int num = 1000;

double f(double x) {
	return 1./(1 + x*x);
}

void integral_crit(int threads_num) {
	int i = 0;
	int my_id, num_proc, num_threads;

	num_proc = omp_get_num_procs();
	omp_set_num_threads(threads_num);
	
	double res = 0;
	double h = 1./num;

	double begin, end, total;
	begin = omp_get_wtime();

	#pragma omp parallel shared(res) private(my_id)
	{	
		double loc_res = 0;
		my_id = omp_get_thread_num();

		#pragma omp for
		for (i = 0; i < num; ++i)	
			loc_res += (f(i*h) + f((i+1)*h)) * h / 2;
		
		#pragma omp critical
		res += loc_res;
	}
	
	end = omp_get_wtime();
	total = end - begin;

	printf("%d\t\t%lf\t%lf\t%lf\n", threads_num, total, 4*res, (M_PI - 4*res)/M_PI);					
}

void integral_red(int threads_num) {
        
	int i = 0;
        int my_id, num_proc, num_threads;

        num_proc = omp_get_num_procs();
        omp_set_num_threads(threads_num);

        double res = 0;
        double h = 1./num;

        double begin, end, total;
        begin = omp_get_wtime();

        #pragma omp parallel reduction (+:res)
	{
		#pragma omp for 
                for (i = 0; i < num; ++i)
                	res += (f(i*h) + f((i+1)*h)) * h / 2;

	}
	res *= 4;

        end = omp_get_wtime();
        total = end - begin;

        printf("%d\t\t%lf\t%lf\t%lf\n", threads_num, total, res, (M_PI - res)/M_PI);
}

int main(int argc, char** argv) {
	setbuf(stdout, 0);
	int i;

	printf("critical section\n");	
	printf("thr_num\ttime\tresult\terror\n"); 
	for (i = 1; i < 12; ++i) { 
		integral_crit(i);
	}

	printf("\nreduction\n");   
        printf("thr_num\ttime\tresult\terror\n"); 
        for (i = 1; i < 12; ++i) { 
                integral_red(i);
        }   
	
	return 0;
}

