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

import com.wombat.mama.*;

/**
 * MamdaMultiParticipantManager is a class that manages updates on a
 * consolidated basis for securities that may be traded on multiple
 * exchanges and which may have a national best bid and offer.
 * Developers are notified of each element available for the
 * consolidated security, including the national best bid and offer,
 * and each regional exchange (Market Maker).  Developers can pick
 * and choose which elements they wish to provide handling for 
 * (e.g., BBO-only, certain regional exchanges, etc.).
 *
 * Specialized Listeners can be added to the
 * <code>MamdaMultiParticipantManager</code> in response to the callbacks
 * on the <code>MamdaMultiParticipantHandler</code> being invoked. 
 * Alternatively, the Listeners can be added up front if the participants
 * are known in advance.
 *
 * Note: the <code>MamdaMultiParticipantManager</code> can also be used for
 * securities that are not traded on multiple exchanges.
 */

public class MamdaMultiParticipantManager implements MamdaMsgListener
{
    private static Logger mLogger =
              Logger.getLogger(MamdaMultiParticipantManager.class.getName());

    private boolean mNotifiedConsolidatedCreate    =   false;
    /* List of registered listeners for consolidated updates */
    private List    mConsolidatedListeners         =   null;
    /* Map of registered participants. Keyed on the participant Id */
    private Map     mParticipants                  =   null;
    /* List of MultiParticipantHandlers for creation callback notification */
    private List    mHandlers                      =   null;
    private String  mSymbol                        =   null;

    /* Not currently used... */
    private MamaBoolean mIsPrimaryParticipant = null;

    MamaString tempPartID = new MamaString();
    MamaString tempSymbol = new MamaString();
        
    /**
     * Only constructor for the class. No default available.
     *
     * @param symbol The group symbol for which the corresponding subscription
     *               was created.
     */
    public MamdaMultiParticipantManager (String symbol)
    {
        this.mNotifiedConsolidatedCreate   =   false;
        this.mConsolidatedListeners        =   new ArrayList ();
        this.mParticipants                 =   new HashMap ();
        this.mHandlers                     =   new ArrayList ();
        this.mSymbol                       =   symbol;
        this.mIsPrimaryParticipant         =   new MamaBoolean (true);
    }
    
    /**
     * Add a specialized handler for notifications about the
     * multi-participant security.  
     *
     * @param handler Concrete instance of the MamdaMultiParticipantHandler
     *                interface.
     */
    public void addHandler (MamdaMultiParticipantHandler  handler)
    {
        mHandlers.add (handler);
    }

    /**
     * Add a specialized message listener (e.g., a MamdaQuoteListener,
     * MamdaTradeListener, etc.) for the consolidated data.
     *
     * Multiple listeners can be added.
     *
     * @param listener Concrete instance of the MamdaMsgListener interface.
     */
    public void addConsolidatedListener (MamdaMsgListener  listener)
    {
        mConsolidatedListeners.add (listener);
    }

    /**
     * Add a specialized message listener (e.g., a MamdaQuoteListener,
     * MamdaTradeListener, etc.) for a participant.
     *
     * Multiple listeners for each participant can be added.
     *
     * @param listener Concrete instance of the MamdaMsgListener interface.
     * @param partId The participant id for the instrument. This is suffix
     *               for the symbol in NYSE Technologies symbology.
     */
    public void addParticipantListener (MamdaMsgListener  listener,
                                        String            partId)
    {
        ParticipantInfo  participantInfo = getParticipantInfo (partId);
        participantInfo.mParticipantListeners.add (listener);
    }

    /**
     * Implementation of the MamdaMsgListener Interface.
     */
    public void onMsg (final MamdaSubscription subscription,
                       final MamaMsg           msg,
                       final short             msgType)
    {
        mLogger.fine("MamdaMultiParticipantManager: got msg type " +
                       msgType);
        try
        {
            switch (msgType)
            {
                case MamaMsgType.TYPE_INITIAL:
                case MamaMsgType.TYPE_RECAP:
                    // First handle the message internally. I.e. create listener
                    // lists, notify handlers etc.
                    handleFullImage (subscription, msg, msgType, getPartId (msg));
                    break;
                case MamaMsgType.TYPE_END_OF_INITIALS:/*Don't forward.*/
                    return;
                default:
                    break;
            }

            // Now pass to other listerners (if any)
            forwardMsg (subscription, msg, msgType, getPartId (msg));
        }
        catch (MamdaDataException ex)
        {
            mLogger.fine ("MamdaMultiParticipantManager: " +
                           "caught MamdaDataException:" + ex.getMessage());
        }
    }

    /*
    * Process initial images. This is most likely when we will be 
    * notify of consolidate/participant creation.
    */
    private void handleFullImage (MamdaSubscription subscription,
                                  MamaMsg           msg,
                                  short             msgType,
                                  String            partId)
    {
        mLogger.fine ("MamdaMultiParticipantManager: full image for "
                + subscription.getSymbol () + "(" + partId + ")");

        if (null == partId)/*This is a consolidated initial*/
        {
             checkConsolidatedCreateNotify(subscription);
        }
        else /*This is a participant initial*/
        {
            ParticipantInfo partInfo = getParticipantInfo (partId);
            checkParticipantCreateNotify (subscription,partId,partInfo);
        }
    }

    /*
    * Returns the participant info object for the specified participant.
    * If the info object is not already in the map one will be created and
    * added to the map.
    */
    private ParticipantInfo getParticipantInfo (String partId)
    {
        ParticipantInfo partInfo = (ParticipantInfo)mParticipants.get (partId);
        if (null == partInfo)
        {
            partInfo = new ParticipantInfo ();
            this.mParticipants.put (partId, partInfo);
        }
        return partInfo;
    }
    
    private void checkParticipantCreateNotify (MamdaSubscription subscription,
                                               String            partId,
                                               ParticipantInfo   partInfo)
    {
        /*
           Check if we need to notify of participant creation. A user may have
           added a listener up front so the PartInfo will exist but we will
           not have already notified them of the first update for this
           participant.
         */
        if (!partInfo.mNotifiedPartCreate) 
        {
            Iterator itr = mHandlers.iterator ();
            while (itr.hasNext ())
            {
                MamdaMultiParticipantHandler handler =
                    (MamdaMultiParticipantHandler)itr.next ();
                handler.onParticipantCreate (subscription, this,
                                             partId, mIsPrimaryParticipant);
            }
            partInfo.mNotifiedPartCreate = true;
        }
    }

    /*
    * Checks whether we need to notify registered handlers of the fist
    * consolidated update for this symbol
    */
    private void checkConsolidatedCreateNotify (MamdaSubscription    subscription)
    {
        if (!mNotifiedConsolidatedCreate)
        {
            /*Only invoke handler onCreate if no listeners have been
             * registered*/
            Iterator itr = mHandlers.iterator ();
            while (itr.hasNext ())
            {
                MamdaMultiParticipantHandler handler =
                    (MamdaMultiParticipantHandler)itr.next ();
                handler.onConsolidatedCreate (subscription, this);
            }
            mNotifiedConsolidatedCreate = true;
        }
    }
    
    private void forwardMsg (MamdaSubscription  subscription,
                             MamaMsg            msg,
                             short              msgType,
                             String             partId)
    {
        if (null!=partId) /*Participant message*/
        {
            /* We may not have an entry for this partId at this stage as
               we may not be receiving initial values for the subscription. */
            ParticipantInfo partInfo = getParticipantInfo (partId);

            /* Need to check if we have notified. We may well not be receiving
             * initials*/
            checkParticipantCreateNotify (subscription, partId, partInfo);

            /* We may have a ParticipantInfo entry for the partId but there is
               still no guarantee that any listeners have been registered. 
               This is quite acceptable as users may not be interested in all
               participants. */
            if (0 != partInfo.mParticipantListeners.size())
            {
                forwardMsg (partInfo.mParticipantListeners, subscription,
                            msg, msgType);
            }
        }
        else /*Consolidated message*/
        {
            /* We may not have notified the user that a consolidated message
               has arrived. If not using initials this can be the case.*/
            checkConsolidatedCreateNotify (subscription);

            /* Forward the message if we have any listeners*/
            if (0!=mConsolidatedListeners.size ())
            {   
                forwardMsg (mConsolidatedListeners,
                            subscription, msg, msgType);
            }
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

    /*
    * Get the participant Id from the message.
    *
    * @param msg The MamaMsg instance
    * @return String The participant id if it exists or null if this is a
    *                consolidated symbol
    * @throws MamdaDataException If the symbol contains a dot but the part id
    *                            cannot be extracted.
    */
    private String getPartId (MamaMsg msg)
    {
        tempPartID.setValue(null);
        tempSymbol.setValue(null);
        if (msg.tryString (MamdaCommonFields.PART_ID, tempPartID))
        {
            if (!tempPartID.getValue().equals(""))
            {
                return tempPartID.getValue();
            }
        }
        
        if (!msg.tryString (MamdaCommonFields.ISSUE_SYMBOL, tempSymbol))
        {
            if (!msg.tryString (MamdaCommonFields.INDEX_SYMBOL, tempSymbol))
            {
                if (!msg.tryString (MamdaCommonFields.SYMBOL, tempSymbol))
                {
                    mLogger.finest ("MamdaMultiParticipantManager: " +
                                    "getPartId failed to extract participant ID: no symbol.");
                    return null;
                }
            }
        }
        
        int lastDotIndex = tempSymbol.getValue().lastIndexOf ('.');
        if (lastDotIndex!=-1)
        {
            lastDotIndex++;
            if (lastDotIndex != tempSymbol.getValue().length ())
            {
                return  tempSymbol.getValue().substring (lastDotIndex);
            }
        }

        mLogger.finest ("MamdaMultiParticipantManager: " +
                        "getPartId() failed to extract participant ID: no dot.");
        return null;
    }

    /*
       This container class is used to store per-participant state
       information.
    */
    private class ParticipantInfo
    {
        /*The list of listeners for this participant*/
        public List mParticipantListeners     = new ArrayList ();
        /*Whether we have notified the handler via a
         call to onParticipantCreate*/
        public boolean mNotifiedPartCreate    = false;
    }

}//end class
