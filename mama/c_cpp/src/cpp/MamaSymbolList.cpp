/* $Id$
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

#include <mama/MamaSymbolList.h>
#include <mama/symbollist.h>
#include <mama/symbollistmember.h>
#include <mama/MamaSymbolListMember.h>
#include <mamacppinternal.h>
#include <mama/mama.h>
#include <list>

using std::list;

namespace Wombat 
{

    typedef list<MamaSymbolListMembershipHandler*>  MembershipHandlerList;

    struct MamaSymbolList::MamaSymbolListImpl
    {
        MamaSymbolListImpl (MamaSymbolList &owner) 
            : myClosure    (NULL) 
            , myOwner      (owner) 
        {}

        void*                   myClosure;
        MamaSymbolList&         myOwner;
        MembershipHandlerList   myMembershipHandlers;

        void invokeMemberAddHandlers    (MamaSymbolListMember *member);
        void invokeMemberRemoveHandlers (MamaSymbolListMember *member);
        void invokeOrderChangeHandlers  (void);
    };

    class RemoveMemberIteratorHandler: public MamaSymbolListIteratorHandler
    {
    public:
        virtual void onMember (MamaSymbolList&        symbolList,
                               MamaSymbolListMember&  member,
                               void*                  iterateClosure)
        {
            symbolList.removeMember (member);
        }
        virtual void onComplete (MamaSymbolList&        symbolList,
                                 void*                  iterateClosure) 
        {}
    };

    void 
    MamaSymbolList::MamaSymbolListImpl::invokeMemberAddHandlers (MamaSymbolListMember *member)
    {
        MembershipHandlerList::iterator end = myMembershipHandlers.end();
        MembershipHandlerList::iterator i;

        for (i = myMembershipHandlers.begin(); i != end; ++i)
        {
            MamaSymbolListMembershipHandler* handler = *i;
            handler->onMemberAdd (myOwner, *member);
        }
    }

    void 
    MamaSymbolList::MamaSymbolListImpl::invokeMemberRemoveHandlers (MamaSymbolListMember *member)
    {
        MembershipHandlerList::iterator end = myMembershipHandlers.end();
        MembershipHandlerList::iterator i;

        for (i = myMembershipHandlers.begin(); i != end; ++i)
        {
            MamaSymbolListMembershipHandler* handler = *i;
            handler->onMemberRemove (myOwner, *member);
        }
    }

    void 
    MamaSymbolList::MamaSymbolListImpl::invokeOrderChangeHandlers (void)
    {
        MembershipHandlerList::iterator end = myMembershipHandlers.end();
        MembershipHandlerList::iterator i;

        for (i = myMembershipHandlers.begin(); i != end; ++i)
        {
            MamaSymbolListMembershipHandler* handler = *i;
            handler->onOrderChange (myOwner);
        }
    }

    extern "C"
    {

        /**
         * The add callback for the C API 
         */
        static mama_status MAMACALLTYPE 
        addMemberHandler (mamaSymbolListMember member, void *closure)
        {
            MamaSymbolList::MamaSymbolListImpl *impl =
                static_cast<MamaSymbolList::MamaSymbolListImpl*>(closure);  

            /**
             * Sets member's closure to be cppMember. We delete this in remove handler
             */
            MamaSymbolListMember* cppMember = NULL;

            /* If added through C++ API closure will be cpp Wrapper */
            mamaSymbolListMember_getClosure (member, (void**)(&cppMember));
            
            if (cppMember == NULL)
            {
                /* has not been created yet (added through the C API) */
                cppMember = new MamaSymbolListMember (&impl->myOwner, member);
            }

            impl->invokeMemberAddHandlers (cppMember);
            return MAMA_STATUS_OK;
        }

        /**
         * The remove callback for the C API 
         */
        static mama_status MAMACALLTYPE
        removeMemberHandler (mamaSymbolListMember member, void *closure)
        {
            MamaSymbolList::MamaSymbolListImpl *impl =
                static_cast<MamaSymbolList::MamaSymbolListImpl*>(closure);

            MamaSymbolListMember* cppMember;
            mamaSymbolListMember_getClosure (member, (void**)(&cppMember));

            impl->invokeMemberRemoveHandlers (cppMember);

            return MAMA_STATUS_OK;
        }

        /**
         * The orderChange callback for the C API 
         */
        static mama_status MAMACALLTYPE
        orderChangeHandler (mamaSymbolList list, void *closure)
        {
            MamaSymbolList::MamaSymbolListImpl *impl =
                static_cast<MamaSymbolList::MamaSymbolListImpl*>(closure);
            impl->invokeOrderChangeHandlers ();

            return MAMA_STATUS_OK;
        }
    }


    MamaSymbolList::MamaSymbolList()
    {
        myPimpl = new MamaSymbolListImpl(*this);

        mamaSymbolList_allocate (&myList);
        /**
         * We want the Pimpl as the closure here because we need to access its
         * invoke methods in the callbacks.
         */
        mamaSymbolList_setClosure             (myList, myPimpl);
        mamaSymbolList_setAddSymbolHandler    (myList, addMemberHandler);
        mamaSymbolList_setRemoveSymbolHandler (myList, removeMemberHandler);
        //mamaSymbolList_setOrderChangeHandler  (myList, orderChangeHandler);
    }

    MamaSymbolList::~MamaSymbolList()
    {
        if (myPimpl != NULL)
        {
            mamaSymbolList_deallocate (myList); 
            delete myPimpl;
        }

        myPimpl = NULL;
    }

    void MamaSymbolList::addMembershipHandler (MamaSymbolListMembershipHandler* handler)
    { 
        myPimpl->myMembershipHandlers.push_back(handler);
    }

    void MamaSymbolList::addMember (MamaSymbolListMember*  member)
    {
        // Note: the closure to the C implementation is the C++ object.
        mamaTry (mamaSymbolList_addMember (myList, member->getCimpl()));
    }

    MamaSymbolListMember* MamaSymbolList::findMember (const char*     symbol,
                                                      const char *    source,
                                                      mamaTransport   transport)
    {

        mamaSymbolListMember  cMemberImpl = NULL;
        mamaTry (mamaSymbolList_findMember (myList,
                                            symbol,
                                            source,
                                            transport,
                                            &cMemberImpl));
        if (cMemberImpl)
        {
            // Note: the closure to the C implementation is the C++ object.
            MamaSymbolListMember* member = NULL;
            mamaTry (mamaSymbolListMember_getClosure (cMemberImpl,
                                                      (void**)&member));
            return member;
        }
        return NULL;
    }

    MamaSymbolListMember* MamaSymbolList::removeMember (const char*     symbol,
                                                        const char*     source,
                                                        mamaTransport   transport)
    {
        mamaSymbolListMember cMemberImpl = NULL;
        mamaTry (mamaSymbolList_removeMember (myList,
                                              symbol,
                                              source,
                                              transport,
                                              &cMemberImpl));
        if (cMemberImpl)                                          
        {
            //Note: the closure to the C implementation is the C++ object.
            MamaSymbolListMember* member = NULL;
            mamaTry (mamaSymbolListMember_getClosure (cMemberImpl,
                                                      (void**)&member));
            return member;
        }
        return NULL;
    }

    void MamaSymbolList::removeMember (MamaSymbolListMember& member)
    {
        mamaTry (mamaSymbolList_removeMemberByRef (myList, member.getCimpl()));
    }

    void MamaSymbolList::removeMemberAll (void)
    { 
        RemoveMemberIteratorHandler i;
        iterate (i);
    }


    void MamaSymbolList::clear()
    {
        // removeMembersAll should only remove all members off the list,
        // while clear() not only remove them but deallocate them.
        mamaTry (mamaSymbolList_clear (myList, 1));
    }

    void MamaSymbolList::dump()
    {
    //    mamaSymbolList_dump (myList);
    }

    bool MamaSymbolList::empty () const
    {
        unsigned long size;
        mamaTry (mamaSymbolList_getSize (myList, &size));

        if (size == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    mama_size_t MamaSymbolList::size() const
    {
        unsigned long size;
        mamaTry (mamaSymbolList_getSize (myList, &size));
        return size;    
    }

    void MamaSymbolList::setClosure (void*   closure)
    {
        myPimpl->myClosure = closure;
    }

    void* MamaSymbolList::getClosure () const
    {
        return myPimpl->myClosure;
    }

    mamaSymbolList MamaSymbolList::getCValue ()
    {
        return myList;
    }

    const mamaSymbolList MamaSymbolList::getCValue () const
    {
        return myList;
    }

    struct iteratorWrapperParams
    {
        iteratorWrapperParams (MamaSymbolList&                 symbolList,
                               MamaSymbolListIteratorHandler&  handler,
                               void*                           iteratorClosure)
            : mySymbolList      (symbolList)
            , myHandler         (handler)
            , myIteratorClosure (iteratorClosure)
        {}

        MamaSymbolList&                 mySymbolList;
        MamaSymbolListIteratorHandler&  myHandler;
        void*                           myIteratorClosure;
    };

    static void MAMACALLTYPE iterateMember (mamaSymbolList        cSymbolList,
                               mamaSymbolListMember  cMember,
                               void*                 closure)
    {
        iteratorWrapperParams* params = (iteratorWrapperParams*)closure;
        MamaSymbolListMember*  member = NULL;
        mamaSymbolListMember_getClosure (cMember, (void**)&member);

        if (member)
        {
            params->myHandler.onMember (params->mySymbolList,
                                        *member,
                                        params->myIteratorClosure);
        }
    }

    static void MAMACALLTYPE iterateComplete (mamaSymbolList  cSymbolList,
                                 void*           closure)
    {
        iteratorWrapperParams* params = (iteratorWrapperParams*)closure;
        params->myHandler.onComplete (params->mySymbolList,
                                      params->myIteratorClosure);
    }

    void MamaSymbolList::iterate (MamaSymbolListIteratorHandler&  handler,
                                  void*                           iterateClosure)
    {
        iteratorWrapperParams params (*this, 
                                      handler, 
                                      iterateClosure);

        mamaSymbolList_iterate       (myList,
                                      iterateMember,
                                      iterateComplete,
                                      &params);
    }

} // namespace Wombat
