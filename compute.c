#include <cuda_runtime.h>
#include <math.h>

#include "vector.h"
#include "config.h"


extern vector3 *d_hPos;
extern vector3 *d_hVel;
extern double *d_mass;

__global__ void compute_kernel(vector3 *hPos, vector3 *hVel, double *mass)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= NUMENTITIES) return;

    vector3 accel_sum = {0,0,0};

    for (int j = 0; j < NUMENTITIES; j++) {
        if (i == j) continue;

        vector3 distance;
        for (int k = 0; k < 3; k++)
            distance[k] = hPos[i][k] - hPos[j][k];

        double dist2 =
            distance[0]*distance[0] +
            distance[1]*distance[1] +
            distance[2]*distance[2];

        double dist = sqrt(dist2);

        double accelmag = -GRAV_CONSTANT * mass[j] / dist2;

        for (int k = 0; k < 3; k++)
            accel_sum[k] += accelmag * distance[k] / dist;
    }

    for (int k = 0; k < 3; k++) {
        hVel[i][k] += accel_sum[k] * INTERVAL;
        hPos[i][k] += hVel[i][k] * INTERVAL;
    }
}

void compute()
{
    int threads = 256;
    int blocks = (NUMENTITIES + threads - 1) / threads;

    compute_kernel<<<blocks, threads>>>(d_hPos, d_hVel, d_mass);

    cudaDeviceSynchronize();
}
