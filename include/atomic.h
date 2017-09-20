#ifndef ATOMIC_H_
#define ATOMIC_H_

#define LOCK_PREFIX "lock;"

#define ATOMIC_INIT(i)  { (i) }

static inline int32_t atomic_read(const atomic_t *v)
{
        return v->counter;
}

static inline void atomic_set(atomic_t *v, int i)
{
        v->counter = i;
}

static inline void atomic_add(atomic_t *v, int i)
{
        __asm__ volatile(LOCK_PREFIX "addl %1,%0"
                     : "+m" (v->counter)
                     : "ir" (i));
}

static inline void atomic_sub(atomic_t *v, int i)
{
        __asm__ volatile(LOCK_PREFIX "subl %1,%0"
                     : "+m" (v->counter)
                     : "ir" (i));
}

static inline int atomic_sub_and_test(atomic_t *v, int i)
{
        unsigned char c;

        __asm__ volatile(LOCK_PREFIX "subl %2,%0; sete %1"
                     : "+m" (v->counter), "=qm" (c)
                     : "ir" (i) : "memory");
        return c;
}

static inline void atomic_inc(atomic_t *v)
{
        __asm__ volatile(LOCK_PREFIX "incl %0"
                     : "+m" (v->counter));
}

static inline int atomic_inc_and_test(atomic_t *v)
{
        unsigned char c;

        __asm__ volatile(LOCK_PREFIX "incl %0; sete %1"
                     : "+m" (v->counter), "=qm" (c)
                     : : "memory");
        return (c != 0);
}

static inline void atomic_dec(atomic_t *v)
{
        __asm__ volatile(LOCK_PREFIX "decl %0"
                     : "+m" (v->counter));
}

static inline int atomic_dec_and_test(atomic_t *v)
{
        unsigned char c;

        __asm__ volatile(LOCK_PREFIX "decl %0; sete %1"
                     : "+m" (v->counter), "=qm" (c)
                     : : "memory");
        return (c != 0);
}

#endif
