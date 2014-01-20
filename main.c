#include "ralib/vector.h"
#include "ralib/bitset.h"

#include <assert.h>
#include <stdio.h>

static void vector_test(void)
{
    ra_vector vec = rav_new(int, 0);
    int i;
    int* ptr;
    
    // Push numbers 1 to 20
    
    for (i=1; i<=20; ++i)
    {
        rav_push(vec, i); // Caveat: i must be an lvalue
    }
    
    assert (rav_size(vec) == 20);

    // Pop last 5 numbers
    
    for (i=0; i<5; ++i)
    {
        rav_pop(vec);
    }
    
    assert (rav_size(vec) == 15);

    // Reserve space for 100 elements
    
    rav_reserve(vec, 100);
    
    assert (rav_size     (vec) ==  15);
    assert (rav_capacity (vec) == 100);

    // Resize to 70 elements
    
    rav_resize(vec, 70);
    
    assert (rav_size     (vec) ==  70);
    assert (rav_capacity (vec) == 100);

    // Elements are zero-initialized

    assert (rav_at(int, vec, 50) == 0);

    // Resize to 10 elements
    
    rav_resize(vec, 10);
    
    // Iterators!
    
    printf("Size: %d\n", (int)rav_size(vec));
    for (ptr = vec.begin; ptr != vec.end; ++ptr)
    {
        printf ("%d\n", *ptr);
    }
    printf("\n");
    
    // Erase
    
    rav_qerase(vec, rav_iter(vec, 2), rav_iter(vec, 5));
    
    // Also indices!
    
    printf("Size: %d\n", (int)rav_size(vec));
    for (i=0; i < rav_size(vec); ++i)
    {
        printf ("%d\n", rav_at(int, vec, i));
    }
    printf("\n");
    
    // Insertion
    
    int arr[] = {17, 42, 69};
    
    rav_insert(vec, rav_begin(int, vec), &arr[0], &arr[3]);
    
    printf("Size: %d\n", (int)rav_size(vec));
    for (i=0; i < rav_size(vec); ++i)
    {
        printf ("%d\n", rav_at(int, vec, i));
    }
    printf("\n");
    
    // Always remember to free!
    
    rav_free(vec);
}

static void bitset_test(void)
{
    ra_bitset bs = rabs_new(10);
    ra_bitset bs2 = rabs_new(10);
    
    char str[11];
    
    rabs_str(bs, str);
    puts(str);
    
    rabs_set(bs, 2, true);
    rabs_str(bs, str);
    puts(str);
    
    rabs_flip(bs, 1);
    rabs_str(bs, str);
    puts(str);
    
    rabs_flip(bs, 2);
    rabs_str(bs, str);
    puts(str);
    
    puts("");
    
    rabs_str(bs2, str);
    puts(str);
    rabs_set(bs2, 5, true);
    rabs_str(bs2, str);
    puts(str);
    
    puts("");
    
    rabs_str(rabs_or_eq(bs2, bs), str);
    puts(str);
    
    puts("");
    
    printf("Bits in bs:  %d\n", (int)rabs_bitcount(bs));
    printf("Bits in bs2: %d\n", (int)rabs_bitcount(bs2));
    
    rabs_free(bs2);
    rabs_free(bs);
}

int main()
{
    bitset_test();
    
    return 0;
}
