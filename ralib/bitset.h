#ifndef RALIB_BITSET_H
#define RALIB_BITSET_H

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

typedef unsigned ra_bitset_word_type;
typedef bool ra_bitset_bit_type;

static const size_t ra_bitset_word_bytes = sizeof(ra_bitset_word_type);
static const size_t ra_bitset_word_bits = sizeof(ra_bitset_word_type)*CHAR_BIT;

typedef struct ra_bitset_t
{
    ra_bitset_word_type* data;
    size_t size;
} *ra_bitset_p, ra_bitset;

static ra_bitset ra_bitset_impl_new(size_t N);
static void ra_bitset_impl_free(ra_bitset_p bs);
static ra_bitset_bit_type ra_bitset_impl_get(ra_bitset_p bs, size_t n);
static void ra_bitset_impl_set(ra_bitset_p bs, size_t n, ra_bitset_word_type v);
static ra_bitset_bit_type ra_bitset_impl_flip(ra_bitset_p bs, size_t n);
static size_t ra_bitset_impl_word_count(ra_bitset_p bs);
static ra_bitset_p ra_bitset_impl_or_assign(ra_bitset_p bs1, ra_bitset_p bs2);
static ra_bitset_p ra_bitset_impl_and_assign(ra_bitset_p bs1, ra_bitset_p bs2);
static ra_bitset_p ra_bitset_impl_xor_assign(ra_bitset_p bs1, ra_bitset_p bs2);
static void ra_bitset_impl_stringify(ra_bitset_p bs, char* str);

static ra_bitset ra_bitset_impl_new(size_t N)
{
    assert(N > 0U);
    
    ra_bitset rv = {
        .data = calloc(((N-1U)/ra_bitset_word_bits+1U), ra_bitset_word_bytes),
        .size = N
    };
    
    return rv;
}

static void ra_bitset_impl_free(ra_bitset_p bs)
{
    free(bs->data);
}

static ra_bitset_bit_type ra_bitset_impl_get(ra_bitset_p bs, size_t n)
{
    assert(n < bs->size);
    
    size_t i = n/ra_bitset_word_bits;
    size_t r = n%ra_bitset_word_bits;
    
    return (((bs->data[i])>>r)&1U);
}

static void ra_bitset_impl_set(ra_bitset_p bs, size_t n, ra_bitset_word_type v)
{
    assert(n < bs->size);
    
    size_t i = n/ra_bitset_word_bits;
    size_t r = n%ra_bitset_word_bits;
    
    ra_bitset_word_type mask = (((ra_bitset_word_type)1U)<<r);
    
    bs->data[i] = (bs->data[i] & ~mask) | (-v & mask);
}

static ra_bitset_bit_type ra_bitset_impl_flip(ra_bitset_p bs, size_t n)
{
    assert(n < bs->size);
    
    size_t i = n/ra_bitset_word_bits;
    size_t r = n%ra_bitset_word_bits;
    
    ra_bitset_word_type mask = (((ra_bitset_word_type)1U)<<r);
    
    bs->data[i] ^= mask;
    
    return (((bs->data[i])>>r)&1U);
}

static size_t ra_bitset_impl_word_count(ra_bitset_p bs)
{
    return ((bs->size-1U)/ra_bitset_word_bits+1U);
}

static ra_bitset_p ra_bitset_impl_or_assign(ra_bitset_p bs1, ra_bitset_p bs2)
{
    assert(bs1->size == bs2->size);
    
    ra_bitset_word_type* iter1 = bs1->data;
    ra_bitset_word_type* end1  = iter1 + ra_bitset_impl_word_count(bs1);
    ra_bitset_word_type* iter2 = bs2->data;
    
    while (iter1 != end1) *(iter1++) |= *(iter2++);
    
    return bs1;
}

static ra_bitset_p ra_bitset_impl_and_assign(ra_bitset_p bs1, ra_bitset_p bs2)
{
    assert(bs1->size == bs2->size);
    
    ra_bitset_word_type* iter1 = bs1->data;
    ra_bitset_word_type* end1  = iter1 + ra_bitset_impl_word_count(bs1);
    ra_bitset_word_type* iter2 = bs2->data;
    
    while (iter1 != end1) *(iter1++) &= *(iter2++);
    
    return bs1;
}

static ra_bitset_p ra_bitset_impl_xor_assign(ra_bitset_p bs1, ra_bitset_p bs2)
{
    assert(bs1->size == bs2->size);
    
    ra_bitset_word_type* iter1 = bs1->data;
    ra_bitset_word_type* end1  = iter1 + ra_bitset_impl_word_count(bs1);
    ra_bitset_word_type* iter2 = bs2->data;
    
    while (iter1 != end1) *(iter1++) ^= *(iter2++);
    
    return bs1;
}

static void ra_bitset_impl_stringify(ra_bitset_p bs, char* str)
{
    size_t i;
    for (i=0; i<bs->size; ++i)
    {
        str[i] = '0' + ra_bitset_impl_get(bs, i);
    }
    str[i] = '\0';
}

static size_t ra_bitset_impl_count_bits(ra_bitset_p bs)
{
    size_t rv = 0;
    size_t i;
    size_t words = ra_bitset_impl_word_count(bs);
    ra_bitset_word_type w;
    
    for (i=0; i<words; ++i)
    {
        for (w = bs->data[i]; w != 0; ++rv)
        {
            w &= w-1U;
        }
    }
    
    return rv;
}

#define rabs_new(N)         (ra_bitset_impl_new((N)))
#define rabs_free(B)        (ra_bitset_impl_free(&(B)))
#define rabs_get(B, N)      (ra_bitset_impl_get(&(B), (N)))
#define rabs_set(B, N, V)   (ra_bitset_impl_set(&(B), (N), (V)))
#define rabs_flip(B, N)     (ra_bitset_impl_flip(&(B), (N)))
#define rabs_or_eq(B1, B2)  (*(ra_bitset_impl_or_assign(&(B1), &(B2))))
#define rabs_and_eq(B1, B2) (*(ra_bitset_impl_and_assign(&(B1), &(B2))))
#define rabs_xor_eq(B1, B2) (*(ra_bitset_impl_xor_assign(&(B1), &(B2))))
#define rabs_str(B, S)      (ra_bitset_impl_stringify(&(B), (S)))
#define rabs_bitcount(B)    (ra_bitset_impl_count_bits(&(B)))

#endif // RALIB_BITSET_H
