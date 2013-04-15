#ifndef _HASHMAP_H
#define _HASHMAP_H

#ifdef _cplusplus
extern "C"
{
#endif

typedef void* hashmap;

/**
 * constructs an empty hashmap with the specified initial capacity
 * param startsize the initial capacity
 * return the hashmap instance
 */
extern hashmap hashmap_new(int startsize);

/**
 * associates the specified value with the specified key in this map.
 * param hashmap this hashmap
 * @param data value to be associated with the specified key
 * @param key key with which the specified value is to be associated
 */
extern void hashmap_insert(hashmap hashmap, void* data, unsigned long key);

/**
 * removes the mapping for this key from this map if present
 * @param key key whose mapping is to be removed from the map
 * @return the value which is associated with the specified key
 */
extern void* hashmap_remove(hashmap hashmap, unsigned long key);

/**
 * returns the value to which the specified key is mapped in this identity hashmap
 * @param hashmap this hashmap
 * @param key the key whose associated value is to be returned
 */
extern void* hashmap_get(hashmap hashmap, unsigned long key);

/**
 * returns the number of key-value mapping in this map
 * @param hashmap this map
 * @return the number of key-value mappings in this map
 */
extern long hashmap_count(hashmap hashmap);

/**
 * delete this map. this function does same work as hashmap_delete.
 * but you can have a chance to dealloate the memory you allocated
 * @param hashmap this map
 * @param fp the function pointer.
 */
extern void hashmap_fandel(hashmap hashmap, void (*fp)(void*));

/**
 * delete this map
 * @param hashmap this map to be deleted
 */
extern void hashmap_delete(hashmap hashmap);

#ifdef _cplusplus
}
#endif
#endif
