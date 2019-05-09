#ifndef gameFuncs_h
#define gameFuncs_h

float func_sqrt(float number) {
    float x = number * 0.5;
    float y  = number;
    int i  = *(int *) &y;
    i  = 0x5f3759df - (i >> 1);
    y  = * (float *) &i;
    y  = y * (1.5 - (x * y * y));
    y  = y * (1.5 - (x * y * y));

    return number * y;
}

float func_rsqrt(float number){
	long i;
	float x2, y;
	const float threehalfs = 1.5F;
	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                       // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck? 
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration

	return y;
}

#endif