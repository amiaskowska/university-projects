#include <stdio.h>
#include "ll.h"
#include "ma.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>


typedef struct moore{
    // tablice przechowujące sygnały wejścia/stanu/wyjścia
    uint64_t *in;
    uint64_t *st;
    uint64_t *out;

    // sygnały wejścia/stanu, pozwala na odzyskanie stanu automatu
    // w przypadku błędu
    uint64_t *in_tmp;
    uint64_t *st_tmp;

    // wielkości wejścia/stanu/wyjścia jako liczba bitów
    size_t n;
    size_t s;
    size_t m;

    // wielkości wejścia/stanu/wyjścia jako liczba używanych ULL
    size_t cn;
    size_t cs;
    size_t cm;

    transition_function_t t;
    output_function_t y;

    // wskażnik na automat oraz numer bitu, do którego podłączony
    // jest bit wejścia
    moore_t **edge_in;
    size_t *num_in;   
    
    // automat oraz numer bitu, do którego był podłaczony bit wejścia
    // pozwala na odzyskanie stanu automatu w przypadku błędu 
    moore_t **edge_in_tmp;
    size_t *num_in_tmp;


    // wskaźnik na koniec listy trzymającej 
    // wejścia, do którego podłączony jest bit wyjścia
    ll **end;
    
} moore_t;



// zwraca sufit z dzielenia x przez 64
uint64_t ceil64(uint64_t x){
    uint64_t r = x & 63;
    if(r > 0)
         return (x >> 6) + 1ULL;
    else 
        return (x >> 6);
}

// funkcja identycznościowa 
void id(uint64_t *output, uint64_t const *state, size_t m, size_t s){
    if(s == 0 || output == NULL || state == NULL)
        return;
    memcpy(output, state, ceil64(m) * sizeof(uint64_t));
}


moore_t * ma_create_full(size_t n, size_t m, size_t s, transition_function_t t, 
output_function_t y, uint64_t const *q){

    if(m == 0 || s == 0){
        errno = EINVAL;
        return NULL;
    }
    else if(t == NULL || y == NULL || q == NULL){
        errno = EINVAL;
        return NULL;
    }
    
    moore_t* am = calloc(1, sizeof(moore_t));
    if(am == NULL){
        errno = ENOMEM;
        return NULL;
    }

    am->n = n;
    am->m = m;
    am->s = s;
    am->cn = ceil64(n);
    am->cm = ceil64(m);
    am->cs = ceil64(s);

    am->t = t;
    am->y = y;

    if(n > 0){
        am->in = calloc(am->cn, sizeof(uint64_t));
        if(am->in == NULL){
            ma_delete(am);
            errno = ENOMEM;
            return NULL;
        }

        am->in_tmp = calloc(am->cn, sizeof(uint64_t));
        if(am->in_tmp == NULL){
            ma_delete(am);
            errno = ENOMEM;
            return NULL;
        }

        am->edge_in_tmp = calloc(am->n, sizeof(moore_t*));
        am->num_in_tmp = calloc(am->n, sizeof(size_t));
        if(am->edge_in_tmp == NULL || am->num_in_tmp == NULL){
            ma_delete(am);
            errno = ENOMEM;
            return NULL;
        }

        am->edge_in = calloc(am->n, sizeof(moore_t*));
        am->num_in = calloc(am->n, sizeof(size_t));
        if(am->edge_in == NULL || am->num_in == NULL){
            ma_delete(am);
            errno = ENOMEM;
            return NULL;
        }
    }
    else{
        am->in = NULL;
        am->in_tmp = NULL;
        am->edge_in = NULL;
        am->edge_in_tmp = NULL;
    }

    am->st = malloc(am->cs * sizeof(uint64_t));
    if(am->st == NULL){
        ma_delete(am);
        errno = ENOMEM;
        return NULL;
    }
    memcpy(am->st, q, am->cs * sizeof(uint64_t));

    am->st_tmp = calloc(am->cs, sizeof(uint64_t));
    if(am->st_tmp == NULL) {
        ma_delete(am);
        errno = ENOMEM;
        return NULL;
    }

    am->out = calloc(am->cm, sizeof(uint64_t));
    if(am->out == NULL){
        ma_delete(am);
        errno = ENOMEM;
        return NULL;
    }
    
    am->end = calloc(m, sizeof(ll*));
    if(am->end == NULL){
        ma_delete(am);
        errno = ENOMEM;
        return NULL;
    }
    for(size_t i = 0; i < am->m; ++i){
        am->end[i] = malloc(sizeof(ll));
        if(am->end[i] == NULL){
            ma_delete(am);
            errno = ENOMEM;
            return NULL;
        }
        
        am->end[i]->left = NULL;
        am->end[i]->right = NULL;

        // każdy bit wyjściowy trzyma na początku swojej listy
        // dodatkowy wierzchołek, by lista była niepusta
        am->end[i]->a = NULL;
        am->end[i]->num = i;
    }

    (am->y)(am->out, am->st, am->m, am->s);
    return am;
}

moore_t * ma_create_simple(size_t n, size_t s, transition_function_t t){
    
    if(s == 0){
        errno = EINVAL;
        return NULL;
    }
    else if(t == NULL){
        errno = EINVAL;
        return NULL;
    }

    moore_t* am = calloc(1, sizeof(moore_t));
    if(am == NULL){
        errno = ENOMEM;
        return NULL;
    }

    am->n = n;
    am->m = s;
    am->s = s;
    am->cn = ceil64(n);
    am->cm = ceil64(s);
    am-> cs = ceil64(s);

    am-> t = t;
    am->y = id;

    if(n > 0){
        am->in = calloc(am->cn, sizeof(uint64_t));
        if(am->in == NULL){
            ma_delete(am);
            errno = ENOMEM;
            return NULL;
        }

        am->in_tmp = calloc(am->cn, sizeof(uint64_t));
        if(am->in_tmp == NULL){
            ma_delete(am);
            errno = ENOMEM;
            return NULL;
        }

        am->edge_in_tmp = calloc(am->n, sizeof(moore_t*));
        am->num_in_tmp = calloc(am->n, sizeof(size_t));
        if(am->edge_in_tmp == NULL || am->num_in_tmp == NULL){
            ma_delete(am);
            errno = ENOMEM;
            return NULL;
        }

        am->edge_in = calloc(am->n, sizeof(moore_t*));
        am->num_in = calloc(am->n, sizeof(size_t));
        if(am->edge_in == NULL || am->num_in == NULL){
            ma_delete(am);
            errno = ENOMEM;
            return NULL;
        }
    }
    else{
        am->in = NULL;
        am->in_tmp = NULL;
        am->edge_in = NULL;
        am->edge_in_tmp = NULL;
    }    

    am->st = calloc(am->cs, sizeof(uint64_t));
    if(am->st == NULL){
        ma_delete(am);
        errno = ENOMEM;
        return NULL;
    }

    am->st_tmp = calloc(am->cs, sizeof(uint64_t));
    if(am->st_tmp == NULL) {
        ma_delete(am);
        errno = ENOMEM;
        return NULL;
    }

    am->out = calloc(am->cm, sizeof(uint64_t));
    if(am->out == NULL){
        ma_delete(am);
        errno = ENOMEM;
        return NULL;
    }

    am->end = calloc(am->m, sizeof(ll*));
    if(am->end == NULL){
        ma_delete(am);
        errno = ENOMEM;
        return NULL;
    }
    for(size_t i = 0; i < am->m; ++i){
        am->end[i] = malloc(sizeof(ll));
        if(am->end[i] == NULL){
            ma_delete(am);
            errno = ENOMEM;
            return NULL;
        }
        
        am->end[i]->left = NULL;
        am->end[i]->right = NULL;

        // każdy bit wyjściowy trzyma na początku swojej listy
        // samego siebie, żeby lista zawsze była niepusta
        am->end[i]->a = NULL;
        am->end[i]->num = i;
    }

    (am->y)(am->out, am->st, am->m, am->s);
    return am;
}

void ma_delete(moore_t *a){

    if(a == NULL) 
        return;

    free(a->in);
    free(a->st);
    free(a->out);
    free(a->in_tmp);
    free(a->st_tmp);
    free(a->edge_in);
    free(a->num_in);
    free(a->edge_in_tmp);
    free(a->num_in_tmp);
    if(a->end != NULL){
        for(size_t j = 0; j < a->m; ++j)
            if(a->end[j]) 
                clear_list(a->end[j]);
    }
    free(a->end);
    free(a);
}

int ma_connect(moore_t *a_in, size_t in, moore_t *a_out, size_t out, size_t num){
    if(num == 0){
        errno = EINVAL;
        return -1;
    }
    else if(a_in == NULL || a_out == NULL){
        errno = EINVAL;
        return -1;
    }
    else if((in + num) > a_in->n || (out + num) > a_out->m){
        errno = EINVAL;
        return -1;
    }

    size_t start_in = in;
    size_t start_out = out;
    // dodanie połączeń
    for(size_t i = 0; i < num; ++i){
        a_in->edge_in_tmp[in] = a_in->edge_in[in];
        a_in->num_in_tmp[in] = a_in->num_in[in];

        // jeśli wejście było już podłączone, to zostaje odłączone
        if(a_in->edge_in[in] != NULL){
            moore_t *a = a_in->edge_in[in];
            size_t bit_num = a_in->num_in[in];
            a->end[bit_num] = list_delete(a_in, in, a->end[bit_num]);
        }

        // dodaje nowe połączenie od strony wejścia
        a_in->edge_in[in] = a_out;
        a_in->num_in[in] = out;

        // dodaje nowe połączenie od strony wyjścia
        ll *new = list_add(a_in, in, a_out->end[out]);
   

        // błąd alokacji pamięci, przywracany jest stan sprzed wywołania
        if(new == NULL){
            while(in >= start_in){
                a_in->edge_in[in] = a_in->edge_in_tmp[in];
                a_in->num_in[in] = a_in->num_in_tmp[in];
                
                if(a_out->end != NULL)
                     a_out->end[out] = list_delete(a_in, in, a_out->end[out]);
                if(a_in->edge_in[in] != NULL){
                    moore_t *a = a_in->edge_in[in];
                    size_t bit_num = a_in->num_in[in];
                    if(is_on_list(a_in, in, a->end[bit_num]) == 0)
                        a->end[bit_num] = list_add(a_in, in, a->end[bit_num]);
                }
                --in;
                --out;
            }
            errno = ENOMEM;
            return -1;
        }
        a_out->end[out] = new;

        ++in;
        ++out;
    }

    in = start_in;
    out = start_out;

    // wyliczenie nowych wartości dla sygnałów wejściowych
    for(size_t i = 1; i <= num; ++i){
        a_in->in[in] = a_out->out[out];
        ++in;
        ++out;
    }

    return 0;
}

int ma_disconnect(moore_t *a_in, size_t in, size_t num){
    if(num == 0){
        errno = EINVAL;
        return -1;
    }
    else if(a_in == NULL){
        errno = EINVAL;
        return -1;
    }
    else if(in + num > a_in->n){
        errno = EINVAL;
        return -1;
    }

    for(size_t i = 1; i <= num; ++i){
        if(a_in->edge_in[in] == NULL){
            ++in;
            continue;
        }

        moore_t *a = a_in->edge_in[in];
        size_t bit_num = a_in->num_in[in];
        a->end[bit_num] = list_delete(a_in, in, a->end[bit_num]);

        a_in->edge_in[in] = NULL;
        a_in->num_in[in] = 0;
        ++in;
    }
    return 0;
}

int ma_set_input(moore_t *a, uint64_t const *input){
    if(a == NULL || input == NULL){
        errno = EINVAL;
        return -1;
    }

    size_t r = 0;
    size_t num = 0;
    uint64_t sum = 0;
    for(size_t i = 0; i < a->n; ++i){
        if(r == 64){
            a->in[num] = sum;
            ++num;
            sum = 0;
            r = 0;
        }
        
        // jeśli wejście jest podłączone, wartość bitu się nie zmienia,
        // a jeżeli nie jest podłączone, to pobierana jest wartość z input
        // dla odpowiedniego bitu  
        if(a->edge_in[i] != NULL)
            sum += ((a->in[num]) & (1ULL << r));
        else 
            sum += (input[num] & (1ULL << r));
        ++r;
    }
    if(r > 0) 
        a->in[num] = sum;
    return 0;
}

int ma_set_state(moore_t *a, uint64_t const *state){
    if(a == NULL || state == NULL){
        errno = EINVAL;
        return -1;
    }

    memcpy(a->st, state, a->cs * sizeof(uint64_t));
    (*a->y)(a->out, a->st, a->m, a->s);
    return 0;
}

uint64_t const * ma_get_output(moore_t const *a){
    if(a == NULL){
        errno = EINVAL;
        return NULL;
    }
    
    (*a->y)(a->out, a->st, a->m, a->s);
    return a->out;
}

int ma_step(moore_t *at[], size_t num){
    if(at == NULL){
        errno = EINVAL;
        return -1;
    }
    else if(num == 0){
        errno = EINVAL;
        return -1;
    }
    for(size_t i = 0; i < num; ++i){
        if(at[i] == NULL || at[i]->st == NULL){
            errno = EINVAL;
            return -1;
        }
    }

    // uaktualnia wartości sygnałów wejściowych 
    for(size_t i = 0; i < num; ++i){
        moore_t *a = at[i];
        
        size_t r = 0;
        size_t count = 0;

        if(a->in != NULL)
            memcpy(a->in_tmp, a->in, a->cn * sizeof(uint64_t)); 
        memcpy(a->st_tmp, a->st, a->cs * sizeof(uint64_t));

        for(size_t j = 0; j < a->n; ++j){
            if(r == 64){
                r = 0;
                ++count;
            }

            moore_t *out = a->edge_in[j];
            // sygnał wejściowy nie jest podłączony, więc wartość jest aktualna
            if(out == NULL){
                ++r;
                continue;
            }
            
            size_t seg_num = a->num_in[j] >> 6ULL;
            size_t bit_num = a->num_in[j] & 63ULL; 

            // jeżeli wartość sygnału nie zgadza się z podłączonym wyjściem, 
            // to jest zmieniana na przeciwną
            if(((out->out[seg_num]&(1ULL << bit_num))>>bit_num) 
                != ((a->in[count]&(1ULL << r))>>r))
                a->in[count] ^= (1ULL << r);
            ++r;
        }
    }

    // wywołuje funkcję przejść na i-tym automacie, nie wpływa na inne automaty
    for(size_t i = 0; i < num; ++i){
        moore_t *a = at[i];
        uint64_t *next_state = malloc(a->cs * sizeof(uint64_t));

        // błąd przy alokowaniu pamięci, przywraca automaty do stanu pierwotnego
        if(next_state == NULL){
            for(size_t j = 0; j < num; ++j){
                if(at[j]->in != NULL)
                     memcpy(at[j]->in, at[j]->in_tmp, at[j]->cn * sizeof(uint64_t));
                memcpy(at[j]->st, at[j]->st_tmp, at[j]->cs * sizeof(uint64_t));
            }
            errno = ENOMEM;
            return -1;
        }

        (a->t)(next_state, a->in, a->st, a->n, a->s);

        free(a->st);
        a->st = next_state;
    }

    // wywołuje funkcję wyjść na i-tym automacie
    for(size_t i = 0; i < num; ++i){
        (at[i]->y)(at[i]->out, at[i]->st, at[i]->m, at[i]->s);
    }

    return 0;
}
