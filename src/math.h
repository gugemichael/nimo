
// check the power of 2
#define clowf_is_power(x) ((x)&((x)+1))
#define clowf_abs(x) (x^(x>>31))-(x>>31)
#define clowf_cmp(x,y) ((x)>(y))-((x)-(y))
#define clowf_max(x,y) ((y)&((x)-(y))>>31|(x)&~(((x)-(y))>>31))
