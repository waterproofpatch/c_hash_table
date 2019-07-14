#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"

int key_comparator(void* key1, void* key2)
{
    return 1;
}

unsigned int hash_function(void* key)
{
    return 1;
}

int main(int argc, char** argv)
{
    hashtable_t* table =
        hashtable_init(5, hash_function, key_comparator, malloc, free);
    printf("done\n");
    return 0;
}
