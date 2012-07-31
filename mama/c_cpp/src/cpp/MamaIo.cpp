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

#include "mama/mamacpp.h"
#include "mamacppinternal.h"
#include "mamainternal.h"

namespace Wombat 
{
	
    class IoTestCallback : public MamaIoCallback
    {
    public:
	    IoTestCallback(MamaIoCallback* ucallback) {mcallback = ucallback;}
        virtual ~IoTestCallback (void) {}
        virtual void onIo (MamaIo*    io,
                           mamaIoType ioType);
    private:
	    MamaIoCallback* mcallback;
    };

    void IoTestCallback::onIo (MamaIo*    io, mamaIoType ioType)
    {
	    try {
		    mcallback->onIo(io, ioType);
	    }
	    catch (...)
	    {
		    fprintf (stderr, "MamaIoCallback onIo EXCEPTION CAUGHT\n");
	    }

    }

    /******************************************************************************
     * MamaIo implementation.
     */
    class MamaIoImpl 
    {
    public:

        MamaIoImpl (void) 
            : mCallback (NULL)
            , mIo       (NULL)
            , mClosure  (NULL) 
        {
        }

        void create (
            MamaQueue*       queue,
            MamaIoCallback*  callback, 
            uint32_t         fd, 
            mamaIoType       ioType,
            void*            closure,
            MamaIo*          cppIo);
        
        virtual ~MamaIoImpl (void) 
        { 
            destroy (); 
        }

        virtual void destroy (void);

        virtual uint32_t  getDescriptor (void) const;

        virtual void* getClosure (void) const 
        {
            return mClosure; 
        }

        MamaIoCallback* mCallback;

    private:
        mamaIo mIo;
        void*  mClosure;
    };
        
    MamaIo::~MamaIo (void) 
    {
        if (mPimpl)
        {
            delete mPimpl;
            mPimpl = NULL;
        }
    }
        
    MamaIo::MamaIo (void)
        : mPimpl (new MamaIoImpl)
    {
    }
        
    void MamaIo::create (
        MamaQueue*        queue,
        MamaIoCallback*   action, 
        uint32_t          descriptor,
        mamaIoType        ioType,
        void*             closure)
    {
        mPimpl->create (queue, action, descriptor, ioType, closure, this);
    }

    uint32_t MamaIo::getDescriptor (void) const
    {
        return mPimpl->getDescriptor ();
    }

    void MamaIo::destroy ()
    {
        mPimpl->destroy ();
    }

    void* MamaIo::getClosure (void) const
    {
        return mPimpl->getClosure ();
    }

    extern "C" 
    {
        static void MAMACALLTYPE mamaIoCb (
            mamaIo io, 
            mamaIoType ioType, 
            void* closure)
        {
            MamaIo *cppIo = static_cast<MamaIo*> (closure);
            cppIo->mPimpl->mCallback->onIo (cppIo, ioType);
        }
    }

    void MamaIoImpl::create (
        MamaQueue*       queue,
        MamaIoCallback*  callback, 
        uint32_t         fd,
        mamaIoType       ioType,
        void*            closure,
        MamaIo*          cppIo)
    {
        mamaQueue qHandle = (queue == NULL) ? NULL :queue->getCValue ();

	    if (mamaInternal_getCatchCallbackExceptions ())
	    {
		    mCallback = new IoTestCallback (callback);
	    }
	    else
	    {
		    mCallback = callback;
	    }

        mClosure  = closure;

        if (mCallback == NULL)
        {
            throw MamaStatus (MAMA_STATUS_INVALID_ARG);
        }

        mamaTry (mamaIo_create (&mIo, qHandle, fd, mamaIoCb, ioType, cppIo));
    }

    void MamaIoImpl::destroy (void)
    {
	    if (mamaInternal_getCatchCallbackExceptions() && (mCallback != NULL))
	    {
		    delete (IoTestCallback*)mCallback;
	    }

        if (mIo)
        {
            mamaTry (mamaIo_destroy (mIo));
            mIo = NULL;
        }
    }

    uint32_t MamaIoImpl::getDescriptor (void) const
    {
        uint32_t result = 0;
       
        if (mIo)
        {
            mamaTry (mamaIo_getDescriptor (mIo, &result));
        }

        return result;
    }

} // namespace Wombat
