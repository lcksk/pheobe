#include "hashmap.h"
#include <stdio.h>
#include <stdlib.h>

/* ---------------------------------------------
 * author: fandel
 * release date: 2008.4.14
 * --------------------------------------------*/

#define TABLE_STARTSIZE 1023

#define ACTIVE 1

typedef struct {
  void* data;
  int flags;
  long key;
} hEntry;

typedef struct {
  hEntry* table;
  long size, count;
} hashmap_t;

/* PRIVATE IMPLEMENTATION */

static unsigned long isPrime(unsigned long val)
{
  int i, p, exp, a;

  for (i = 9; i--;)
  {
    a = (rand() % (val-4)) + 2;
    p = 1;
    exp = val-1;
    while (exp)
    {
      if (exp & 1)
        p = (p*a)%val;

      a = (a*a)%val;
      exp >>= 1;
    }

    if (p != 1)
      return 0;
  }

  return 1;
}

static int findPrimeGreaterThan(int val)
{
  if (val & 1)
    val+=2;
  else
    val++;

  while (!isPrime(val))
    val+=2;

  return val;
}

static void rehash(hashmap_t* hm)
{
  long size = hm->size;
  hEntry* table = hm->table;

  hm->size = findPrimeGreaterThan(size<<1);
  hm->table = (hEntry*)calloc(sizeof(hEntry), hm->size);
  hm->count = 0;

  while(--size >= 0)
    if (table[size].flags == ACTIVE)
      hashmap_insert(hm, table[size].data, table[size].key);

  free(table);
}

/* PUBLIC IMPLEMENTATION */

hashmap hashmap_new(int startsize)
{
  hashmap_t* hm = (hashmap_t*)malloc(sizeof(hashmap_t));

  if (!startsize)
    startsize = TABLE_STARTSIZE;
  else
    startsize = findPrimeGreaterThan(startsize-2);

  hm->table = (hEntry*)calloc(sizeof(hEntry), startsize);
  hm->size = startsize;
  hm->count = 0;

  return hm;
}

void hashmap_insert(hashmap hash, void* data, unsigned long key)
{
  hashmap_t* hm = (hashmap_t*)hash;
  long index, i, step;

  if (hm->size <= hm->count)
    rehash(hm);

  do
  {
    index = key % hm->size;
    step = (key % (hm->size-2)) + 1;

    for (i = 0; i < hm->size; i++)
    {
      if (!(hm->table[index].flags & ACTIVE))
      {
        hm->table[index].flags |= ACTIVE;
        hm->table[index].data = data;
        hm->table[index].key = key;
        hm->count++;
        return;
      }
      else
      {
        if (hm->table[index].key == key)
        {
          hm->table[index].data = data;
          return;
        }
      }

      index = (index + step) % hm->size;
    }

    /* it should not be possible that we EVER come this far, but unfortunately not every
       generated prime number is prime (see Carmichael numbers). */
    rehash(hm);
  }
  while (1);
}

void* hashmap_remove(hashmap hash, unsigned long key)
{
  hashmap_t* hm = (hashmap_t*)hash;
  long index, i, step;

  index = key % hm->size;
  step = (key % (hm->size-2)) + 1;

  for (i = 0; i < hm->size; i++)
  {
    if (hm->table[index].data)
    {
      if (hm->table[index].key == key)
      {
        if (hm->table[index].flags & ACTIVE)
        {
          hm->table[index].flags &= ~ACTIVE;
          hm->count--;
          return hm->table[index].data;
        }
        else /* in, but not active (i.e. deleted) */
          return 0;
      }
    }
    else /* found an empty place (can't be in) */
      return 0;

    index = (index + step) % hm->size;
  }
  /* everything searched through, but not in */
  return 0;
}

void* hashmap_get(hashmap hash, unsigned long key)
{
  hashmap_t* hm = (hashmap_t*)hash;

  if (hm->count)
  {
    long index, i, step;
    index = key % hm->size;
    step = (key % (hm->size-2)) + 1;

    for (i = 0; i < hm->size; i++)
    {
      if (hm->table[index].key == key)
      {
        if (hm->table[index].flags & ACTIVE)
          return hm->table[index].data;
        break;
      }
      else
        if (!hm->table[index].data)
          break;

      index = (index + step) % hm->size;
    }
  }

  return 0;
}

long hashmap_count(hashmap hash)
{
  return ((hashmap_t*)hash)->count;
}

void hashmap_delete(hashmap hash)
{
  hashmap_t* hm = (hashmap_t*)hash;

  free(hm->table);
  free(hm);
}

void hashmap_fandel(hashmap hash, void(*fp)(void*))
{
  hashmap_t* hm = (hashmap_t*)hash;

  while (--hm->size >= 0)
    if (hm->table[hm->size].flags & ACTIVE)
      fp(hm->table[hm->size].data);

  free(hm->table);
  free(hm);
}
