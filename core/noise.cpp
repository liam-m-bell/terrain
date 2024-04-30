// This code is adapted from the original Java Code by Ken Perlin (2002)
// Which can be found here: https://cs.nyu.edu/~perlin/noise/

#include<stdlib.h>
#include<stdio.h>
#include<math.h>

#include "noise.h"

int p[512];

double fade(double t) { return t * t * t * (t * (t * 6 - 15) + 10); }
double lerp(double t, double a, double b) { return a + t * (b - a); }
double grad(int hash, double x, double y, double z) {
      int h = hash & 15;                      
      double u = h<8 ? x : y,                 
             v = h<4 ? y : h==12||h==14 ? x : z;
      return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
   }
   
double noise3D(double x, double y, double z) {
      int X = (int)floor(x) & 255,                  
          Y = (int)floor(y) & 255,                  
          Z = (int)floor(z) & 255;
      x -= floor(x);                                
      y -= floor(y);                                
      z -= floor(z);
      double u = fade(x),                                
             v = fade(y),                                
             w = fade(z);
      int A = p[X  ]+Y, AA = p[A]+Z, AB = p[A+1]+Z,      
          B = p[X+1]+Y, BA = p[B]+Z, BB = p[B+1]+Z;   
 
      return lerp(w, lerp(v, lerp(u, grad(p[AA  ], x  , y  , z   ), 
                                     grad(p[BA  ], x-1, y  , z   )),
                             lerp(u, grad(p[AB  ], x  , y-1, z   ), 
                                     grad(p[BB  ], x-1, y-1, z   ))),
                     lerp(v, lerp(u, grad(p[AA+1], x  , y  , z-1 ), 
                                     grad(p[BA+1], x-1, y  , z-1 )), 
                             lerp(u, grad(p[AB+1], x  , y-1, z-1 ),
                                     grad(p[BB+1], x-1, y-1, z-1 ))));
}

double noise2D(double x, double y){
	return noise3D(x, y, 0.0);
}

void loadNoisePermutation(char* fileName){
	FILE* fp = fopen(fileName,"r");
	int permutation[256],i;
	
	for(i=0;i<256;i++)
		fscanf(fp,"%d",&permutation[i]);
	
	fclose(fp);
	
	for (int i=0; i < 256 ; i++) p[256+i] = p[i] = permutation[i];
}

// Combines layers octaves of perlin noise
double perlinNoise(Vector p, int octaves, double lacunarity, double persistence, double scale){
	double sum = 0.0;
	double frequency = scale;
	double amplitude = 1.0;

	for(int i = 0; i < octaves; i++) {
		double n = noise2D(p.x * frequency, p.y * frequency);
		sum += n * amplitude;
		frequency *= lacunarity;
		amplitude *= persistence;
	} 

	return sum;
}

// Billow noise is the absolute value of perlin noise
double billowNoise(Vector p, int octaves, double lacunarity, double gain, double scale){
	return fabs(perlinNoise(p, octaves, lacunarity, gain, scale));
}

// Ride noise and billow noise sum's to 1
double ridgeNoise(Vector p, int octaves, double lacunarity, double gain, double scale){
	return 1.0f - billowNoise(p, octaves, lacunarity, gain, scale);
}

// Warped noise changes the position the noise is sampled from to create a warping effect
double warpedNoise(Vector warp, double warpScale, Vector p, int octaves, double lacunarity, double persistence, double scale){
    Vector offset = Vector(perlinNoise(p, octaves, lacunarity, persistence, scale), 
            perlinNoise(p + warp, octaves, lacunarity, persistence, scale));

    return perlinNoise(p + offset * warpScale, octaves, lacunarity, persistence, scale);
}

double randRange(double min, double max){
    return min + (max - min) * ((double)rand() / RAND_MAX);
}