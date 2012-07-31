/* $Id: symbollist.c,v 1.15.24.4 2011/09/01 09:41:03 emmapollock Exp $
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

/* *************************************************** */
/* Includes. */
/* *************************************************** */
#include "list.h"
#include "mama/symbollist.h"
#include "mama/symbollistmember.h"
#include "mama/symbollisttypes.h"

/* *************************************************** */
/* Structures. */
/* *************************************************** */

/* Implementation of the symbol list. */
typedef struct mamaSymbolList_
{
    /* This callback is invoked whenever new members are added to the symbol list. */
    addSymbolCbType myAddCb;

    /* User supplied closure. */
    void *myClosure;
    
    /* This list contains mamaSymbolListMemberImpl objects, one for each symbol. */
    wList myMembers;

    /* This callback is invoked whenever members are removed from the symbol list. */
    removeSymbolCbType myRemoveCb;
    
} mamaSymbolListImpl;

/* *************************************************** */
/* Private Function Prototypes. */
/* *************************************************** */

/**
 * This function is used by mamaSymbolList_clear to remove all of the members
 * from the list. It will be invoked for each member and will de-allocate all
 * associated memory.
 * Note that the remove callback will be invoked for each member.
 *
 * @param[in] list The list of members.
 * @param[in] member Casted pointer to the symbol list member.
 * @param[in] closure Casted pointer to the symbol list impl.
 */
void mamaSymbolListImpl_removeMember(wList list, void *member, void *closure);

/* *************************************************** */
/* Public Functions. */
/* *************************************************** */

mama_status mamaSymbolList_addMember(mamaSymbolList symbolList, mamaSymbolListMember member) 
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if((NULL != symbolList) && (NULL != member))
    {
        /* Get the impl. */
        mamaSymbolListImpl *impl = (mamaSymbolListImpl *)symbolList;

        /* Add the new member to the list. */        
        list_push_back (impl->myMembers, member);

        /* Invoke the callback if it has been supplied. */
        if(NULL != impl->myAddCb)
        {
            impl->myAddCb(member, impl->myClosure);
        }

        /* Function succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSymbolList_allocate(mamaSymbolList *result) 
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != result)
    {
        /* Allocate a new impl. */
        mamaSymbolListImpl *impl = (mamaSymbolListImpl*) calloc(1, sizeof(mamaSymbolListImpl));
        ret = MAMA_STATUS_NOMEM;
        if(NULL != impl)
        {
            /* Create the list which will contain symbol list member structures. */
            impl->myMembers = list_create(mamaSymbolListMember_getSize());

            /* Function succeeded. */
            ret = MAMA_STATUS_OK;
        }

        /* Return the impl. */
        *result = (mamaSymbolList)impl;
    }
   
    return ret;
}

mamaSymbolListMember mamaSymbolList_allocateMember(mamaSymbolList symbolList)
{
    /* Returns. */
    mamaSymbolListMember ret = NULL;
    if(NULL != symbolList)
    {
        /* Get the impl. */
        mamaSymbolListImpl *impl = (mamaSymbolListImpl *)symbolList;

        /* Allocate a new member for the symbol list. */
        ret = list_allocate_element(impl->myMembers);
    }

    return ret;
}

mama_status mamaSymbolList_clear(mamaSymbolList symbolList, int membersToo) 
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != symbolList)
    {
        /* Get the impl. */
        mamaSymbolListImpl *impl = (mamaSymbolListImpl *)symbolList;
    
        /* Remove all the members if the flag is set. */
        if(0 != membersToo)
        {
            list_for_each(impl->myMembers, mamaSymbolListImpl_removeMember, impl);
        }

        /* Function succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSymbolList_deallocate(mamaSymbolList symbolList)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != symbolList)
    {
        /* Get the impl. */
        mamaSymbolListImpl *impl = (mamaSymbolListImpl*)symbolList;
    
        /* Remove all the symbol list member structures. */
        ret = mamaSymbolList_clear(symbolList, 0);
        if(MAMA_STATUS_OK == ret)
        {
            /* Destroy the member list. */
            list_destroy(impl->myMembers, NULL, NULL);

            /* Free the impl. */
            free(impl);

            /* The function has succeeded. */
            ret = MAMA_STATUS_OK;
        }
    }

    return ret;
}

mama_status mamaSymbolList_deallocateMember(mamaSymbolList symbolList, mamaSymbolListMember member)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if((NULL != symbolList) && (NULL != member))
    {
        /* Get the impl. */
        mamaSymbolListImpl *impl = (mamaSymbolListImpl *)symbolList;

        /* Free the element corresponding to this member. */
        list_free_element(impl->myMembers, member);

        /* Function succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSymbolList_deallocateWithMembers(mamaSymbolList symbolList)
{
    /* This is the same as deallocate. */
    return mamaSymbolList_deallocate(symbolList);
}

mama_status mamaSymbolList_findMember(const mamaSymbolList symbolList, const char *symbol, const char *source, mamaTransport transport, mamaSymbolListMember *member) 
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if((NULL != symbolList) && (NULL != member))
    {
        /* Get the impl. */
        mamaSymbolListImpl *impl = (mamaSymbolListImpl *)symbolList;

        /* Create an iterator to traverse the list of members. */
        mamaSymbolListMember tempMember = NULL;
        wIterator iterator = list_create_iterator(impl->myMembers);

        /* Enumerate all members. */
        ret = MAMA_STATUS_INVALID_ARG;
        while (NULL != (tempMember = iterator_next(iterator)))
        {
            /* Get details from the member. */
            const char * tempSymbol = NULL;
            const char * tempSource = NULL;
            mamaTransport tempTport = NULL;
            mamaSymbolListMember_getSymbol(tempMember, &tempSymbol);
            mamaSymbolListMember_getSource(tempMember, &tempSource);
            mamaSymbolListMember_getTransport(tempMember, &tempTport);
            
            /* Compare the member's names and transport. */
            if((strncmp(source, tempSource, MAMA_MAX_SOURCE_LEN) == 0) && (transport == tempTport))
            {
                /* Match, quit out. */
                ret = MAMA_STATUS_OK;
                break;
            }
        }

        /* Destroy the iterator. */
        iterator_destroy(iterator);

        /* Return the member. */
        *member = tempMember;
    }
    
    return ret;
}

mama_status mamaSymbolList_getClosure(const mamaSymbolList symbolList, void **closure)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if((NULL != symbolList) && (NULL != closure))
    {
        /* Get the impl. */
        mamaSymbolListImpl *impl = (mamaSymbolListImpl *)symbolList;

        /* Return the closure. */
        *closure = impl->myClosure;

        /* Function succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSymbolList_getSize(const mamaSymbolList symbolList, unsigned long *size)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if((NULL != symbolList) && (NULL != size))
    {
        /* Get the impl. */
        mamaSymbolListImpl *impl = (mamaSymbolListImpl *)symbolList;

        /* Return the size. */
        *size = list_size(impl->myMembers);

        /* Function succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSymbolList_iterate(mamaSymbolList symbolList, mamaSymbolListIterateMemberFunc memberFunc, mamaSymbolListIterateCompleteFunc completeFunc, void *iterateClosure)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if((NULL != symbolList) && (NULL != memberFunc))
    {
        /* Get the impl. */
        mamaSymbolListImpl *impl = (mamaSymbolListImpl *)symbolList;

        /* Create a new iterator. */        
        wIterator  i = list_create_iterator (impl->myMembers);
        
        /* Iterate all the members in the list. */
        mamaSymbolListMember member = NULL;
        while (NULL != (member = iterator_next(i)))
        {
            memberFunc (symbolList, member, iterateClosure);
        }

        /* Destroy the iterator. */
        iterator_destroy (i);
        
        /* Invoke the callback if it has been supplied. */
        if(NULL != completeFunc)
        {
            completeFunc(symbolList, iterateClosure);
        }

        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSymbolList_removeMember(mamaSymbolList symbolList, const char *symbol, const char *source, mamaTransport transport, mamaSymbolListMember *member)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if((NULL != symbolList) && (NULL != member))
    {
        /* Get the impl. */
        mamaSymbolListImpl *impl = (mamaSymbolListImpl *)symbolList;

        /* To return the removed member. */
        mamaSymbolListMember localMember = NULL;
    
        /* Find the member in the symbol list. */
        ret = mamaSymbolList_findMember(symbolList, symbol, source, transport, &localMember);
        if(MAMA_STATUS_OK == ret)
        {
            /* Remove the member from the list. */
            list_remove_element(impl->myMembers, localMember);
    
            /* Invoke the callback if it has been installed. */
            if(NULL != impl->myRemoveCb)
            {
                impl->myRemoveCb(localMember, impl->myClosure);
            }

            /* Free the member. */
            ret = mamaSymbolListMember_deallocate(localMember);
        }

        /* Return the member reference. */
        *member = localMember;
    }

    return ret;    
}

mama_status mamaSymbolList_removeMemberByRef(mamaSymbolList symbolList, mamaSymbolListMember member)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if((NULL != symbolList) && (NULL != member))
    {
        /* Get the impl. */
        mamaSymbolListImpl *impl = (mamaSymbolListImpl *)symbolList;

        /* Remove the elemebt from the list. */
        list_remove_element (impl->myMembers, (void*)member);

        /* Invoke the remove callback if it has been supplied. */
        if(NULL != impl->myRemoveCb)
        {
            impl->myRemoveCb(member, impl->myClosure);
        }

        /* Deallocate all memory associated with the member. */
        ret = mamaSymbolListMember_deallocate(member);
    }

    return ret;
}

mama_status mamaSymbolList_setAddSymbolHandler(mamaSymbolList symbolList, addSymbolCbType addCb)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != symbolList)
    {
        /* Get the impl. */
        mamaSymbolListImpl *impl = (mamaSymbolListImpl *)symbolList;

        /* Save the callback. */
        impl->myAddCb = addCb;
        
        /* Function succeeded. */
        ret = MAMA_STATUS_OK;        
    }

    return ret;
}

mama_status mamaSymbolList_setClosure(mamaSymbolList symbolList, void *closure)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != symbolList)
    {
        /* Get the impl. */
        mamaSymbolListImpl *impl = (mamaSymbolListImpl *)symbolList;

        /* Save the closure. */
        impl->myClosure = closure;
        
        /* Function succeeded. */
        ret = MAMA_STATUS_OK;        
    }

    return ret;
}

mama_status mamaSymbolList_setRemoveSymbolHandler(mamaSymbolList symbolList, removeSymbolCbType removeCb)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != symbolList)
    {
        /* Get the impl. */
        mamaSymbolListImpl *impl = (mamaSymbolListImpl *)symbolList;

        /* Save the callback. */
        impl->myRemoveCb = removeCb;
        
        /* Function succeeded. */
        ret = MAMA_STATUS_OK;        
    }
    return ret;
}

/* *************************************************** */
/* Private Functions. */
/* *************************************************** */

void mamaSymbolListImpl_removeMember(wList list, void *member, void *closure)
{
    /* Remove this member. */
    mamaSymbolList_removeMemberByRef((mamaSymbolList)closure, (mamaSymbolListMember)member);
}
