/* $Id: symbollistmember.c,v 1.4.34.3 2011/08/10 14:53:26 nicholasmarriott Exp $
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
#include <mama/symbollist.h>
#include <mama/symbollistmember.h>

/* *************************************************** */
/* Structures. */
/* *************************************************** */

/* This structure contains all the field to implement a symbol list member. */
typedef struct mamaSymbolListMember_
{
    /* User supplied closure. */
    void *myClosure;

    /* The associated source name. */
    char *mySource;

    /* The associated symbol. */
    char *mySymbol;

    /* The symbol list. */
    mamaSymbolList  mySymbolList;

    /* The transport that the associated subscription is running on. */
    mamaTransport   myTransport;

} mamaSymbolListMemberImpl;

/* *************************************************** */
/* Public Functions. */
/* *************************************************** */

mama_status mamaSymbolListMember_allocate(mamaSymbolList symbolList, mamaSymbolListMember *member)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if((NULL != symbolList) && (NULL != member))
    {
        /* Allocate the member using the parent symbol list. */        
        mamaSymbolListMemberImpl *impl = mamaSymbolList_allocateMember(symbolList);
        ret = MAMA_STATUS_NOMEM;
        if(NULL != impl)
        {
            /* Save the symbol list pointer. */
            impl->mySymbolList = symbolList;

            /* Reset all other member variables. */
            impl->myClosure     = NULL;
            impl->mySource      = 0;
            impl->mySymbol      = 0;            
            impl->myTransport   = NULL;

            /* Function succeeded. */
            ret = MAMA_STATUS_OK;            
        }

        /* Return the impl. */
        *member = impl;
    }

    return ret;    
}

mama_status mamaSymbolListMember_deallocate(mamaSymbolListMember member)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != member)
    {
        /* Get the impl. */
        mamaSymbolListMemberImpl *impl = (mamaSymbolListMemberImpl *)member;

        /* Free the strings. */
        if(NULL != impl->mySource)
        {
            free(impl->mySource);
        }
        if(NULL != impl->mySymbol)
        {
            free (impl->mySymbol);
        }

        /* Free the actual structure itself via the parent symbol list. */
        ret =  mamaSymbolList_deallocateMember(impl->mySymbolList, member);        
    }

    return ret;
}

mama_status mamaSymbolListMember_getClosure(const mamaSymbolListMember member, void **closure)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if((NULL != member) && (NULL != closure))
    {
        /* Get the impl. */
        mamaSymbolListMemberImpl *impl = (mamaSymbolListMemberImpl *)member;

        /* Return the closure. */
        *closure = impl->myClosure;
        
        /* Function succeeded. */
        ret = MAMA_STATUS_OK;        
    }

    return ret;
}

int mamaSymbolListMember_getSize(void)
{
    /* Return the size of the symbol list member structure. */
    return sizeof(mamaSymbolListMemberImpl);
}

mama_status mamaSymbolListMember_getSource(const mamaSymbolListMember member, const char **source)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if((NULL != member) && (NULL != source))
    {
        /* Get the impl. */
        mamaSymbolListMemberImpl *impl = (mamaSymbolListMemberImpl *)member;

        /* Return the transport. */
        *source = impl->mySource;
        
        /* Function succeeded. */
        ret = MAMA_STATUS_OK;        
    }

    return ret;
}

mama_status mamaSymbolListMember_getSymbol(const mamaSymbolListMember member, const char **symbol)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if((NULL != member) && (NULL != symbol))
    {
        /* Get the impl. */
        mamaSymbolListMemberImpl *impl = (mamaSymbolListMemberImpl *)member;

        /* Return the transport. */
        *symbol = impl->mySymbol;
        
        /* Function succeeded. */
        ret = MAMA_STATUS_OK;        
    }

    return ret;
}

mama_status mamaSymbolListMember_getSymbolList(const mamaSymbolListMember member, mamaSymbolList *symbolList)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if((NULL != member) && (NULL != symbolList))
    {
        /* Get the impl. */
        mamaSymbolListMemberImpl *impl = (mamaSymbolListMemberImpl *)member;

        /* Return the transport. */
        *symbolList = impl->mySymbolList;
        
        /* Function succeeded. */
        ret = MAMA_STATUS_OK;        
    }

    return ret;
}

mama_status mamaSymbolListMember_getTransport(const mamaSymbolListMember member, mamaTransport *transport)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if((NULL != member) && (NULL != transport))
    {
        /* Get the impl. */
        mamaSymbolListMemberImpl *impl = (mamaSymbolListMemberImpl *)member;

        /* Return the transport. */
        *transport = impl->myTransport;
        
        /* Function succeeded. */
        ret = MAMA_STATUS_OK;        
    }

    return ret;
}

mama_status mamaSymbolListMember_setClosure(mamaSymbolListMember member, void *closure)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != member)
    {
        /* Get the impl. */
        mamaSymbolListMemberImpl *impl = (mamaSymbolListMemberImpl *)member;

        /* Save the closure. */
        impl->myClosure = closure;
        
        /* Function succeeded. */
        ret = MAMA_STATUS_OK;        
    }

    return ret;
}

mama_status mamaSymbolListMember_setSource(mamaSymbolListMember member, const char *source)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if((NULL != member) && (NULL != source))
    {
        /* Get the impl. */
        mamaSymbolListMemberImpl *impl = (mamaSymbolListMemberImpl *)member;

        /* Free any existing symbol. */
        if(NULL != impl->mySource)
        {
            free(impl->mySource);
        }

        /* Copy the supplied symbol. */
        ret = MAMA_STATUS_NOMEM;
        impl->mySource = strdup(source);
        if(NULL != impl->mySource)
        {
            ret = MAMA_STATUS_OK;
        }
    }

    return ret;
}

mama_status mamaSymbolListMember_setSymbol(mamaSymbolListMember member, const char *symbol)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if((NULL != member) && (NULL != symbol))
    {
        /* Get the impl. */
        mamaSymbolListMemberImpl *impl = (mamaSymbolListMemberImpl *)member;

        /* Free any existing symbol. */
        if(NULL != impl->mySymbol)
        {
            free(impl->mySymbol);
        }

        /* Copy the supplied symbol. */
        ret = MAMA_STATUS_NOMEM;
        impl->mySymbol = strdup(symbol);
        if(NULL != impl->mySymbol)
        {
            ret = MAMA_STATUS_OK;
        }
    }

    return ret;
}

mama_status mamaSymbolListMember_setSymbolList(mamaSymbolListMember member, mamaSymbolList symbolList)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != member)
    {
        /* Get the impl. */
        mamaSymbolListMemberImpl *impl = (mamaSymbolListMemberImpl *)member;

        /* Save the symbol list. */
        impl->mySymbolList = symbolList;
        
        /* Function succeeded. */
        ret = MAMA_STATUS_OK;        
    }

    return ret;
}

mama_status mamaSymbolListMember_setTransport(mamaSymbolListMember member, mamaTransport transport)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != member)
    {
        /* Get the impl. */
        mamaSymbolListMemberImpl *impl = (mamaSymbolListMemberImpl *)member;

        /* Save the transport. */
        impl->myTransport = transport;
        
        /* Function succeeded. */
        ret = MAMA_STATUS_OK;        
    }

    return ret;
}

