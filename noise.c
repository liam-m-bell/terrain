//https://rosettacode.org/wiki/Perlin_noise

#include<stdlib.h>
#include<stdio.h>
#include<math.h>

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


// Noise functions

float perlinNoise(float x, float y, int octaves, float lacunarity, float persistence, float scale){
	float sum = 0.0f;
	float frequency = scale;
	float amplitude = 1.0f;

	for(int i = 0; i < octaves; i++) {
		float n = noise2D(x * frequency, y * frequency);
		sum += n * amplitude;
		frequency *= lacunarity;
		amplitude *= persistence;
	} 

	return sum;
}

float billowNoise(float x, float y, int octaves, float lacunarity, float gain, float scale){
	return fabs(perlinNoise(x, y, octaves, lacunarity, gain, scale));
}

float ridgeNoise(float x, float y, int octaves, float lacunarity, float gain, float scale){
	return 1.0f - billowNoise(x, y, octaves, lacunarity, gain, scale);
}