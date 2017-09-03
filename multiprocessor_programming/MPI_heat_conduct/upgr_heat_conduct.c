#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int size;

double  Lx = 0.5, Ly = 0.5;
double u_l = 80, u_r = 30, u_0 = 5;
int ro = 8960, c = 380, lambda = 401;
double T = 60, h = 0.01;


int main(int argc, char ** argv)
{
        int i, str, col;

        // начальная строка и конечная строка для каждого потока
        int start, end;

        // коэффициент температуропроводности
        double k = ((double)lambda)/ro/c;
        //printf("k is %lf\n", k);

        // шаг по времени t < h^2/k
        double t = 0.1*h*h/k;
        //printf("time step : %lf \n", t);

        // количество итераций
        int num_iter = T/t;
        //printf("num_iter %d\n", num_iter);

        //вычисление size
        size = Lx / h + 1;
        //printf("size is %d\n", size);

        //вычисление постоянного множителя
        double mult = k*t/h/h;
        //printf(" mult %lf\n", mult);

        //измерение времени
        double begin, end_, total;

        //..........................................//
        int my_rank, proc_number;
        MPI_Status status;

        begin = MPI_Wtime();
        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &proc_number);
       
	 // вычиляем остаток при делении на количество потоков
        int remainder = size % proc_number;
        //printf("remainder is %d\n", remainder);

        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

        // делим массив
        // первые remainder потоков берут строки (size/proc_num)+1      
        if (my_rank < remainder) {
                start = (size / proc_number) * (my_rank) + my_rank;
                end = start + (size / proc_number);
        } else {
                start = (size / proc_number) * (my_rank) + remainder;
                end = start + (size / proc_number)-1;
        }
        //printf("my rank is %d, start is %d, end is %d\n", my_rank, start, end);

        int str_num = end - start + 1;
        //printf("\nmy rank is %d, str_num is %d\n", my_rank, str_num);

        // массив для хранения темпертур, будем хранить транспонированную матрицу температур, 
        // чтобы удобно было пересылать другим потокам в виде массива
        float** temp = (float**)malloc(str_num*sizeof(float*));
        for ( i = 0; i < str_num; ++i)
                temp[i] = (float*)malloc(size*sizeof(float));

        // массив для хранения промежуточного шага
        float** betw_temp = (float**)malloc(str_num*sizeof(float*));
        for ( i = 0; i < str_num; ++i)
                betw_temp[i] = (float*)malloc(size*sizeof(float));


        // массив для получения и отправки значений
        float buf[size];

        //заполнение начальными значениями
        for (str = 0; str < str_num; ++str)
                for (col = 0; col < size; ++col)
                         temp[str][col] = u_0;

        /*for (str = 0; str < str_num; ++str) {
                for (col = 0; col < size; ++col)
                         printf(" %.2f", temp[str][col]);
                printf("\n");
        }*/

        // цикл по итерациям
        // betw iter для экспериментов (чтобы проверить, что хотя бы на 1 итерации работает)
        int betw_iter = num_iter;
        //printf("my rank is %d\n", my_rank);
        for ( i = 0; i < betw_iter; i++) {

                //.......................................//
                //промежуточный шаг

                for ( str = 0; str < str_num; ++str) {

                        // заполняем вручную значения крайних ячеек
                        betw_temp[str][0] = temp[str][0] + mult*(temp[str][0] - 2*temp[str][0] + temp[str][1]);
                        //printf("str is %d, temp[str][0] is %.1f, temp[str][1] is %.1f, betw_temp[str][0] is %.1f\n", str, temp[str][0], temp[str][1], betw_temp[str][0]);

                        betw_temp[str][size-1] = temp[str][size-1] + mult*(temp[str][size-2] - 2*temp[str][size-1] + temp[str][size-1]);
                        //printf("temp[str][size-2] is %.1f, temp[str][size-3] is %.1f, betw_temp[str][size-1] is %.1f\n", temp[str][size-2], temp[str][size-3], betw_temp[str][size-1]);

                        for (col = 1; col < size - 1; ++col) {
                                betw_temp[str][col] = temp[str][col] + mult*(temp[str][col-1] - 2*temp[str][col] + temp[str][col+1]);
                                //if (col < 4) {
                                        //printf("col is %d, temp[str][col] is %.1f\n", col, temp[str][col]);
                                        //printf("temp[str][col-1] is %.1f,temp[str][col+1] is %.1f, betw_temp[str][col] is %.1f\n", temp[str][col-1], temp[str][col+1], betw_temp[str][col]);
                                //}
                        }
                }
                /*printf("\niteration %d, betw_temp\n", i);
                for (str = 0; str < str_num; ++str) {
                        for (col = 0; col < size; ++col)
                                printf(" %.f", betw_temp[str][col]);
                         printf("\n");
                 }*/

                //.......................................//
                //заполнение основной таблицы
		//пересылаем нижнему соседу свою последнюю строку
		//сначала четные пересылают нечетным, а потом наоборот
		if (my_rank % 2 == 0) {
			if (my_rank + 1 != proc_number) {
				//printf("my rank is %d and I send message to %d\n", my_rank, my_rank + 1);
				MPI_Send(temp[str_num-1], size, MPI_FLOAT, my_rank + 1, my_rank + 1, MPI_COMM_WORLD);
			}
		} else {
			 //printf("my rank is %d and I recieve message from %d\n", my_rank, my_rank - 1);
			 MPI_Recv(buf, size, MPI_FLOAT, my_rank - 1, my_rank, MPI_COMM_WORLD, &status);
		} 
		if (my_rank % 2 != 0) {
                        if (my_rank + 1 != proc_number) {
                        	//printf("my rank is %d and I send message to %d\n", my_rank, my_rank + 1);
      			        MPI_Send(temp[str_num-1], size, MPI_FLOAT, my_rank + 1, my_rank + 1, MPI_COMM_WORLD);
                	}
		} else {
			if (my_rank != 0){
                        	//printf("my rank is %d and I recieve message from %d\n", my_rank, my_rank - 1); 
				MPI_Recv(buf, size, MPI_FLOAT, my_rank - 1, my_rank, MPI_COMM_WORLD, &status);
               		}
		 }


                double up_temp = 0, down_temp = 0;
                  for ( str = 0; str < str_num - 1; ++str) {
                        for ( col = 0; col < size; ++col) {
                                if ( str == 0 ) {
                                        if (my_rank == 0)
                                                up_temp = u_l;
                                        else up_temp = buf[col];
                                } else up_temp = betw_temp[str-1][col];

                                temp[str][col] = betw_temp[str][col] + mult*(up_temp -  2*betw_temp[str][col] + betw_temp[str+1][col]);
                                /*if ( col < 4 ) {      
                                        printf("col is %d, temp[str][col] is %.1f\n", col, temp[str][col]);
                                        printf("betw_temp[str][col] is %.1f, betw_temp[str+1][col] is %.1f, up_temp is %.1f\n", betw_temp[str][col], betw_temp[str+1][col], up_temp);
                                }*/

                        }
                }
                // теперь наоборот пересылаем верхнему соседу его последнюю строку
                if (my_rank % 2 == 0) {
                        if (start != 0) {
				//printf("my rank is %d and I send message to %d\n", my_rank, my_rank - 1);
                                MPI_Send(temp[0], size, MPI_FLOAT, my_rank - 1, my_rank - 1, MPI_COMM_WORLD);
                	}
		} else {
			if (my_rank + 1 != proc_number) { 
				//printf("my rank is %d I recieve message from %d\n", my_rank, my_rank + 1);
                        	MPI_Recv(buf, size, MPI_FLOAT, my_rank + 1, my_rank, MPI_COMM_WORLD, &status);
			}
                }
                if (my_rank % 2 != 0) {
				//printf("my rank is %d and I send message to %d\n", my_rank, my_rank - 1);
                        	MPI_Send(temp[0], size, MPI_FLOAT, my_rank - 1, my_rank - 1, MPI_COMM_WORLD);
                } else {
			if (end != size - 1) {
				//printf("my rank is %d I recieve message from %d\n", my_rank, my_rank + 1);
				MPI_Recv(buf, size, MPI_FLOAT, my_rank + 1, my_rank, MPI_COMM_WORLD, &status);
			}
		}

                // заполняем последнюю строку
                for (col = 0; col < size; ++col) {
                        if (my_rank == proc_number - 1)
                                down_temp = u_r;
                        else down_temp = buf[col];

                        temp[str_num-1][col] = betw_temp[str_num-1][col] + mult*(down_temp -  2*betw_temp[str_num-1][col] + betw_temp[str_num-2][col]);
                }
                /*printf("\ntemp\n");
                for (str = 0; str < str_num; ++str) {
                        for (col = 0; col < size; ++col)
                                printf(" %.f", temp[str][col]);
                         printf("\n");
                 }*/
        }
        printf("\ntemp\n");
        for (str = 0; str < str_num; ++str) {
                //printf("rank %d ", my_rank);
                //for (col = 0; col < size; ++col)
                        printf(" %.f", temp[str][0]);
                printf("\n");
        }                                                                          
        /*printf("\nbetw_temp\n");
        for (str = 0; str < str_num; ++str) {
                for (col = 0; col < size; ++col)
                        printf(" %.f", betw_temp[str][col]);
                printf("\n");
        }*/

        MPI_Finalize();

        end_ = MPI_Wtime();
        total = end_ - begin;

        if (my_rank == 0) printf("\n\nproc_number %d, time %lf\n", proc_number, total);
        return 0;
}
                                              
