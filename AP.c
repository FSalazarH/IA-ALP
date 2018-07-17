/* 

Aircraft Landing Problem with Tabu Search
Static Case
1 single Runway

*/

/* 

Objetivo: Dada una lista de aviones e intervalos de tiempos, crear calendarización de la secuencia de aterrizajes.
Minimizar los costos asociados a no respetar los tiempos ideales de aterrizaje de cada avión.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_AVIONES 500

struct Data {
	int numAviones;
	int avionesArray[MAX_AVIONES][3];
	float penalizaciones[MAX_AVIONES][2];
	int distancias[MAX_AVIONES][MAX_AVIONES];
};

//Funcion que carga los datos para los aviones, sus penalizaciones y distancias, recibe un puntero al archivo de instancia. 
struct Data initData(FILE *file){
	int numAviones;
	fscanf(file, "%i", &numAviones);

	int avionesArray[numAviones][3]; 
	float penalizaciones[numAviones][2];
	int distancias[numAviones][numAviones];

 	struct Data chargedData;
  	chargedData.numAviones = numAviones;

	char singleLine[255];

	//read the entire file
	int numLines = 0;
		
	while(numLines < numAviones){
		//Se asigna la matriz de aviones n x 3 que contiene los tiempos e_i, t_i y l_i para cada avión, n=numAviones.
		//Se asigna la matriz de penalizaciones n x 2 que contiene las penalizaciones g_i y h_i para cada avión, donde n=numAviones.
		fscanf(file, "%i", &avionesArray[numLines][0]);
		chargedData.avionesArray[numLines][0] = avionesArray[numLines][0];
		
		fscanf(file, "%i", &avionesArray[numLines][1]);
		chargedData.avionesArray[numLines][1] = avionesArray[numLines][1];
		
		fscanf(file, "%i", &avionesArray[numLines][2]);
		chargedData.avionesArray[numLines][2] = avionesArray[numLines][2];
		
		fscanf(file, "%f", &penalizaciones[numLines][0]);
		chargedData.penalizaciones[numLines][0] = penalizaciones[numLines][0];
		
		fscanf(file, "%f", &penalizaciones[numLines][1]);
		chargedData.penalizaciones[numLines][1] = penalizaciones[numLines][1];

		//siguiente linea (distancias) 
		for (int i=0; i<numAviones; i++){
			fscanf(file, "%i", &distancias[numLines][i]);
			chargedData.distancias[numLines][i] = distancias[numLines][i];
		}
		numLines+=1;
	}	
	printf("imprimiendo las distancias para los %i aviones \n", numAviones);
	for (int i=0; i<numAviones; i++){
		for (int j=0; j<3; j++){
			printf("%i ", chargedData.avionesArray[i][j]);
		}printf("\n");
	}
	printf("imprimiendo las penalizaciones para los %i aviones \n", numAviones);
	for (int i=0; i<numAviones; i++){
		for (int j=0; j<2; j++){
			printf("%f ", chargedData.penalizaciones[i][j]);
		}printf("\n");
	}
	printf("imprimiendo las distancias entre los %i aviones \n", numAviones);
	for (int i=0; i<numAviones; i++){
		for (int j=0; j<numAviones; j++){
			printf("%i ", chargedData.distancias[i][j]);
		}printf("\n");
	}
	return chargedData;
}

int main(int argc, char *argv[]) {
	//Se define un puntero a FILE para manejar archivos 
	FILE *file = NULL;

	if (argc != 2){
		printf("Debes ingresar el nombre del archivo!\n");
		return 1;
	}

	file = fopen(argv[1], "r");
	if (!file){
		perror("Error abriendo el archivo!\n");
		return 1;
	}

	struct Data chargedData = initData(file);
	printf("El total de aviones es: %i \n", chargedData.numAviones);

	fclose(file);
	return 0;
}

