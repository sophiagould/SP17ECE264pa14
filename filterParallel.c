#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "filterParallel.h"

void applyThread(BMPImage * imageIn, BMPImage * imageOut, BoxFilter f, int tid, int numThreads) {
	
	//FILL IN. This is the work that thread #tid should do
	//You may find it useful to know the number of threads (numThreads) to
	//figure out what to do.
	int size;
	int sect;
	int all;
	int red;
	int blue;
	int green;
	int alpha;
	int x;
	int y;
	int a;
	int b;
	int fir;
	int las;

	size = ((imageOut -> norm_height + numThreads - 1) / numThreads);
	sect = size * tid;
	all = sect + size;

	if(all > imageOut -> norm_height){
		all = imageOut -> norm_height;
	}
	
	for(x = sect; x < all; ++x){
		for(y = 0; y < imageIn -> header.width_px; ++y){
			red = 0;
			blue = 0;
			green = 0;
			alpha = 0;
			for(a = 0; a < 3; ++a){
				for(b = 0; b < 3; ++b){
					fir = x - 1 + a;
					las = y - 1 + b;
					if(((fir) < 0) || ((fir) >= imageOut -> norm_height) || ((las) < 0) || ((las) >= imageOut -> header.width_px)){
				
						red = red + f.filter[a][b] * imageIn -> pixels[x][y].red;
						blue = blue + f.filter[a][b] * imageIn -> pixels[x][y].blue;
						green = green + f.filter[a][b] * imageIn -> pixels[x][y].green;
						alpha = alpha + f.filter[a][b] * imageIn -> pixels[x][y].alpha;
					}else{
						
						red = red + f.filter[a][b] * imageIn -> pixels[fir][las].red;
						blue = blue + f.filter[a][b] * imageIn -> pixels[fir][las].blue;
						green = green + f.filter[a][b] * imageIn -> pixels[fir][las].green;
						alpha = alpha + f.filter[a][b] * imageIn -> pixels[fir][las].alpha;
					}
				}
			}

			red = red / f.norm;
			blue = blue / f.norm;
			green = green / f.norm;
			alpha = alpha / f.norm;

			if(red > 255){
				red = 255;
			}
			if(blue > 255){
				blue = 255;
			}
			if(green > 255){
				green = 255;
			}
			if(alpha > 255){
				alpha = 255;
			}
			if(red < 0){
				red = 0;
			}
			if(blue < 0){
				blue = 0;
			}
			if(green < 0){
				green = 0;
			}
			if(alpha < 0){
				alpha = 0;
			}

			imageOut -> pixels[x][y].red = red;
			imageOut -> pixels[x][y].blue = blue;
			imageOut -> pixels[x][y].green = green;
			imageOut -> pixels[x][y].alpha = alpha;

		}
	}
	return;
}

void * applyThreadWrapper(void * args) {
	ApplyArgs * applyArgs = (ApplyArgs *) args;
	fprintf(stdout, "calling thread %d\n", applyArgs->tid);
	
	applyThread(applyArgs->in, applyArgs->out, applyArgs->filter, applyArgs->tid, applyArgs->numThreads);
	
	fprintf(stdout, "finished with thread %d\n", applyArgs->tid);
	
	return NULL;
}

void apply(BMPImage * imageIn, BMPImage * imageOut, BoxFilter f, int numThreads) {

	//FILL IN:
	
	//1. create numThreads threads, that each call applyThreadWrapper
	//2. wait for all threads to finish
	//3. clean up any memory allocation you did.
	 
	pthread_t * threads = malloc(sizeof(pthread_t) * numThreads);
	ApplyArgs * arg = malloc(sizeof(ApplyArgs) * numThreads);
	int x;

	for(x = 0; x < numThreads; ++x){	
		arg[x].in = imageIn;
		arg[x].out = imageOut;
		arg[x].tid = x;
		arg[x].numThreads = numThreads;
		arg[x].filter = f;
		pthread_create(&threads[x], NULL, applyThreadWrapper, &arg[x]);
		pthread_join(threads[x], NULL);
	}
	free(threads);
	free(arg);

	return;
	
}
