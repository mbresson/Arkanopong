
#ifndef INC_MATHUTILS_
#define INC_MATHUTILS_

#define CLAMP(min, max, val) ((val) < (min) ? (min) : ((val) > (max) ? (max) : (val)))
#define CONVERT(val, i_min, i_max, o_min, o_max) (((val) - (i_min)) * ((o_max) - (o_min)) / (float)((i_max) - (i_min)) + (o_min))
#define ABS(val) ((val) < 0 ? -(val) : (val))
#define AVERAGE(bottom, top) ((top) - ((top) - (bottom))/2)

extern int mathutils_pow(int val, int exp);

extern int mathutils_random_in_range(int min, int max);

#endif

