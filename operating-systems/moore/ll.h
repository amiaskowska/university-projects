#ifndef LL_H
#define LL_H

#include <stdlib.h>
#include "ma.h"

typedef struct LL {
    //wskaźniki na lewe i prawe wierzchołki
    struct LL *left;
    struct LL *right;

    //wartości w wierzchołku
    moore_t *a;
    int64_t num;    
} ll;

int is_on_list(moore_t *a, int64_t num, ll *end);
ll * list_add(moore_t *a, int64_t num, ll *end);
ll * list_delete(moore_t *a, int64_t num, ll *end);
void clear_list(ll *end);

#endif