
// check the power of 2
#define nimo_is_power(x) ((x)&((x)+1))
#define nimo_abs(x) (x^(x>>31))-(x>>31)
#define nimo_cmp(x,y) ((x)>(y))-((x)-(y))
#define nimo_max(x,y) ((y)&((x)-(y))>>31|(x)&~(((x)-(y))>>31))
