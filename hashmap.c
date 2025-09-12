#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}


void insertMap(HashMap * map, char * key, void * value) {
    long position = hash(key, map->capacity);

    while (map->buckets[position]!=NULL && map->buckets[position]->key != NULL) {
        if (is_equal(map->buckets[position]->key, key)) {
            map->current = position;
            return;
        }
        position = (position + 1) % map->capacity;
    }

    map->buckets[position]=createPair(key, value);
    map->current = position;
    map->size++;
}

void enlarge(HashMap * map) {

    enlarge_called = 1; // no borrar (testing purposes)
    long old_capacity = map->capacity;
    Pair ** old_buckets = map->buckets;

    // Duplica la capacidad.
    map->capacity = 2 * old_capacity;
    // Crea una nueva tabla vacía.
    map->buckets = (Pair **) calloc(map->capacity, sizeof(Pair *));
    map->size = 0;
    map->current = -1;

    // Recorre la antigua tabla e inserta los elementos válidos.
    for (long i = 0; i < old_capacity; i++) {
        if (old_buckets[i] != NULL && old_buckets[i]->key != NULL) {
            insertMap(map, old_buckets[i]->key, old_buckets[i]->value);
            free(old_buckets[i]); // liberar el Pair usado en la tabla vieja
        }
    }

    // Libera la tabla antigua (el arreglo de punteros).
    free(old_buckets);
}

//Crea un HashMap con una capacidad inicial dada por el parametro y retorna Map
HashMap * createMap(long capacity) {
    HashMap * map = (HashMap *)malloc(sizeof(HashMap));
    map->buckets=(Pair **)calloc(capacity, sizeof(Pair*));
    map->size=0;
    map->capacity=capacity;
    map->current=-1;
    return map;
    
}

void eraseMap(HashMap * map,  char * key) { 
    if(map==NULL || key==NULL) return;

    Pair * p=searchMap(map,key);
    if (p==NULL)return;

    p->key=NULL;
    map->size--;
}

Pair * searchMap(HashMap * map,  char * key) {
    if (map==NULL || map->size==0) return NULL;

    long cap=map->capacity;
    long pos=hash(key,cap);

    while(map->buckets[pos]!=NULL)
    {
        if(map->buckets[pos]->key!=NULL && is_equal(map->buckets[pos]->key,key))
        {
            map->current=pos;
            return map->buckets[pos];
        }
        pos=(pos+1)%cap;
    }
    
    return NULL;
}

Pair * firstMap(HashMap * map) {
    if (map == NULL) return NULL;
    for (long i = 0; i < map->capacity; i++) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
            map->current = i;
            return map->buckets[i];
        }
    }
    return NULL;
}

Pair * nextMap(HashMap * map) {
    if (map == NULL) return NULL;
    for (long i = map->current + 1; i < map->capacity; i++) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
            map->current = i;
            return map->buckets[i];
        }
    }
    return NULL;
}
