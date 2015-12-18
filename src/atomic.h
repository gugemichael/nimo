#ifndef __CLOWF_ATOMIC_H__
#define __CLOWF_ATOMIC_H__

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * Full memory barrier
 */
#define MEMORY_BARRIER() __asm__ __volatile__ ("" ::: "memory")

/*
 * Atomic operations that C can't guarantee us.  Useful for
 * resource counting etc..
 */

#define LOCK "lock ; "

/*
 * Make sure gcc doesn't try to be clever and move things around
 * on us. We need to use _exactly_ the address the user gave us,
 * not some alias that contains the same information.
 */
typedef struct { volatile int counter; } atomic_t;

#define ATOMIC_INIT(i)	{ (i) }

/**
 * atomic_read - read atomic variable
 * @param v pointer of type atomic_t
 * 
 * Atomically reads the value of v.
 */ 
#define atomic_read(v)		((v)->counter)

/**
 * atomic_set - set atomic variable
 * @param v pointer of type atomic_t
 * @param i required value
 * 
 * Atomically sets the value of v to i.
 */ 
#define atomic_set(v,i)		(((v)->counter) = (i))

/**
 * atomic_add - add integer to atomic variable
 * @param i integer value to add
 * @param v pointer of type atomic_t
 * 
 * Atomically adds i to v.
 */
static __inline__ void atomic_add(atomic_t *v, int i)
{
	__asm__ __volatile__(
		LOCK "addl %1,%0"
		:"=m" (v->counter)
		:"ir" (i), "m" (v->counter));
}

/**
 * atomic_sub - subtract the atomic variable
 * @param i integer value to subtract
 * @param v pointer of type atomic_t
 * 
 * Atomically subtracts i from v.
 */
static __inline__ void atomic_sub(atomic_t *v, int i)
{
	__asm__ __volatile__(
		LOCK "subl %1,%0"
		:"=m" (v->counter)
		:"ir" (i), "m" (v->counter));
}


/**
 * atomic_add_return - add and return
 * @param v pointer of type atomic_t
 * @param i integer value to add
 *
 * Atomically adds i to v and returns i + v
 */
static __inline__ int atomic_add_return(atomic_t *v, int i)
{
  int __i;
  /* Modern 486+ processor */
  __i = i;
  __asm__ __volatile__(
                LOCK "xaddl %0, %1"
                :"+r" (i), "+m" (v->counter)
                : : "memory");
  return i + __i;
}

static __inline__ int atomic_sub_return(atomic_t *v, int i)
{
  return atomic_add_return(-i,v);
}


/**
 * atomic_sub_and_test - subtract value from variable and test result
 * @param i integer value to subtract
 * @param v pointer of type atomic_t
 * 
 * Atomically subtracts i from v and returns
 * true if the result is zero, or false for all
 * other cases.
 */
static __inline__ int atomic_sub_and_test(atomic_t *v, int i)
{
	unsigned char c;

	__asm__ __volatile__(
		LOCK "subl %2,%0; sete %1"
		:"=m" (v->counter), "=qm" (c)
		:"ir" (i), "m" (v->counter) : "memory");
	return c;
}

/**
 * atomic_increment - increment atomic variable
 * @param v pointer of type atomic_t
 * 
 * Atomically increments v by 1.
 */ 
static __inline__ void atomic_increment(atomic_t *v)
{
	__asm__ __volatile__(
		LOCK "incl %0"
		:"=m" (v->counter)
		:"m" (v->counter));
}

/**
 * atomic_decrement - decrement atomic variable
 * @param v pointer of type atomic_t
 * 
 * Atomically decrements v by 1.
 */ 
static __inline__ void atomic_decrement(atomic_t *v)
{
	__asm__ __volatile__(
		LOCK "decl %0"
		:"=m" (v->counter)
		:"m" (v->counter));
}

/**
 * atomic_dec_and_test - decrement and test
 * @param v pointer of type atomic_t
 * 
 * Atomically decrements v by 1 and
 * returns true if the result is 0, or false for all other
 * cases.
 */ 
static __inline__ int atomic_dec_and_test(atomic_t *v)
{
	unsigned char c;

	__asm__ __volatile__(
		LOCK "decl %0; sete %1"
		:"=m" (v->counter), "=qm" (c)
		:"m" (v->counter) : "memory");
	return c != 0;
}

/**
 * atomic_inc_and_test - increment and test 
 * @param v pointer of type atomic_t
 * 
 * Atomically increments v by 1
 * and returns true if the result is zero, or false for all
 * other cases.
 */ 
static __inline__ int atomic_inc_and_test(atomic_t *v)
{
	unsigned char c;

	__asm__ __volatile__(
		LOCK "incl %0; sete %1"
		:"=m" (v->counter), "=qm" (c)
		:"m" (v->counter) : "memory");
	return c != 0;
}

/**
 * atomic_add_negative - add and test if negative
 * @param v pointer of type atomic_t
 * @param i integer value to add
 * 
 * Atomically adds i to v and returns true
 * if the result is negative, or false when
 * result is greater than or equal to zero.
 */ 
static __inline__ int atomic_add_negative(atomic_t *v, int i)
{
	unsigned char c;

	__asm__ __volatile__(
		LOCK "addl %2,%0; sets %1"
		:"=m" (v->counter), "=qm" (c)
		:"ir" (i), "m" (v->counter) : "memory");
	return c;
}

/* These are x86-specific, used by some header files */
#define atomic_clear_mask(mask, addr) \
__asm__ __volatile__(LOCK "andl %0,%1" \
: : "r" (~(mask)),"m" (*addr) : "memory")

#define atomic_set_mask(mask, addr) \
__asm__ __volatile__(LOCK "orl %0,%1" \
: : "r" (mask),"m" (*(addr)) : "memory")

#define atomic_inc_return(v)  (atomic_add_return(1,v))
#define atomic_dec_return(v)  (atomic_sub_return(1,v))


#define __ub_cas(mem, oldval, newval) \
	({ __typeof (*mem) ret;                             \
	   __asm __volatile ("lock;" "cmpxchgl %2, %1;sete %%al; movzbl %%al,%%eax"                \
					                            : "=a" (ret), "=m" (*mem)                  \
					                            : "r" (newval), "m" (*mem), "a" (oldval)\
					                            :"memory");         \
	   ret; })
#define ub_cas __ub_cas

#ifdef __cplusplus
}
#endif

#endif // ATOMIC_H
