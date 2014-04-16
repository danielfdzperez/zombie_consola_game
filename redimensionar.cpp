#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

int **redimensionar(int **tablero, int *tamano_x, int *tamano_y, int *max_x, int *max_y){

    int diferencia;
    if(*tamano_x > COLS-1)
	*tamano_x = COLS - 1;

    if(*tamano_y > LINES)
	*tamano_y = LINES;

    if(*tamano_x < COLS-1)
	if (COLS - 1 <= *max_x)
	    *tamano_x = COLS - 1;
	else{
	    diferencia = (COLS-1) - *tamano_x;
	    *max_x = *tamano_x = COLS - 1; 
	    for (int i= 0; i<*tamano_y; i++)
		tablero[i] = (int *) malloc(*tamano_x * sizeof(int));
	}

    if(*tamano_y < LINES)
	if(LINES <= *max_y)
	    *tamano_y = LINES;
	else{
	    diferencia = LINES - *tamano_y;
	    *max_y = *tamano_y = LINES;
	    tablero =(int**) realloc(tablero, *tamano_y * sizeof(int*));
	    for(int i = 0; i<diferencia; i++){
		tablero[*tamano_y-(i+1)] = (int*) malloc(*tamano_x * sizeof(int));
	    }
	}

    return tablero;
}

void pintar(int **tablero, int tamano_x, int tamano_y){

    clear();
    for(int y=0; y<tamano_y; y++){
	for(int x=0; x<tamano_x; x++)
	    printw("%i",tablero[y][x]);
	printw("\n");
    }
    refresh();
}

void rellenar(int **tablero, int tamano_y, int tamano_x){
    for(int i=0; i<tamano_y; i++)
	memset(tablero[i], 0, tamano_x * sizeof(int));
    for(int y=0; y<tamano_y; y++)
	for(int x=0; x<tamano_x; x++)
	    if(y == 0 || y == tamano_y - 1 || x == 0 || x == tamano_x - 1)
		tablero[y][x] = 5;
}

int **inicializar(int **tablero, int tamano_y, int tamano_x){

    tablero = (int **) malloc(tamano_y * sizeof (int *));
    for (int i= 0; i<tamano_y; i++)
	tablero[i] = (int *) malloc(tamano_x * sizeof(int));

    rellenar(tablero, tamano_y, tamano_x);
    return tablero;
}

int main(){
    initscr();
    raw();
    keypad(stdscr, TRUE); 
    noecho();
    start_color();
    int **tablero;
    char a = 'j';
    int tamano_x,
	tamano_y,
	max_x,
	max_y;

    max_x = tamano_x = COLS-1;
    max_y = tamano_y = LINES;

    tablero = inicializar(tablero, tamano_y, tamano_x);

    while(a != 'q'){

	//redimensiona el tablero
	if(tamano_x < COLS-1 || tamano_x > COLS-1 || tamano_y < LINES || tamano_y > LINES){
	    tablero = redimensionar(tablero, &tamano_x, &tamano_y, &max_x, &max_y);

	    rellenar(tablero, tamano_y, tamano_x);
	}
	pintar(tablero, tamano_x, tamano_y);
	a = getch();
    }

    //libera la memoria 
    for(int i = 0; i<tamano_y; i++)
	free(tablero[i]);
    free(tablero);
    endwin();
    return EXIT_SUCCESS;
}
