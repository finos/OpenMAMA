/* $Id: list.h,v 1.7.16.3 2011/08/10 14:53:24 nicholasmarriott Exp $
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Technologies, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#ifndef LIST_H__
#define LIST_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "wombat/wincompat.h"
#include <stddef.h> /* for size_t */

#define INVALID_LIST NULL

/**
 * List handle.
 */
typedef void* wList;

/*
 * Iterator handle.
 */
typedef void* wIterator;

/**
 * Callback typedef fucntion for list_for_each() 
 *
 * The signature should be: 
 *      void function (void* element, void* closure)
 */
typedef void (*wListCallback)(wList list, void* element, void* closure);

/**
 * Create a list for elements of a specific size. This implementation does not
 * allow list elements of different sizes. Returns INVALID_LIST if it fails.
 */
COMMONExpDLL
wList 
list_create (size_t elementSize);

/**
 * Destroy a list. All of the elements are automatically freed. It is only
 * necessary to call list_free() for elements that are not currently on the
 * list. The destructor callback gets invoked for every element that remains
 * in the list. This can be used to free resources.
 */
COMMONExpDLL
void
list_destroy (wList list, wListCallback destructor, void *closure);

/**
 * Allocate an element.
 *
 * Returns a pointer to the element or NULL if it can not allocate the
 * memory.
 */
 COMMONExpDLL
void*
list_allocate_element (wList list);

/**
 * Free an element created by list_allocate(). This should not be called for
 * an element that has been put in the list. If the element is in the list, it
 * will not be freed or removed.
 */
 COMMONExpDLL
void
list_free_element (wList list, void* element);

/**
 * Insert an element at the beginning of the list.
 */
 COMMONExpDLL
void
list_push_front (wList list, void* element);

/**
 * Put an element on the end of the list.
 *
 */
 COMMONExpDLL
void
list_push_back (wList list, void* element);

/**
 * Allocate a new element and add it to the end of the list
 * Same as list_allocate_element() followed by list_push_back()
 * Return a pointer to the new element, or NULL if an error occurs
 */
 COMMONExpDLL
void*
list_add_element (wList list);

/**
 * Insert a list item before the specified item.
 */
 COMMONExpDLL
void
list_insert_before (wList list, void* position, void* element);

/**
 * Return the first element in the list.
 */
 COMMONExpDLL
void*
list_get_head (wList list);

/**
 * Return the last element in the list.
 */
 COMMONExpDLL
void*
list_get_tail (wList list);

/**
 * Return the element after the specified element.
 */
 COMMONExpDLL
void*
list_get_next (wList list, void *element);

/**
 * Return the element at the specified index
 * If index is out-of-bounds, return NULL
 * Index is zero-index i.e. index=0 is the first item in the list
 */
 COMMONExpDLL
void*
list_get_element_at_index (wList list, int index);

/**
 * Remove an element from the front of the list. list_free_element() must be
 * called if the element is not placed back on the list.
 */
 COMMONExpDLL
void*
list_pop_front (wList list);

/**
 * Remove the specified element from the list.
 */
 COMMONExpDLL
void
list_remove_element (wList list, void* element);


/**
 * Invoke the specified callback for each element in the list passing the
 * specified closure. The callback may remove the current element.
 */
 COMMONExpDLL
void
list_for_each (wList list, wListCallback cb, void* closure);

/**
 * Return the number of entries in the list.
 */
 COMMONExpDLL
unsigned long 
list_size (wList list);

/**
 * Lock the list. Must be followed by unlock. Locks are recursive.
 */
 COMMONExpDLL
void
list_lock (wList list);

COMMONExpDLL
void 
list_unlock (wList list);

/*
 * The list iterator provides a robust and thread-safe, cursor mechanism for
 * lists. It treats the list as a cirularly linked list: when it reaches the end
 * iterator_next() moves the iterator back to the first element. The iterator
 * remains valid event when elements are deleted. If the current element is
 * deleted the iterator is moved to the next element. For an empty list the
 * iterator's current node is NULL.
 */

/**
 * Create an iterator.
 */
 COMMONExpDLL
wIterator
list_create_iterator (wList list);
/**
 * Return the iterator's current node.
 */
 COMMONExpDLL
void*
iterator_current (wIterator iterator);

/**
 * Return the current node and advance to the next node.
 */
 COMMONExpDLL
void*
iterator_next (wIterator iterator);

/**
 * Return the current node and advance to the next node. Return 
 * to the first node after the last.
 */
 COMMONExpDLL
void*
iterator_next_circ (wIterator iterator);

/**
 * Insert the specified element before the current element of the iteration.
 */
 COMMONExpDLL
void
iterator_insert_before (wIterator iterator, void *element);

/**
 * Destroy an iterator.
 *
 */
 COMMONExpDLL
void
iterator_destroy (wIterator iterator);

/**
 * Move the specified element from its current position to the postion before
 * the iterator. 
 */
 COMMONExpDLL
void
iterator_move_element_before (wIterator iterator, void *element);


#if defined(__cplusplus)
}
#endif

#endif /* LIST_H__ */

