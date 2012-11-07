/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: blst_queue.h $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 12/15/10 12:42p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/lst/blst_queue.h $
 * 
 * Hydra_Software_Devel/7   12/15/10 12:42p erickson
 * SW7420-941: add #include for bstd.h. too many files #include blst
 * before bstd.
 * 
 * Hydra_Software_Devel/6   12/15/10 11:02a erickson
 * SW7420-941: add BDBG_ASSERT to BLST macros to ensure proper removal
 * 
 * Hydra_Software_Devel/5   1/15/04 10:04a erickson
 * PR9321: fixed BLST_Q_INSERT_TAIL and BLST_Q_REMOVE_HEAD
 * 
 * Hydra_Software_Devel/4   12/11/03 4:38p vsilyaev
 * PR 8962: Remove semicolons after while(0)
 *
 * Hydra_Software_Devel/3   10/8/03 5:21p darnstein
 * Fix BLST_Q_INSERT_TAIL.  Well, at least it compiles now.
 * 
 * Hydra_Software_Devel/2   10/8/03 5:15p darnstein
 * Get rid of DOS line control. A nontrivial checkin will follow shortly.
 * 
 * Hydra_Software_Devel/1   3/14/03 7:50p vsilyaev
 * Double linked queue.
 * 
 * Hydra_Software_Devel/2   3/12/03 4:37p vsilyaev
 * Fixed some macro names.
 * 
 * Hydra_Software_Devel/1   3/10/03 4:32p vsilyaev
 * Doubly linked list.
 * 
 *
 ***************************************************************************/

#ifndef BLST_QUEUE_H
#define BLST_QUEUE_H   

/*================== Module Overview =====================================
This modules defines macros to control doubly linked queue.
All operations are typesafe (doesn't required typecasting) and constant time.

This list allow:
  o Insert a new entry at the head of the list
  o Insert a new entry after/before any element in the list
  o O(1) removal of any entry in the list
  o Forward traversal through the list
  o Each element requires two pointers
  o Code size and execution time is about 2.5 that for singly-linked list
  
========================================================================*/

#include "bstd.h"
#include "bdbg.h"
 
/***************************************************************************
Summary:
    Creates new data type for the list head
    
Description:
    Creates new data type for the list head, this type used to create variable for the lists head.
    User should create new the list head data type for every different element datatype.
    
Input:
    name - name for the new data type (structure)
    type - existing user data type used for element of the list

Example:    
     struct block {
        BLST_Q_ENTRY(block) link;
        char string[256];
     };  
     
     BLST_Q_HEAD(blockqueue, block) queue;
     BLST_Q_INIT(&queue);

Returns:
    <none>
****************************************************************************/
#define BLST_Q_HEAD(name, type) struct name { struct type *q_first, *q_last; }

/***************************************************************************
Summary:
    Defines links entry
    
Description:
    Defines entrys for the list inside the user structure.for the element.
    
Input:
    type - the datatype for element

Example:
     struct block {
        BLST_Q_ENTRY(block) link;
        char string[256];
     };  

Returns:
    <none>
****************************************************************************/
#define BLST_Q_ENTRY(type)  struct { struct type *q_next, *q_prev; const void *l_head; }

/***************************************************************************
Summary:
    Initializes lists head
    
Description:
    Initializes the head of the list. The head shall be initialized before list can be used.
    This macro used for dynamic initialization.
    
Input:
    head - pointer to the list head

See also:
    BLST_Q_INITIALIZER

Example:
    BLST_Q_INIT(&head);

Returns:
    <none>
****************************************************************************/
#define BLST_Q_INIT(head) ((head)->q_last = (head)->q_first=NULL)

/***************************************************************************
Summary:
    Initializes lists head
    
Description:
    Initializes the head of the list. The head shall be initialized before list can be used.
    This macro used for static initialization.
    
Input:
    head - pointer to the list head

See also:
    BLST_Q_INIT

Example:
    static struct block_head  head = BLST_Q_INITIALIZER(&head);

Returns:
    <none>
****************************************************************************/
#define BLST_Q_INITIALIZER(head) {NULL, NULL}

/***************************************************************************
Summary:
    Tests if list is empty
    
Description:
    Tests if list is empty.

Input:
    head - pointer to the list head

Returns:
    true - list empty
    false - list has elements

Example:
    if (BLST_Q_EMPTY(&head) { return ; }

****************************************************************************/
#define BLST_Q_EMPTY(head) ((head)->q_first == NULL)

/***************************************************************************
Summary:
    Returns the lists first element
    
Description:
    Returns pointer to the first element from the list
    
Input:
    head - pointer to the list head

Returns:
    pointer to the first element from the list.

Example:
    struct block *first=BLST_Q_FIRST(&head);
****************************************************************************/
#define BLST_Q_FIRST(head) ((head)->q_first)

/***************************************************************************
Summary:
    Returns the lists first element
    
Description:
    Returns pointer to the last element from the list

Input:
    head - pointer to the list head

Returns:
    pointer to the last element from the list.

Example:
    struct block *first=BLST_Q_FIRST(&head);
****************************************************************************/
#define BLST_Q_LAST(head) ((head)->q_last)

/***************************************************************************
Summary:
    Returns next element from the lists
    
Description:
    Returns pointer to the next element from the list
    
Input:
    elm - pointer to the list element
    field - name of the elements link field

Returns:
    pointer to the next element from the list

Example:
    struct block *second=BLST_Q_NEXT(first);
****************************************************************************/
#define BLST_Q_NEXT(elm, field) ((elm)->field.q_next)


/***************************************************************************
Summary:
    Returns next element from the lists
    
Description:
    Returns pointer to the previous element from the list
    
Input:
    elm - pointer to the list element
    field - name of the elements link field

Returns:
    pointer to the previous element from the list

Example:
    struct block *first=BLST_Q_PREV(second);
****************************************************************************/
#define BLST_Q_PREV(elm, field) ((elm)->field.q_prev)

/***************************************************************************
Summary:
    Inserts element into the list

Description:
    Inserts new element into the head of the list.

Input:
    head - pointer to the list head
    new_elm - pointer to the new element
    field - name of the elements link field

Returns:
    <none>

Example:
    BLST_Q_INSERT_HEAD(&head, new_block, link);
****************************************************************************/
#define BLST_Q_INSERT_HEAD(head, new_elm, field) do { \
    (new_elm)->field.l_head = (const void *)head; \
    if ( ((new_elm)->field.q_next = (head)->q_first) != NULL ) (head)->q_first->field.q_prev = (new_elm); \
    if ( (head)->q_last == NULL) (head)->q_last = (new_elm); \
    (head)->q_first = (new_elm); (new_elm)->field.q_prev = NULL;}  while(0)

/***************************************************************************
Summary:
    Inserts element into the list
    
Description:
Description:
    Inserts new element into the tail list.
    
Input:
    head - pointer to the list head
    new_elm - pointer to the new element
    field - name of the elements link field

Returns:
    <none>

Example:
    BLST_Q_INSERT_TAIL(&head, elm, link);
****************************************************************************/
#define BLST_Q_INSERT_TAIL(head, new_elm, field) do { \
    (new_elm)->field.l_head = (const void *)head; \
    (new_elm)->field.q_next = NULL;               \
    (new_elm)->field.q_prev = (head)->q_last;     \
    if ((head)->q_last) {                         \
            (head)->q_last->field.q_next = (new_elm); \
    } else {                                      \
            (head)->q_first = (new_elm);          \
    }                                             \
    (head)->q_last = (new_elm);                   \
   } while(0)
    

/***************************************************************************
Summary:
    Inserts element into the list
    
Description:
    Inserts new element after existing element.
    
Input:
    head - pointer to the list head
    elm - pointer to the element from the list
    new_elm - pointer to the new element
    field - name of the elements link field

Returns:
    <none>

Example:
    BLST_Q_INSERT_AFTER(&head, first, second, link);
****************************************************************************/
#define BLST_Q_INSERT_AFTER(head, elm, new_elm, field) do { \
    BDBG_ASSERT((elm)->field.l_head == (const void *)head); \
    (new_elm)->field.l_head = (const void *)head; \
    (new_elm)->field.q_prev = (elm); \
    if (((new_elm)->field.q_next = (elm)->field.q_next)!=NULL)  (elm)->field.q_next->field.q_prev = (new_elm); \
    if ((elm)->field.q_next == NULL)  (head)->q_last = (new_elm);\
    (elm)->field.q_next = (new_elm); } while(0)

/***************************************************************************
Summary:
    Inserts element into the list
    
Description:
    Inserts new element after existing element.
    
Input:
    head - pointer to the list head
    elm - pointer to the element from the list
    new_elm - pointer to the new element
    field - name of the elements link field

Returns:
    <none>

Example:
    BLST_Q_INSERT_BEFORE(&head, second, first, link);
****************************************************************************/
#define BLST_Q_INSERT_BEFORE(head, elm, new_elm, field) do { \
    BDBG_ASSERT((elm)->field.l_head == (const void *)head); \
    (new_elm)->field.l_head = (const void *)head; \
    (new_elm)->field.q_next = (elm); \
    if (((new_elm)->field.q_prev = (elm)->field.q_prev)!=NULL) elm->field.q_prev->field.q_next = new_elm; \
    else (head)->q_first = (new_elm); \
    (elm)->field.q_prev = (new_elm); } while(0)



/***************************************************************************
Summary:
    Removes element from the list

Description:
    Removes element from the list.

Input:
    head - pointer to the list head
    elm - pointer to the list element
    field - name of the elements link field

See also:
    BLST_Q_REMOVE_HEAD

Returns:
    <none>

Example:
    BLST_Q_REMOVE(&head, first, link);
****************************************************************************/
#define BLST_Q_REMOVE(head, elm, field) do { \
    BDBG_ASSERT((elm)->field.l_head == (const void *)head); \
    if ((elm)->field.q_next) (elm)->field.q_next->field.q_prev = (elm)->field.q_prev;  else (head)->q_last = (elm)->field.q_prev; \
    if ((elm)->field.q_prev) (elm)->field.q_prev->field.q_next = (elm)->field.q_next; else (head)->q_first = (elm)->field.q_next; \
    } while(0)

/***************************************************************************
Summary:
    Removes element from the list

Description:
    Removes element from the head of the list.

Input:
    head - pointer to the list head
    field - name of the elements link field

See also:
    BLST_Q_REMOVE

Returns:
    <none>

Example:
    BLST_Q_REMOVE_HEAD(&head, first, link);
****************************************************************************/
#define BLST_Q_REMOVE_HEAD(head, field) do { \
    BDBG_ASSERT((head)->q_first); \
    BDBG_ASSERT((head)->q_first->field.l_head == (const void *)head); \
    (head)->q_first = (head)->q_first->field.q_next; \
    if ((head)->q_first) { (head)->q_first->field.q_prev = NULL;} else (head)->q_last=NULL; \
    } while(0)


#endif /* BLST_QUEUE_H  */
