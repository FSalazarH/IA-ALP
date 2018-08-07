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

//Struct que contiene toda la data inicial
struct Data {
	int numAviones;								//number of planes
	int avionesArray[MAX_AVIONES][3];			//matrix with landing times
	float penalizaciones[MAX_AVIONES][2];		//matrix with e_i and l_i (penalizations)
	int distancias[MAX_AVIONES][MAX_AVIONES];	//matrix with distances
};

//Struct que contendrá la solución y su penalización
struct Solution{
	int solution[MAX_AVIONES];			//num aviones
	float penalizacion[MAX_AVIONES];	//penalizaciones para cada avion
	int totalPenalizacion;				//costo total de la solucion (en penalizaciones)
	int factible;
};

//Funcion que carga los datos para los aviones, sus penalizaciones y distancias, recibe un puntero al archivo de instancia y retorna el struct Data con la data.
struct Data initData(FILE *file){
	int numAviones;
	fscanf(file, "%i", &numAviones);			//read file

	int avionesArray[numAviones][3]; 			//set times e_i, t_i and l_i for each plane
	float penalizaciones[numAviones][2];		//set penalizations g_i and h_i
	int distancias[numAviones][numAviones];		//set matrix of distances between all planes

 	struct Data chargedData;					//create struct Data with data from file
  	chargedData.numAviones = numAviones;		//set number of planes

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
	return chargedData;
}

//Create initial random solution for all airplanes in their respective landing window
//Recibe el struct Data y retorna una solución inicial que puede o no ser factible
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
	return initialSolution;
}

//Check separation times between all planes, return true if all distances are ok and return false in other case
//recibe Data y retorna true o false
int checkDistances(struct Data chargedData, int initialSolution[]){
	
	int boolDistances;

	for (int i=0; i<chargedData.numAviones; i++){			//for each plane 

		int land1 = initialSolution[i];						//landing time for plane i

		for (int j=0; j<chargedData.numAviones; j++){		//for each plane where i=!j
			if (i != j){
				int land2 = initialSolution[j];				//landing time for plane j

				//If distance is OK
				if(abs(land2-land1) > chargedData.distancias[i][j]){	//distances are OK
					boolDistances = true;
				}
				else{	
					boolDistances = false;
					return boolDistances;		//if one distance doesn't respected, return false
				}
			}
		}
	}
	return boolDistances;
}

//Función que setea las penalizaciones para los aterrizajes de cada avión de la solución inicial
float * penalizaciones(struct Data chargedData, int initialSolution[]){
	
	static float penalizaciones[MAX_AVIONES];

	for (int i=0; i<chargedData.numAviones; i++){

		int ti = chargedData.avionesArray[i][1]; //ideal landing time for each plane

		int realTime = initialSolution[i]; 		 //real landing time of our solution

		//compare landing time for penalization
		if(realTime > ti){
			int delta = realTime - ti;
			penalizaciones[i] = delta*chargedData.penalizaciones[i][1];
		}
		else if(realTime == ti){
			penalizaciones[i] = 0.000000; //Caso de aterrizar en el ideal landing time
		}
		else{
			int delta = ti - realTime;
			penalizaciones[i] = delta*chargedData.penalizaciones[i][0];
		}
	}
	return penalizaciones;
}

int setTotalPenalization(int numAviones, struct Solution solution){
	int totalPenalizacion = 0;
	for (int i=0; i<numAviones; i++){
		totalPenalizacion += solution.penalizacion[i];	//Sum total penalizations
	}
	printf("El total de penalizaciones es %i\n", totalPenalizacion);
	return 1;
}

//Función que genera el vecindario y evalúa las penalizaciones de los vecinos, retorna el mejor vecino factible y su posición
struct Solution * getNeighborhood(int numAviones, struct Solution initSolution, struct Data chargedData){
	struct Solution solution;					//set solution
	static struct Solution neighborhood[MAX_AVIONES];	//set neighborhood structure
	solution = initSolution;					//set solution to initSolution

	/*Se genera el vecidario tras restarle 50 al avión i */
	for(int i=0; i<numAviones;i++){
		for(int j=0; j<numAviones; j++){
			if(i==j){
				neighborhood[i].solution[j] = solution.solution[j]-50; 
			}
			else{
				neighborhood[i].solution[j] = solution.solution[j];
			}
		}
	}

	int aux_solution[numAviones][numAviones];	//array with solutions

	//set array with solutions
	for(int i=0; i<numAviones; i++){
		for(int j=0; j<numAviones; j++){
			aux_solution[i][j] = neighborhood[i].solution[j];
		}
	}

	//check hard constraint for each solution
	for(int i=0; i<numAviones; i++){
		int aux_bool = checkDistances(chargedData, aux_solution[i]);
		neighborhood[i].factible = aux_bool;
	}
		
	printf("\n------ VECINDARIO ------- \n \n");
	for (int i=0; i<numAviones; i++){
		for(int j=0; j<numAviones; j++){
			printf("%i ", neighborhood[i].solution[j]);
		}printf("    ----       factible? %i", neighborhood[i].factible);
		printf("\n");
	}
	return neighborhood;
}

//Funcion que setea la elección del siguiente movimiento y guarda en la lista tabú el número de la solución escogida 
void selectBestNeighbor(int numAviones, struct Data chargedData, struct Solution * neighborhood, int * tabuList){
	float *penalizacionesArr[numAviones][numAviones];
	float *penalizacionesAux[numAviones];

	for(int i=0; i<numAviones; i++){
		penalizacionesAux[i] = 	penalizaciones(chargedData, neighborhood[i].solution); 
		printf("penalizacion del vecino %i :\n", i);
		for(int j=0; j<numAviones; j++){
			printf("%f ", penalizacionesAux[i][j]);
			neighborhood[i].penalizacion[j] = penalizacionesAux[i][j];
		}printf("\n");
	}printf("\n");

	for(int i=0; i<numAviones; i++){
		for(int j=0; j<numAviones; j++){
			printf("%f ", neighborhood[i].penalizacion[j]);
		}printf("\n");
	}
}

struct Solution solutionTabuSearch(int numAviones, struct Solution initSolution, struct Data chargedData){
	
	int tabuList[10];					//lista tabú tamaño 10
	struct Solution initialSolution;	//solución inicial factible
	struct Solution bestSolution;		//best solution
	int max_iter = 10;		
	int max_solutions = 10;			//max nro iteraciones
	struct Solution arraySolutions[max_solutions];	//arreglo que contendrá las soluciones para compararlas en el tiempo

	initialSolution = initSolution; 	//setting initial solution
	bestSolution = initialSolution;		//setting best solution

	struct Data data = chargedData;

	//generate Neighborhood of Sbest with factibility
	struct Solution * neighborhood = getNeighborhood(numAviones, bestSolution, data);

	//get best neighbor of neighborhood
	selectBestNeighbor(numAviones, chargedData, neighborhood, tabuList);
	
	return bestSolution;
}

int main(int argc, char *argv[]) {

	clock_t begin = clock();	//begin clock

	FILE *file = NULL;			//Se define un puntero a FILE para manejar archivos 

	if (argc != 2){	 
		printf("Debes ingresar el nombre del archivo!\n");
		return 1;
	}

	file = fopen(argv[1], "r");
	if (!file){
		perror("Error abriendo el archivo!\n");
		return 1;
	}

	struct Data chargedData = initData(file);	//charge initial Data from file

	int *initialSolution;	//define pointer to initialSolution Array

	initialSolution = initialSolutions(chargedData);	//get initial solution 

	int boolDistances;	
	boolDistances = checkDistances(chargedData, initialSolution);	//Check the separation times between all planes
	printf("La distancia se cumple: %i \n \n", boolDistances);	//1 true, 0 false

	int *finalInitialSolution;	//final initial Solution (factible)

	if (boolDistances == true){	//check factibility of first initial solution
		printf("La solucion cumple con las restricciones duras");
	}
	else{	//try with other solutions 
		for (;;) {	//stop when encounter a factible initial solution
			finalInitialSolution = initialSolutions(chargedData);
			boolDistances = checkDistances(chargedData, finalInitialSolution);
			if (boolDistances == true){
				break;
			}
		}
	}
	printf("La solución es      : ");
	for (int i=0; i<chargedData.numAviones; i++){
		printf("%i ", finalInitialSolution[i]);
	}printf("\n");
	printf("La solución IDEAL es: ");
	for (int i=0; i<chargedData.numAviones; i++){
		printf("%i ", chargedData.avionesArray[i][1]);
	}printf("\n");
	printf("\n");

	struct Solution solutionPenalizations;	//penalizations for finalInitialSolution

	int totalPenalizacion = 0; 

	for(int i=0; i<chargedData.numAviones; i++){	//Add finalInitialSolution to struct that contain the penalizations for this solution
		solutionPenalizations.solution[i] = finalInitialSolution[i];
	}

	float *penalizacionesArray = penalizaciones(chargedData, finalInitialSolution);	//Seteando las penalizaciones

	for(int i=0; i<chargedData.numAviones; i++){
		solutionPenalizations.penalizacion[i] = penalizacionesArray[i];
	}

	setTotalPenalization(chargedData.numAviones, solutionPenalizations);	//seteando el total de penalizacion

	printf("las penalizaciones son: ");
	for(int i=0; i<chargedData.numAviones; i++){
		printf("%f ", solutionPenalizations.penalizacion[i]);
	}printf("\n");

	//Init Tabu Search
	solutionTabuSearch(chargedData.numAviones, solutionPenalizations, chargedData);


	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("\nEl programa tarda %f en encontrar una solucion factible \n", time_spent);

	fclose(file);
	return 0;
}

