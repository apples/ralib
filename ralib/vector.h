#ifndef RALIB_VECTOR_H
#define RALIB_VECTOR_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct ra_vector_t
{
    void* begin;
    void* end;
    void* final;
    size_t stride;
} *ra_vector_p, ra_vector;

inline ra_vector ra_vector_impl_new(size_t s, size_t n);
inline void ra_vector_impl_free(ra_vector_p v);
inline size_t ra_vector_impl_capacity(ra_vector_p v);
inline size_t ra_vector_impl_size(ra_vector_p v);
inline size_t ra_vector_impl_sizeb(ra_vector_p v);
inline void ra_vector_impl_reserve(ra_vector_p v, size_t n);
inline void ra_vector_impl_expand(ra_vector_p v);
inline void ra_vector_impl_resize(ra_vector_p v, size_t n);
inline void ra_vector_impl_push_back(ra_vector_p v, void* d);
inline void ra_vector_impl_pop_back(ra_vector_p v);
inline void* ra_vector_impl_at(ra_vector_p v, size_t i);
inline bool ra_vector_impl_empty(ra_vector_p v);
inline void ra_vector_impl_erase(ra_vector_p v, void* b, void* e);

inline ra_vector ra_vector_impl_new(size_t s, size_t n)
{
    ra_vector rv = { .begin = NULL, .end = NULL, .final = NULL, .stride = s };
    ra_vector_impl_resize(&rv, n);
    return rv;
}

inline void ra_vector_impl_free(ra_vector_p v)
{
    free(v->begin);
}

inline size_t ra_vector_impl_capacity(ra_vector_p v)
{
    if (!v->begin) return 0;
    return (((char*)v->final - (char*)v->begin) / v->stride);
}

inline size_t ra_vector_impl_size(ra_vector_p v)
{
    if (!v->begin) return 0;
    return (((char*)v->end - (char*)v->begin) / v->stride);
}

inline size_t ra_vector_impl_sizeb(ra_vector_p v)
{
    if (!v->begin) return 0;
    return ((char*)v->end - (char*)v->begin);
}

inline void ra_vector_impl_reserve(ra_vector_p v, size_t n)
{
    size_t prevcap = ra_vector_impl_capacity(v);
    size_t prevszb;
    size_t nb;
    void* new_block;
    
    if (n <= prevcap) return;
    
    prevszb = ra_vector_impl_sizeb(v);
    nb = n * v->stride;
    
    new_block = malloc(nb);
    memcpy(new_block, v->begin, prevszb);
    free(v->begin);
    
    v->begin = new_block;
    v->end = new_block + prevszb;
    v->final = new_block + nb;
}

inline void ra_vector_impl_expand(ra_vector_p v)
{
    size_t prevcap = ra_vector_impl_capacity(v);
    
    if (prevcap == 0) prevcap = 1;
    ra_vector_impl_reserve(v, prevcap*2);
}

inline void ra_vector_impl_resize(ra_vector_p v, size_t n)
{
    size_t prevcap = ra_vector_impl_capacity(v);
    size_t prevsz = ra_vector_impl_size(v);
    void* new_end;
    
    if (n > prevcap) ra_vector_impl_reserve(v, n);
    
    new_end = (char*)v->begin + (v->stride * n);

#ifndef RA_VECTOR_CONFIG_DONT_ZERO
    if (n > prevsz) memset(v->end, 0, ((char*)new_end - (char*)v->end));
#endif
    
    v->end = new_end;
}

inline void ra_vector_impl_push_back(ra_vector_p v, void* d)
{
    assert(d);
    
    if (v->end == v->final) ra_vector_impl_expand(v);
    
    memcpy(v->end, d, v->stride);
    v->end = (char*)v->end + v->stride;
}

inline void ra_vector_impl_pop_back(ra_vector_p v)
{
    assert(!ra_vector_impl_empty(v));
    v->end = (char*)v->end - v->stride;
}

inline void* ra_vector_impl_at(ra_vector_p v, size_t i)
{
    assert(i < ra_vector_impl_size(v));
    return ((char*)v->begin + (v->stride * i));
}

inline bool ra_vector_impl_empty(ra_vector_p v)
{
    return (v->begin == v->end);
}

inline void ra_vector_impl_erase(ra_vector_p v, void* b, void* e)
{
    assert((char*)v->begin < (char*)b);
    assert((char*)v->begin < (char*)e);
    assert((char*)b < (char*)e);
    assert(((char*)b - (char*)v->begin) < ra_vector_impl_sizeb(v));
    assert(((char*)e - (char*)v->begin) < ra_vector_impl_sizeb(v));
    
    size_t e_to_end = ((char*)v->end - (char*)e);
    
    memmove(b, e, e_to_end);
    
    v->end = b + e_to_end;
}

inline void ra_vector_impl_quick_erase(ra_vector_p v, void* b, void* e)
{
    assert((char*)v->begin < (char*)b);
    assert((char*)v->begin < (char*)e);
    assert((char*)b < (char*)e);
    assert(((char*)b - (char*)v->begin) < ra_vector_impl_sizeb(v));
    assert(((char*)e - (char*)v->begin) < ra_vector_impl_sizeb(v));
    
    size_t b_to_e = ((char*)e - (char*)b);
    size_t e_to_end = ((char*)v->end - (char*)e);
    void* new_end;
    
    if (b_to_e > e_to_end)
    {
        memmove(b, e, e_to_end);
        v->end = b + e_to_end;
        return;
    }
    
    new_end = ((char*)v->end - b_to_e);
    
    memcpy(b, new_end, b_to_e);
    
    v->end = new_end;
}

void* ra_vector_impl_make_iterator(ra_vector_p v, size_t i)
{
    assert(i <= ra_vector_impl_size(v));
    return (v->begin + (i * v->stride));
}

#define rav_new(T, N)           (ra_vector_impl_new(sizeof(T), (N)))
#define rav_free(V)             (ra_vector_impl_free(&(V)))
#define rav_capacity(V)         (ra_vector_impl_capacity(&(V)))
#define rav_size(V)             (ra_vector_impl_size(&(V)))
#define rav_reserve(V, N)       (ra_vector_impl_reserve(&(V), (N)))
#define rav_resize(V, N)        (ra_vector_impl_resize(&(V), (N)))
#define rav_push(V, D)          (ra_vector_impl_push_back(&(V), &(D)))
#define rav_pop(V)              (ra_vector_impl_pop_back(&(V)))
#define rav_at(T, V, N)         ((*(T*)ra_vector_impl_at(&(V), (N))))
#define rav_empty(V)            (ra_vector_impl_empty(&(V)))
#define rav_erase(V, B, E)      (ra_vector_impl_erase(&(V), (B), (E)))
#define rav_qerase(V, B, E)     (ra_vector_impl_quick_erase(&(V), (B), (E)))
#define rav_iter(V, I)          (ra_vector_impl_make_iterator(&(V), (I)))

#endif // RALIB_VECTOR_H
