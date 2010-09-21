#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define FIL 3
#define COL 5
#define AST printf("******************\n");
typedef int TipoLinea[5];
typedef TipoLinea TipoCarton[3]; 

void generarCarton(TipoCarton carton);
void ordenarCarton(TipoCarton carton);
int jugar(int bolillero[], TipoCarton carton1, TipoCarton carton2);
void imprimirCarton(TipoCarton carton);
int sacarBolilla(int bolillero[], int *cantBolillas);
int controlarCarton(TipoCarton carton, int bolilla);
int controlarLineas(TipoLinea linea);
int buscarBolilla(TipoCarton carton, int bolilla);


int 
bingo(int argc, char * argv[])
{

	int i, ganador;
	TipoCarton carton1;
	TipoCarton carton2;
	int bolillero[90];
	/*Lleno el bolillero*/
	for ( i = 0; i < 90; i++)
		bolillero[i] = i+1;

	generarCarton(carton1);
	ordenarCarton(carton1);
	generarCarton(carton2);
	ordenarCarton(carton2);

	ganador = jugar(bolillero, carton1, carton2);
	if ( ganador == 1)
		printf("Bingo para el jugador 1!\n");
	else if ( ganador == 2)
		printf("Bingo para el jugador 2!\n");
	else if ( ganador == 3 )
		printf("Bingo para ambos. Es un empate.\n");
	else
		printf("Termino el juego.\n");
	AST;
	return 0;
}


void
generarCarton(TipoCarton carton)
{
	int i, j, k, m;

	for ( i = 0; i < FIL; i++)
		for ( j = 0; j < COL; j++)
		{  
			carton[i][j] = rand()%90+1;
			/* Para hacer que no tengan elementos repetidos */
			for ( k = 0, m = 0; !(k == i && m == j); m++)
			{
				if ( carton[k][m] == carton[i][j] )
				{
					carton[i][j] = rand()%90+1;
					k = 0;
					m = -1; /* Porque luego se incrementa al terminar el loop */
				}
				if ( m == COL-1 ) /*Para comparar con la siguiente fila */
				{
					k++;
					m = -1;
				}
			}
					
		}
	
	
}


void
imprimirCarton(TipoCarton carton)
{
	int i, j;
	for ( i = 0; i < FIL; i++)
	{
		for ( j = 0; j < COL; j++)
		{
			if ( carton[i][j] == -1 )
				printf("X\t");
			else
				printf("%d\t", carton[i][j]);
		}
		printf("\n");
	}
}


int
jugar(int bolillero[], TipoCarton carton1, TipoCarton carton2)
{
	int cantBolillas = 90, bolilla, cantLineas1, cantLineas2, primeraLinea = 0, c;
	clear();
	printf("BINGO\n");
	AST;
	while ( cantBolillas > 0 )
	{
		printf("Carton 1:\n");
		imprimirCarton(carton1);
		AST;
		printf("Carton 2:\n");
		imprimirCarton(carton2);
		AST;
		
		bolilla = sacarBolilla(bolillero, &cantBolillas);
		printf("Bolilla %d\n", bolilla);
		
		cantLineas1 = controlarCarton(carton1, bolilla);
		cantLineas2 = controlarCarton(carton2,bolilla);
		AST;
		/*Verifica quien saco primera linea */
		if ( cantLineas1 == 1 && primeraLinea == 0 )
		{
			printf("Linea para el jugador 1!\n");
			AST;
			primeraLinea = 1;
		}
		else if ( cantLineas2 == 1 && primeraLinea == 0 )
		{
			printf("Linea para el jugador 2!\n");
			AST;
			primeraLinea = 1;
		}
		
		/* Verifica cuando haya Bingo */
		if ( cantLineas1 == FIL && cantLineas2 == FIL )
			return 3;
		else if ( cantLineas1 == FIL)
			return 1;
		else if ( cantLineas2 == FIL ) 
			return 2;
		
		do
			printf("Presione la tecla ENTER para continuar(q para terminar).\n");
		while ( (c=getchar()) != 'q' && c != '\n' );
		if ( c == 'q' )
				return -1;
	}
	return 0;
}	


int
sacarBolilla(int bolillero[], int *cantBolillas)
{
	int index, flag = 0, bolilla;
	while ( flag == 0 )
	{
		index = rand()%90;
		bolilla = bolillero[index];
		if ( bolilla != -1 ) /* Si la bolilla ya salio seguira randomizando el index hasta que sea nueva */
		{	
			flag = 1;
			bolillero[index] = -1;
			*cantBolillas--;
		}	
	}
	return bolilla;
}
	

int
controlarCarton(TipoCarton carton, int bolilla)
{
	int i, value, cantLineas = 0;
	
	/* Se fija si el numero de bolilla coincide con alguno de los del carton y lo marca con una X */
	value = buscarBolilla(carton, bolilla);
	
	/* Se fija cuantas lineas ya tiene completas */
	for ( i=0; i < FIL; i++)
		cantLineas += controlarLineas(carton[i]);
	return cantLineas;
}
	

int
controlarLineas(TipoLinea linea)
{
	int j, contador = 0;
	for ( j=0;j < COL; j++)
		if ( linea[j] == -1 )/* Si ya fue marcada entonces que la cuente */
			contador++;
	if ( contador == COL )
		return 1;
	else
		return 0;
}
	

int
buscarBolilla(TipoCarton carton, int bolilla)
{
	int i, j;
	for ( i=0; i < FIL; i++)
		for ( j=0; j < COL;j++)
			if ( carton[i][j] == bolilla )
			{
				carton[i][j] = -1; /* La marca para que se sepa que ya salio */
				return 1;
			}
	return 0;
}

void
ordenarCarton(TipoCarton carton)
{
	int i, j, n, aux;
	for ( n = 0; n < FIL*COL; n++)
	{
		/*Primero manda todos los maximos a la ultima columna de cada fila */
		for ( i = 0; i < FIL; i++)
			for ( j = 0; j < COL - 1; j++)
			{
				if ( carton[i][j] > carton[i][j+1] )
				{
					aux = carton[i][j+1];
					carton[i][j+1] = carton[i][j];
					carton[i][j] = aux;
				}
			}
		 /* Compara el maximo de cada linea con el primero de la siguiente */
		for ( i = 0, j = COL - 2; i<FIL-1; i++)
			if ( carton[i][j+1] > carton[i+1][0] )
			{
				aux = carton[i][j+1];
				carton[i][j+1] = carton[i+1][0];
				carton[i+1][0] = aux;
			}
	}
}
