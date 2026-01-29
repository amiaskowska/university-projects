#include <stdlib.h>
#include "ll.h"
#include "ma.h"


// sprawdza, czy element {a,num} znajduje się na liście
int is_on_list(moore_t *a, int64_t num, ll *end){
    ll *cur = end;
    
    while(cur != NULL){
        ll *next = cur->left;
        if(cur->a == a && cur->num == num)
            return 1;
        cur = next;
    }
    return 0;
}

// dodaje element {a, num} na koniec listy 
ll * list_add(moore_t *a, int64_t num, ll *end){
    ll *new = malloc(1 * sizeof(ll));
    if(new == NULL)
        return NULL;
    
    end->right = new;
    new->left = end;
    new->right = NULL;
    new->a = a;
    new->num = num;

    return new;
}

// usuwa element {a, num} z listy automatu out, jeśli istnieje
// oraz zwraca wskaźnik na koniec listy 
ll * list_delete(moore_t *a, int64_t num, ll *end){
    ll *prev = NULL;
    ll *cur = end;
    
    while(cur != NULL){
        ll *next = cur->left;
        if(cur->a == a && cur->num == num){
            if(next != NULL)
                next->right = cur->right;

            if(prev != NULL)
                prev->left = next;
            else{
                free(cur);
                return next;
            }

            free(cur);
            return end;
        }
        prev = cur;
        cur = next;
    }

    return end;
}

// zwalnia pamięć z całej listy
void clear_list(ll *end){
    if(end == NULL)
        return;

    ll *cur = end;
    while(cur != NULL){
        ll *next = cur->left;
        free(cur);
        cur = next;
    }
}