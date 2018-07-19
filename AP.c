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
#include <time.h>
#define MAX_AVIONES 500
#define false 0
#define true 1

struct Data {
	int numAviones;								//number of planes
	int avionesArray[MAX_AVIONES][3];			//matrix with landing times
	float penalizaciones[MAX_AVIONES][2];		//matrix with e_i and l_i (penalizations)
	int distancias[MAX_AVIONES][MAX_AVIONES];	//matrix with distances
};

struct penalSolution{
	int initialSolution[MAX_AVIONES];
	float penalizacionSolution[MAX_AVIONES];
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
	/*printf("imprimiendo las penalizaciones para los %i aviones \n", numAviones);
	for (int i=0; i<numAviones; i++){
		for (int j=0; j<2; j++){
			printf("%f ", chargedData.penalizaciones[i][j]);
		}printf("\n");
	}*/
	printf("imprimiendo las distancias entre los %i aviones \n", numAviones);
	for (int i=0; i<numAviones; i++){
		for (int j=0; j<numAviones; j++){
			printf("%i ", chargedData.distancias[i][j]);
		}printf("\n");
	}
	return chargedData;
}

//Create initial random solution for all airplanes in their respective landing time
int * initialSolutions(struct Data chargedData){
	//generar solución aleatoria para cada avión
	static int initialSolution[MAX_AVIONES];

	//setting random seed	
	srand(time(NULL));
	
	for (int j=0; j<chargedData.numAviones; j++){

		//get min and max airland time for each airplane
		int min = chargedData.avionesArray[j][0];
		int opt = chargedData.avionesArray[j][1];
		int max = chargedData.avionesArray[j][2];

		//generate random time for each airplane between e_i and l_i
		int num = (rand() % (max - min +1)) + min;

		//add random number to initialSolution
		initialSolution[j] = num;
	}

	printf("La solucion inicial es: ");
	for (int i=0; i<chargedData.numAviones; i++){
		printf("%i ", initialSolution[i]);
	} printf("\n ");

	return initialSolution;
}

//Check separation times between all planes, return true if all distances are ok and return false in other case
int checkDistances(struct Data chargedData, int initialSolution[]){
	int boolDistances;

	for (int i=0; i<chargedData.numAviones; i++){

		int land1 = initialSolution[i];
		printf("Comparando aterrizaje %i : %i \n",i, land1);

		for (int j=0; j<chargedData.numAviones; j++){
			if (i != j){
				int land2 = initialSolution[j];
				printf("Con aterrizaje %i : %i \n", j , land2);
				printf("La separacion entre ambos debe ser mayor a: %i \n", chargedData.distancias[i][j]);
				printf("Y es... %i \n ", abs(land2-land1));

				//If distance is OK
				if(abs(land2-land1) > chargedData.distancias[i][j]){
					printf("Se respeta la distancia \n");
					boolDistances = true;
				}
				else{
					printf("No se respeta la distancia entre avion %i y %i \n", i, j);
					printf("Las distancias son %i y %i \n ", land1, land2);
					boolDistances = false;
					return boolDistances;
				}
			}
		}
	}
	return boolDistances;
}

float * penalizaciones(struct Data chargedData, int initialSolution[], struct penalSolution penalSolution){
	float penalizaciones[MAX_AVIONES];
	for (int i=0; i<chargedData.numAviones; i++){

		int ti = chargedData.avionesArray[i][1]; //ideal landing time for each plane

		int realTime = initialSolution[i]; 		 //real landing time of our solution

		//compare landing time for penalization
		if(realTime > ti){
			penalizaciones[i] = *chargedData.penalizaciones[1];
			printf("Agregando penalizacion %f \n", *chargedData.penalizaciones[1]);
		}
		else{
			penalizaciones[i] = *chargedData.penalizaciones[0];
		}
	}
}

int main(int argc, char *argv[]) {
	//begin clock
	clock_t begin = clock();

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

	//charge initial Data from file
	struct Data chargedData = initData(file);

	//define pointer to initialSolution Array
	int *initialSolution;

	//get initial solution 
	initialSolution = initialSolutions(chargedData);

	//Check the separation times between all planes
	int boolDistances;
	boolDistances = checkDistances(chargedData, initialSolution);
	printf("La distancia se cumple: %i \n", boolDistances);

	int *finalInitialSolution;

	//check factibility of first initial solution
	if (boolDistances == true){
		printf("La solucion cumple con las restricciones duras");
	}
	else{
		for (;;) {
			finalInitialSolution = initialSolutions(chargedData);
			boolDistances = checkDistances(chargedData, finalInitialSolution);
			if (boolDistances == true){
				break;
			}
		}
	}
	printf("La solución es : ");
	for (int i=0; i<chargedData.numAviones; i++){
		printf("%i ", finalInitialSolution[i]);
	}printf("\n ");

	struct penalSolution penalSolution;
	for(int i=0; i<chargedData.numAviones; i++){
		penalSolution.initialSolution[i] = finalInitialSolution[i];
	}
	printf("el primer valor de la solucion inicial es: %i \n", penalSolution.initialSolution[0]);

	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("El programa tarda %f en encontrar una solucion factible \n", time_spent);

	fclose(file);
	return 0;
}

