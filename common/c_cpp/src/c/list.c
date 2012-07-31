/* $Id: list.c,v 1.14.16.4 2011/09/07 09:45:08 emmapollock Exp $
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

#include "wombat/port.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "wlock.h"
#include "list.h"

int gWListDebug;

/* For worst case memeory alignment. May need to change for some platforms. */
typedef long align;

typedef union header_
{
    struct
    {
        union header_* mPrev;
        union header_* mNext;
    } mNode;
    align nil;
} header;

struct wListImpl_;

typedef struct wIteratorImpl_
{
    struct wListImpl_     *list;
    header                *currentElement;
    struct wIteratorImpl_ *next;
} wIteratorImpl;

typedef struct wListImpl_
{
    /* Using dummy head and tail nodes makes implementation a lot simpler. */
    header mHead;          /* dummy head node */
    header mTail;          /* dummy tail node */
    header *mFreeList;     /* reuse elements to avoid excessive malloc() calls*/
    size_t mAllocSize;
    unsigned long size;
    wIteratorImpl *iterators;
    wLock lock; /* recursive locking mechanism */

} wListImpl;


/**
 * Create a list for elements of a specific size. This implementation does not
 * allow list elements of different sizes. Returns INVALID_LIST if it fails.
 *
 */

wList 
list_create (size_t elementSize)
{
    wListImpl* rval;

    rval = malloc (sizeof (wListImpl));
    memset (rval, 0, sizeof (wListImpl));

    /* We need to deal with alignment when alocating elements */
    rval->mAllocSize = 
        elementSize/sizeof(align) + (elementSize%sizeof(align) != 0);

    rval->mAllocSize *= sizeof(align);
    
    /* header alignment addressed in header struct/union definition */
    rval->mAllocSize += sizeof (header); 

    rval->mHead.mNode.mNext = &rval->mTail;
    rval->mTail.mNode.mPrev = &rval->mHead;

    rval->lock = wlock_create();

    return rval;
}

/**
 * Destroy a list. All of the elements are automatically freed. It is only
 * necessary to call list_free() for elements that are not currently on the
 * list.
 */

void
list_destroy (wList list, wListCallback destructor, void *closure)
{
    wListImpl*     impl        = (wListImpl*)list;
    header*        cur         = impl->mHead.mNode.mNext;
    wIteratorImpl* curIterator = impl->iterators;

    wlock_lock (impl->lock);

    while (cur != &impl->mTail)
    {
        header *next = cur->mNode.mNext;

        if (destructor != NULL)
        {
            destructor (list, cur + 1, closure);
        }

        free (cur);
        cur = next;
    }

    while (curIterator != NULL)
    {
       wIteratorImpl *nextIterator = curIterator->next;
       free (curIterator);
       curIterator = nextIterator;
    }

    cur = impl->mFreeList;
    while (cur != NULL)
    {
        header *next = cur->mNode.mNext;
        free (cur);
        cur = next;
    }
   
    wlock_unlock (impl->lock);
    wlock_destroy (impl->lock);

    free (list);
}

/**
 * Allocate an element.
 *
 * Returns a pointer to the element or NULL if it can not allocate the
 * memory.
 */
void*
list_allocate_element (wList list)
{
    header*       result = NULL;
    wListImpl* impl   = (wListImpl*)list;

    wlock_lock (impl->lock);
    if (impl->mFreeList != NULL)
    {
        result          = impl->mFreeList;
        impl->mFreeList = impl->mFreeList->mNode.mNext;
        memset (result, 0, impl->mAllocSize);
    }
    wlock_unlock (impl->lock);

    if (result == NULL)
    {
        result = calloc (1, impl->mAllocSize);
        if  (result == NULL)
        {
            return NULL;
        }
        memset (result, 0, impl->mAllocSize);
    }

    if (gWListDebug) 
        fprintf (stderr, "list: allocate: %p return: %p\n", 
                 (void*)result, (void *)(result + 1));

    return result + 1;
   
}

/**
 * Allocate a new element and add it to the end of the list
 * Same as list_allocate_element() followed by list_push_back()
 * Return a pointer to the new element, or NULL if an error occurs
 */
void*
list_add_element (wList list)
{
  void* element;

  if (NULL==list) return NULL;

  element = list_allocate_element(list);

  if (NULL==element) return NULL;
  
  list_push_back(list, element);

  return element;
}

static void
free_element_ (wListImpl *impl, header* ele)
{
    /* 
     * Don't free the element if it is still int the list.
     */
    if (ele->mNode.mPrev != NULL || ele->mNode.mNext != NULL)
    {
        return;
    }

    /* Set mPrev to special value to signify being added to freelist */
    ele->mNode.mPrev = (header*)-1;
    ele->mNode.mNext = impl->mFreeList;
    impl->mFreeList = ele;
    
    if (gWListDebug) 
        fprintf (stderr, "list: free: %p fl: %p\n", 
                (void*)ele, (void*)(impl->mFreeList));
}

/**
 * Free an element created by list_allocate().
 */
void
list_free_element (wList list, void* element)
{
    header*       ele   = ((header*)element) - 1;
    wListImpl* impl  = (wListImpl*)list;

    wlock_lock (impl->lock);

    if (&impl->mTail == ele) 
    {
        abort();
    }
    
    free_element_ (impl, ele);

    wlock_unlock (impl->lock);
}

/**
 * Put an element on the end of the list.
 *
 */
void
list_push_back (wList list, void* element)
{
    header*       ele   = ((header*)element) - 1;
    wListImpl* impl  = (wListImpl*)list;

    wlock_lock (impl->lock);
    ele->mNode.mNext               = &impl->mTail;
    ele->mNode.mPrev               = impl->mTail.mNode.mPrev;
    impl->mTail.mNode.mPrev->mNode.mNext = ele;
    impl->mTail.mNode.mPrev        = ele;

    impl->size++;
    wlock_unlock (impl->lock);
}

void
list_push_front (wList list, void* element)
{
    header*       ele   = ((header*)element) - 1;
    wListImpl* impl  = (wListImpl*)list;

    wlock_lock (impl->lock);
    
    ele->mNode.mPrev                     = &impl->mHead;
    ele->mNode.mNext                     = impl->mHead.mNode.mNext;
    impl->mHead.mNode.mNext->mNode.mPrev = ele;
    impl->mHead.mNode.mNext              = ele;

    impl->size++;
    wlock_unlock (impl->lock);
}

/**
 * Return the first element in the list.
 */
void*
list_get_head (wList list)
{
    wListImpl* impl  = (wListImpl*)list;
    void* rval = NULL;
    
    wlock_lock (impl->lock);
    rval = impl->mHead.mNode.mNext == &impl->mTail ? NULL :
        impl->mHead.mNode.mNext + 1;
    wlock_unlock (impl->lock);

    return rval;
}

/**
 * Return the last element in the list.
 */
void*
list_get_tail (wList list)
{
    wListImpl* impl  = (wListImpl*)list;
    void* rval = NULL;
    
    wlock_lock (impl->lock);
    rval = impl->mHead.mNode.mNext == &impl->mTail ? NULL :
        impl->mTail.mNode.mPrev + 1;
    wlock_unlock (impl->lock);

    return rval;
}

/*
 * Return the element at the specified index
 * If index is out-of-bounds, return NULL
 * Index is zero-index i.e. index=0 is the first item in the list
 */
void*
list_get_element_at_index (wList list, int index)
{
  wListImpl* impl;
  int i, count;
  void* result;

  if (NULL==list) return NULL;

  impl = (wListImpl*)list;
  
  wlock_lock(impl->lock);

  count = list_size(list);

  if ((index<0) || (index>=count))
  {
    result = NULL;
  }
  else
  {
    result = list_get_head(list);
    for (i=0;i<index;i++)
      result = list_get_next(list, result);
  }

  wlock_unlock(impl->lock);

  return result;
}

void
list_lock (wList list)
{
    wListImpl* impl  = (wListImpl*)list;
    wlock_lock (impl->lock);
}


void 
list_unlock (wList list)
{
    wListImpl* impl  = (wListImpl*)list;
    wlock_unlock (impl->lock);
}

/**
 * Return the element after the specified element.
 */
void*
list_get_next (wList list, void *element)
{
    wListImpl* impl  = (wListImpl*)list;
    header* ele = ((header*)element) - 1;
    void *rval = NULL;

    wlock_lock (impl->lock);
    rval = ele->mNode.mNext == &impl->mTail ? NULL : 
        ele->mNode.mNext + 1;
    wlock_unlock (impl->lock);

    return rval;
}

static void
insert_before (wListImpl *impl, header* what, header *where)
{
    what->mNode.mNext               = where;
    what->mNode.mPrev               = where->mNode.mPrev;

    where->mNode.mPrev->mNode.mNext = what;
    where->mNode.mPrev              = what;
    
    impl->size++;
}

/**
 * Insert a list item before the specified item.
 *
 * NOTE: Since iterators do not move backwards, this should be safe.
 */
void
list_insert_before (wList list, void* position, void* element)
{
    header* where = ((header*)position) - 1;
    header* what  = ((header*)element) - 1;
    wListImpl* impl  = (wListImpl*)list;

    wlock_lock (impl->lock);
    /* We just fail quietly if the position is not in the list or the passed
     * elelemnt is in the list. TODO: Introduce better error handling into
     * list
     */
    if (where->mNode.mPrev == NULL || what->mNode.mNext != NULL)
    {
        wlock_unlock (impl->lock);
        fprintf (stderr, "Unexpected: list insert failed.");
        return;
    }

    insert_before (impl, what, where);

    wlock_unlock (impl->lock);
}

/**
 * Return non zero if the list is empty.
 */
static int
list_is_empty (wList list)
{
    wListImpl* impl  = (wListImpl*)list;
    int rval = 0;

    wlock_lock (impl->lock);
    rval = (impl->mHead.mNode.mNext == &impl->mTail);
    wlock_unlock (impl->lock);

    return rval;
}

static void
list_adjust_iterators (wListImpl *list, header *ele)
{
    /* ele is being deleted */
    wIteratorImpl *cur = list->iterators;
    header *next =  ele->mNode.mNext;

    if (next == &list->mTail && list->size > 1)
    {
        next = list->mHead.mNode.mNext;
    }

    while (cur != NULL)
    {
        if (cur->currentElement == ele)
        {
            cur->currentElement = next;
        }
        cur = cur->next;
    }
}

/**
 * Remove an element from the front of the list. list_free_element() must be
 * called if the element is not placed back on the list.
 */
void*
list_pop_front (wList list)
{
    wListImpl* impl  = (wListImpl*)list;
    header*       rval;

    wlock_lock (impl->lock);

    if (list_is_empty (list))
    {
        wlock_unlock (impl->lock);
        return NULL;
    }

    rval                = impl->mHead.mNode.mNext;

    list_adjust_iterators (impl, rval);

    impl->mHead.mNode.mNext   = rval->mNode.mNext;
    rval->mNode.mNext->mNode.mPrev  = &impl->mHead;

    rval->mNode.mPrev = rval->mNode.mNext = NULL; /* so we know it is not in the list */
    impl->size--;
    wlock_unlock (impl->lock);

    return rval + 1;
}

/**
 * Remove the specified element from the list. list_free_element() must be
 * called subsequently.
 */
void
list_remove_element (wList list, void* element)
{
    header*       ele   = ((header*)element) - 1;
    wListImpl* impl  = (wListImpl*)list;


    wlock_lock (impl->lock);
    list_adjust_iterators (impl, ele);

    if (ele == &impl->mTail)
    {
        abort();
    }

    /* Check null for removed items and check -1 for freelist items */
    if((ele->mNode.mPrev == NULL) || (ele->mNode.mPrev == (header*)-1))
    {
        wlock_unlock (impl->lock);
        return; /* The element is not in the list. has already been 
                    removed / freed */
    }

    ele->mNode.mPrev->mNode.mNext = ele->mNode.mNext;
    ele->mNode.mNext->mNode.mPrev = ele->mNode.mPrev;

    impl->size--;

    ele->mNode.mNext = ele->mNode.mPrev = NULL;

    wlock_unlock (impl->lock);
}

/**
 * Invoke the specified callback for each element in the list passing the
 * specified closure. The callback may remove the current element. This method
 * does not lock the list, but there should be no problems as long as the
 * current or next node are not removed.
 */
void
list_for_each (wList list, wListCallback cb, void* closure)
{
    wListImpl* impl   = (wListImpl*)list;
    header *cur;

    wlock_lock (impl->lock);

    cur          = impl->mHead.mNode.mNext;

    while (cur != &impl->mTail && cur != &impl->mHead)
    {
        header *next = cur->mNode.mNext;

        if (cb != NULL)
        {
            cb (list, cur + 1, closure);
        }
        cur = next;
    }
    wlock_unlock (impl->lock);
}

unsigned long
list_size (wList list)
{
    wListImpl* impl   = (wListImpl*)list;
    return impl->size;
}

static header *
getCurrentNode (wIteratorImpl *it)
{
    header *rval = NULL;

    if (it->currentElement == &it->list->mHead ||
        it->currentElement == &it->list->mTail)
    {
        if (it->list->size > 0) /* insert to empty list */
        {
            it->currentElement = it->list->mHead.mNode.mNext;
            rval = it->currentElement;
        }
    }
    else
    {
        rval = it->currentElement;
    }

    return rval;
}

/*
 * Create an iterator. 
 */
wIterator
list_create_iterator (wList list)
{
    wListImpl* impl   = (wListImpl*)list;

    wIteratorImpl* rval = 
        (wIteratorImpl*)malloc (sizeof (wIteratorImpl));
    memset (rval, 0, sizeof (wIteratorImpl));

    wlock_lock (impl->lock);

    rval->list = impl;
    rval->next = impl->iterators;
    impl->iterators = rval;
    
    rval->currentElement = impl->mHead.mNode.mNext;
    
    wlock_unlock (impl->lock);

    return rval;
}

/*
 * Return the iterator's current node.
 */
void*
iterator_current (wIterator iterator)
{
    wIteratorImpl* it   =   (wIteratorImpl*)iterator;
    header *rval = NULL;

    wlock_lock (it->list->lock); /* current could be deleted */
    rval = getCurrentNode (it);
    wlock_unlock (it->list->lock); 

    return rval == NULL ? NULL : rval + 1;
}

void
iterator_insert_before (wIterator iterator, void *element)
{
    wIteratorImpl* it   =   (wIteratorImpl*)iterator;
    header *ele   = ((header*)element) - 1;

    wlock_lock (it->list->lock); /* current could be deleted */
    insert_before (it->list, ele, getCurrentNode(it));
    wlock_unlock (it->list->lock); /* current could be deleted */
}

/*
 * Return the current node and advance to the next node.
 */
void*
iterator_next (wIterator iterator)
{
    wIteratorImpl* it   =   (wIteratorImpl*)iterator;
    header *rval = NULL;

    wlock_lock (it->list->lock); /* current could be deleted */
    
    if (it->currentElement != &it->list->mTail)
    {
        rval = getCurrentNode (it);
        it->currentElement = it->currentElement->mNode.mNext;
    }
   
    wlock_unlock (it->list->lock); /* current could be deleted */

    return rval == NULL ? NULL : rval + 1;
}

/*
 * Return the current node and advance to the next node.
 */
void*
iterator_next_circ (wIterator iterator)
{
    wIteratorImpl* it   =   (wIteratorImpl*)iterator;
    header *rval = NULL;

    wlock_lock (it->list->lock); 
    
    rval = getCurrentNode (it);

    if (it->currentElement->mNode.mNext == &it->list->mTail)
    {
        it->currentElement = it->list->mHead.mNode.mNext;
    }
    else
    {
        it->currentElement = it->currentElement->mNode.mNext;
    }
   
    wlock_unlock (it->list->lock); 

    return rval == NULL ? NULL : rval + 1;
}

/*
 * Destroy an iterator.
 *
 */
void
iterator_destroy (wIterator iterator)
{
    wIteratorImpl* it   =   (wIteratorImpl*)iterator;

    wlock_lock (it->list->lock); 
    if (it->list->iterators == it)
    {
        it->list->iterators = it->next;
    }
    else
    {
        wIteratorImpl* cur  =   it->list->iterators;
        while (cur != NULL)
        {
            if (cur->next == it)
            {
                cur->next = it->next;
                break;
            }
        }
    }
    wlock_unlock (it->list->lock); 

    free(it);
}

/*
 * Move the specified element from its current position to the postion before
 * the iterator. 
 */
void
iterator_move_element_before (wIterator iterator, void *element)
{
    wIteratorImpl* it   =   (wIteratorImpl*)iterator;
    header*       ele   = ((header*)element) - 1;
    header*       cur   = NULL;

    wlock_lock (it->list->lock); 

    cur = getCurrentNode(it);
    
    if (ele != cur && ele != cur->mNode.mPrev)
    {
        list_remove_element (it->list, element);
        insert_before (it->list, ele, cur);
    }
    
    wlock_unlock (it->list->lock); 
}

