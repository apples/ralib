#include "ralib/ralib.h"

#include <assert.h>
#include <stdio.h>

int main()
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

    assert (rav_get(vec, 50, int) == 0);

    // Resize to 10 elements
    
    rav_resize(vec, 10);
    
    // Iterators!
    
    for (ptr = vec.begin; ptr != vec.end; ++ptr)
    {
        printf ("%d\n", *ptr);
    }
    
    // Also indices!
    
    for (i=0; i < rav_size(vec); ++i)
    {
        ptr = rav_at(vec, i);
        printf ("%d\n", *ptr);
    }
    
    // Always remember to free!
    
    rav_free(vec);
    
    return 0;
}
