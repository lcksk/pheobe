/*
 * linkedlist.h
 *
 *  Created on: Sep 28, 2010
 *      Author: buttonfly
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_


#ifdef __cplusplus
extern "C"
{
#endif


typedef void*    linkedlist;
/**
 * constructs an empty list
 * @return an instance of linkedlist
 */
extern linkedlist linkedlist_new(void);

/**
  * appends the specified element to the end of thie list
  * @param list an linkedlist instance which contains elements
  * @param data an element to be inserted.
  */
extern int linkedlist_add(linkedlist list, void* data);

/**
  * appends the specified element to the end of thie list
  * @param linkedlist an arraylist instance which contains elements
  * @param data an element to be inserted.
  */
extern int linkedlist_add_at(linkedlist list, void* data, int i);

/**
 * removes the element at the specified position in this list.
 * shifts any subsequent elements to the left (subtracts one from their indices)
 * @param list an linkedlist instance which contains elements
 * @pararm data an element to be removed
 * @return the data which was removed from the list.
 */
extern void* linkedlist_remove_at(linkedlist list, int i);

/**
 * removes a single instance of specified element from this list
 * @param list an linkedlist instance which contains elements
 * @param data an element to be removed from this list, if present
 */
extern void* linkedlist_remove(linkedlist list, void* data);

/**
 * returns the element at the specified position in this list.
 * @param i    index of element to return
 * @return the element at the specified position in this list. it returns null unless present
 */
extern const void* linkedlist_get(linkedlist list, int i);

/**
 * returns the number of elements in this list
 * @param list linkedlist
 * @return the number of elements in this list
 */
extern int linkedlist_size(linkedlist list);

/**
  * remove all node from this list
  * @param list an linkedlist instance which contains elements
  * @param obj an element to be inserted.
  */
extern void linkedlist_clear(linkedlist, void(*fp)(void*));

/**
 * destroy this list. if you've used memory which is allocated in heap space,
 * you have to deallocate those before calling this function
 * @param list linkedlist
 * @param fp callback
 * @return close this list
 */
extern void linkedlist_delete(linkedlist, void(*fp)(void*));

#ifdef __cplusplus
}
#endif


#endif /* LINKEDLIST_H_ */
