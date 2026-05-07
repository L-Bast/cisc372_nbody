#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "vector.h"
#include "config.h"
#include "planets.h"
#include "compute.h"
#include <cuda_runtime.h>


vector3 *hVel, *d_hVel;
vector3 *hPos, *d_hPos;
double *mass;
double *d_mass;


void initHostMemory(int numObjects)
{
	hVel = (vector3 *)malloc(sizeof(vector3) * numObjects);
	hPos = (vector3 *)malloc(sizeof(vector3) * numObjects);
	mass = (double *)malloc(sizeof(double) * numObjects);
}

void freeHostMemory()
{
	free(hVel);
	free(hPos);
	free(mass);
}


void planetFill(){
	int i,j;
	double data[][7]={SUN,MERCURY,VENUS,EARTH,MARS,JUPITER,SATURN,URANUS,NEPTUNE};
	for (i=0;i<=NUMPLANETS;i++){
		for (j=0;j<3;j++){
			hPos[i][j]=data[i][j];
			hVel[i][j]=data[i][j+3];
		}
		mass[i]=data[i][6];
	}
}


void randomFill(int start, int count)
{
	int i, j;
	for (i = start; i < start + count; i++)
	{
		for (j = 0; j < 3; j++)
		{
			hVel[i][j] = (double)rand() / RAND_MAX * MAX_DISTANCE * 2 - MAX_DISTANCE;
			hPos[i][j] = (double)rand() / RAND_MAX * MAX_VELOCITY * 2 - MAX_VELOCITY;
			mass[i] = (double)rand() / RAND_MAX * MAX_MASS;
		}
	}
}


void printSystem(FILE* handle){
	int i,j;
	for (i=0;i<NUMENTITIES;i++){
		fprintf(handle,"pos=(");
		for (j=0;j<3;j++){
			fprintf(handle,"%lf,",hPos[i][j]);
		}
		fprintf(handle,"),v=(");
		for (j=0;j<3;j++){
			fprintf(handle,"%lf,",hVel[i][j]);
		}
		fprintf(handle,"),m=%lf\n",mass[i]);
	}
}

int main(int argc, char **argv)
{
	clock_t t0=clock();
	int t_now;

	srand(1234);

	initHostMemory(NUMENTITIES);
	planetFill();
	randomFill(NUMPLANETS + 1, NUMASTEROIDS);


	cudaMalloc((void**)&d_hPos, sizeof(vector3) * NUMENTITIES);
	cudaMalloc((void**)&d_hVel, sizeof(vector3) * NUMENTITIES);
	cudaMalloc((void**)&d_mass, sizeof(double) * NUMENTITIES);


	cudaMemcpy(d_hPos, hPos, sizeof(vector3) * NUMENTITIES, cudaMemcpyHostToDevice);
	cudaMemcpy(d_hVel, hVel, sizeof(vector3) * NUMENTITIES, cudaMemcpyHostToDevice);
	cudaMemcpy(d_mass, mass, sizeof(double) * NUMENTITIES, cudaMemcpyHostToDevice);

#ifdef DEBUG
	printSystem(stdout);
#endif

	for (t_now=0; t_now<DURATION; t_now+=INTERVAL){
		compute();
	}


	cudaMemcpy(hPos, d_hPos, sizeof(vector3) * NUMENTITIES, cudaMemcpyDeviceToHost);
	cudaMemcpy(hVel, d_hVel, sizeof(vector3) * NUMENTITIES, cudaMemcpyDeviceToHost);

	clock_t t1=clock()-t0;

#ifdef DEBUG
	printSystem(stdout);
#endif

	printf("This took a total time of %f seconds\n",(double)t1/CLOCKS_PER_SEC);

	cudaFree(d_hPos);
	cudaFree(d_hVel);
	cudaFree(d_mass);

	freeHostMemory();
}
