/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2012 NYSE Technologies, Inc.
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

package com.wombat.mamda;

import java.util.*;
import java.util.logging.Logger;

import com.wombat.mama.MamaMsg;
import com.wombat.mama.MamaMsgType;
import com.wombat.mama.MamaBoolean;

/**
 * MamdaMultiSecurityManager is a class that manages updates on a
 * group symbol which provides a single subscription to multiple
 * different securities.
 * Developers are notified of each element available for the group and can
 * choose which elements they wish to provide handling for 
 * (e.g. based on wildcards).
 *
 * Specialized Listeners can be added to the
 * <code>MamdaMultiSecurityManager</code> in response to the callbacks
 * on the <code>MamdaMultiSecurityHandler</code> being invoked. 
 * Alternatively, the Listeners can be added up front if the participants
 * are known in advance.
 */

public class MamdaMultiSecurityManager implements MamdaMsgListener
{
    private static Logger mLogger =
              Logger.getLogger(MamdaMultiSecurityManager.class.getName());

    /* Map containing a list of listeners for each security. 
      Keyed on the symbol */
    private Map     mListeners                    =   null;

    /* List of MultiSecurityHandlers for creation callback notification */
    private List    mHandlers                     =   null;

    /* Group symbol subscribed to */
    private String  mSymbol                       =   null;

    /**
     * Only constructor for the class. No default available.
     *
     * @param symbol The group symbol for which the corresponding subscription
     *               was created.
     */
    public MamdaMultiSecurityManager (String symbol)
    {
        this.mListeners                    =   new HashMap ();
        this.mHandlers                     =   new ArrayList ();
        this.mSymbol                       =   symbol;
    }
    
    /**
     * Add a specialized handler for notifications about new 
     * securities.  
     *
     * @param handler Concrete instance of the MamdaMultiSecurityHandler
     *                interface.
     */
    public void addHandler (MamdaMultiSecurityHandler  handler)
    {
        mHandlers.add (handler);
    }

    /**
     * Add a specialized message listener (e.g. a MamdaQuoteListener,
     * MamdaTradeListener, etc.) for a security.
     *
     * Multiple listeners for each security can be added.
     *
     * @param listener Concrete instance of the MamdaMsgListener interface.
     * @param symbol The symbol for the instrument. 
     */
    public void addListener (MamdaMsgListener  listener,
                             String            symbol)
    {
        ((ArrayList)mListeners.get(symbol)).add(listener);
    }

    /**
     * Implementation of the MamdaMsgListener Interface.
     */
    public void onMsg (final MamdaSubscription subscription,
                       final MamaMsg           msg,
                       final short             msgType)
    {
    
        mLogger.fine("MamdaMultiSecurityManager: got msg type " +
                       msgType);
        try
        {
            String symbol = msg.getString (MamdaCommonFields.ISSUE_SYMBOL);
            if(symbol == null)
            {
                return;
            }
            // First handle the message internally. I.e. create listener
            // lists, notify handlers etc.
            switch (msgType)
            {
                case MamaMsgType.TYPE_INITIAL:
                case MamaMsgType.TYPE_RECAP:
                    handleFullImage (subscription, msg, msgType, symbol);
                    break;
                case MamaMsgType.TYPE_END_OF_INITIALS:/*Don't forward.*/
                    return;
                default:
                    break;
            }
            // Now pass to other listeners (if any)
            forwardMsg (subscription, msg, msgType, symbol);
        }
        catch (MamdaDataException ex)
        {
            mLogger.fine ("MamdaMultiSecurityManager: " +
                           "caught MamdaDataException:" + ex.getMessage());
        }
    }

    /*
    * Process initial images. This is most likely when we will be 
    * notify of the creation of a new security.
    */
    private void handleFullImage (MamdaSubscription subscription,
                                  MamaMsg           msg,
                                  short             msgType,
                                  String            symbol)
    {
        mLogger.fine ("MamdaMultiSecurityManager: full image for "
                + symbol);

        if(!mListeners.containsKey (symbol))
        {
            checkSecurityCreateNotify (subscription,symbol);
        }
    }

    private void checkSecurityCreateNotify (MamdaSubscription subscription,
                                            String            symbol)
    {
        mListeners.put(symbol,new ArrayList());
        Iterator itr = mHandlers.iterator ();
        while (itr.hasNext ())
        {
            MamdaMultiSecurityHandler handler =
                (MamdaMultiSecurityHandler)itr.next ();
            handler.onSecurityCreate (subscription, this, symbol);
        }
    }

    private void forwardMsg (MamdaSubscription  subscription,
                             MamaMsg            msg,
                             short              msgType,
                             String             symbol)
    {
        ArrayList securityListeners = (ArrayList)mListeners.get(symbol);
        if ((securityListeners != null)&&(securityListeners.size() > 0))
        {
            forwardMsg (securityListeners, subscription,
                        msg, msgType);
        }
        else
        {
            /*Needed for new symbols created intraday*/
            checkSecurityCreateNotify (subscription,symbol);
        }
    }

    /*
    * Generic method for dispatching messages to instances of
    * MamaMsgListeners.
    */
    private void forwardMsg (
            List                listeners,
            MamdaSubscription   subscription,
            MamaMsg             msg,
            short               msgType)
    {
        Iterator itr  = listeners.iterator ();
        while (itr.hasNext ())
        {
            MamdaMsgListener listener = (MamdaMsgListener)itr.next ();
            listener.onMsg (subscription, msg, msgType);
        }
    }

}//end class
