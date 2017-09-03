#include <stdio.h>
//#include <omp.h>
#include <stdlib.h>

int life_time = 20;
int Lx, Ly;

#define dfor(a, b) for(a = 0; a < Ly; ++a) { for(b = 0; b < Lx; ++b) {
#define dbrakcets } }

int neigh_counter(int str, int col, char** field) {
	int counter = 0, i, j;
	//printf("\nstr %d, col %d\n", str, col);
	for (i = -1; i <= 1; ++i) {
		for (j = -1; j <= 1; ++j) {
			// если угловая клетка, то у неё на одного соседа меньше, саму клетку мы не считаем
			if ( (str+i < 0)&&((col+j >= Lx)||(col+j < 0)) || (str+i >= Ly)&&((col+j >= Lx)||(col+j < 0)) || (i == 0)&&(j == 0))
				continue;
			//printf("%d %d\n", (str+i+Ly) % Ly, (col+j+Lx)%Lx);

			if ( field[(str+i+Ly) % Ly][(col+j+Lx) % Lx] == '#')
				++counter;
		}
	}
	//printf("%d\n\n\n", counter);
	return counter;
}

void print_table(char** table) {
	int str, col;
	for (col = 0; col < Lx+3; ++col)
                printf("-");
        printf("\n");
        
	for (str = 0; str < Ly; ++str) {
                for (col = 0; col < Lx; ++col) {
                        if (col == 0)
                                printf("|");
                        printf("%c", table[str][col]);
                        if (col == Lx - 1)
                                printf("|");
                }
                printf("\n");
        }
        
	for (col = 0; col < Lx+3; ++col)
                printf("-");
        printf("\n\n");
}

void update_table(char** field, int** betw) {
	int str, col;
	for (str = 0 ; str < Ly; ++str)
                for (col = 0; col < Lx; ++col) {
                	if (field[str][col] == '#')
               			if ((betw[str][col] != 2) && (betw[str][col] != 3))
                        		field[str][col] = ' ';
                	else
                        	if (betw[str][col] == 3)
                               		field[str][col] = '#';

        	 }
}


int main(){

	int str, col, t, counter;

	// считывание данных
	FILE* data;
	data = fopen("state.txt", "r");
	fscanf(data, "%d%d", &Lx, &Ly);
	
	char** field = (char**)malloc(Ly*sizeof(char*));
	for(str = 0; str < Ly; ++str) {
		field[str] = (char*)malloc(Lx*sizeof(char));
	}
	
	int** betw = (int**)malloc(Ly*sizeof(int*));
        for(str = 0; str < Ly; ++str) {
                betw[str] = (int*)malloc(Lx*sizeof(int));
        }


	// заполнение таблицы 	
	for (str = 0 ; str < Ly; ++str) {
        	for (col = 0; col < Lx; ++col) {
                	field[str][col] = ' ';
		}
	}

	while (fscanf(data, "%d%d", &str, &col) != EOF) {
		field[str][col] = '#';
	}

	print_table(field);

	// .................................................................................................//
	for ( t = 0; t < life_time; ++t) {
		for (str = 0 ; str < Ly; ++str) {
			for (col = 0; col < Lx; ++col) {
				counter = neigh_counter(str, col, field);
				betw[str][col] = counter;
			}
		}

		/*dfor(str, col);
		if (col == 0) printf("\n");
		printf("%d ", betw[str][col]);
		dbrakcets;*/

		for (str = 0 ; str < Ly; ++str) {
                        for (col = 0; col < Lx; ++col) {
                                if (field[str][col] == '#')
			                if ((betw[str][col] != 2) && (betw[str][col] != 3))
                        			field[str][col] = ' ';
               			if (field[str][col] == ' ')
                        		if (betw[str][col] == 3)
                               			field[str][col] = '#';

                        }
                }
		printf("\n\n");
		//update_table(field, betw);
		print_table(field);
		//printf("\n\n");
	}
 
	return 0;
}
