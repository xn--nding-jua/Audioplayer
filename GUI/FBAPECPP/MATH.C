#include "math.h"
             
float lin2log(float value, float minp, float maxp, float minv, float maxv)
{
	float scale;
	scale = ((float)log(maxv) - (float)log(minv)) / (maxp - minp);
	
	return (float)exp((float)log(minv) + scale * (value - minp));
}

float log2lin(float value, float minp, float maxp, float minv, float maxv)
{                                                                         
	float scale;
	scale = ((float)log(maxv) - (float)log(minv)) / (maxp - minp);
	
	return (((float)log(value) - (float)log(minv)) / scale) + minp;
}                                        

float round(float value)
{
	return value < 0.0 ? (float)ceil(value - 0.5f) : (float)floor(value + 0.5f);
}