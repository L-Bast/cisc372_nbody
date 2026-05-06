#include <cuda_runtime.h>
#include <math.h>

#include "vector.h"
#include "config.h"

__global__ void compute_kernel(vector3* pos,
                               vector3* vel,
                               double* mass)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;

    if (i >= NUMENTITIES) return;

    vector3 accel = {0.0, 0.0, 0.0};

    for (int j = 0; j < NUMENTITIES; j++)
    {
        if (i == j) continue;

        vector3 d;

        for (int k = 0; k < 3; k++)
            d[k] = pos[i][k] - pos[j][k];

        double dist2 =
            d[0]*d[0] + d[1]*d[1] + d[2]*d[2];

        if (dist2 < 1e-10) continue;

        double dist = sqrt(dist2);

        double force =
            -GRAV_CONSTANT * mass[j] / dist2;

        for (int k = 0; k < 3; k++)
            accel[k] += force * d[k] / dist;
    }

    for (int k = 0; k < 3; k++)
    {
        vel[i][k] += accel[k] * INTERVAL;
        pos[i][k] += vel[i][k] * INTERVAL;
    }
}


void compute()
{
    int threads = 256;
    int blocks = (NUMENTITIES + threads - 1) / threads;

    compute_kernel<<<blocks, threads>>>(
        d_hPos,
        d_hVel,
        d_mass
    );

    cudaDeviceSynchronize();
}
