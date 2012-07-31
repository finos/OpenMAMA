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

package com.wombat.mamda.examples;

import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.Iterator;
import com.wombat.mamda.*;
import com.wombat.mama.Mama;
import com.wombat.mama.MamaBridge;
import com.wombat.mama.MamaMsg;
import com.wombat.mama.MamaTransport;
import com.wombat.mama.MamaDictionary;
import com.wombat.mama.MamaDictionaryCallback;
import com.wombat.mama.MamaSubscription;
import com.wombat.mama.MamaSource;
import com.wombat.mama.MamaServiceLevel;
import com.wombat.mama.MamaMsgField;
import com.wombat.mama.MamaInteger;
import com.wombat.mama.MamaMsgFieldIterator;

public class MamdaListen
{
    private static Logger theLogger = 
            Logger.getLogger ("com.wombat.mamda.examples");

    private static MamaSource  mMamaSource ;
    private static MamaBridge  mBridge            = null;
    private static String      mDictTransportName = null;

    private static MamaDictionary buildDataDictionary (MamaTransport transport,
                                                       String        dictSource)
        throws InterruptedException
    {
        final boolean gotDict[] = {false};
        MamaDictionaryCallback dictionaryCallback =
            new MamaDictionaryCallback ()
        {
            public void onTimeout ()
            {
                System.err.println
                    ("Timed out waiting for dictionary");
                System.exit (1);
            }

            public void onError (final String s)
            {
                System.err.println ("Error getting dictionary: " + s);
                System.exit (1);
            }

            public synchronized void onComplete ()
            {
                gotDict[0] = true;
                Mama.stop (mBridge);
                notifyAll ();
            }
        };

        synchronized (dictionaryCallback)
        {
            MamaSubscription subscription = new MamaSubscription ();
            
            mMamaSource.setTransport       (transport);
            mMamaSource.setSymbolNamespace (dictSource);

            MamaDictionary   dictionary   = subscription.
                createDictionarySubscription (
                    dictionaryCallback,
                    Mama.getDefaultQueue (mBridge),
                    mMamaSource);
                    
            Mama.start(mBridge);
            if (!gotDict[0]) dictionaryCallback.wait (30000);
            if (!gotDict[0])
            {
                System.err.println( "Timed out waiting for dictionary." );
                System.exit( 0 );
            }
            return dictionary;
        }
    }

    public static void main (final String[] args)
    {
        MamaTransport        transport      = null;
	    MamaTransport	     mDictTransport = null;
        MamaDictionary       dictionary     = null;
        ListenerCallback     callback       = new ListenerCallback ();
        CommandLineProcessor options        = new CommandLineProcessor (args);
        Level level = options.getLogLevel();
        
        try
        {
            if (level != null)
            {
                theLogger.setLevel (level);
                Mama.enableLogging (level);
            }
            // Initialize MAMA
            mBridge = options.getBridge();
            Mama.open ();
            theLogger.info( "Source: " + options.getSource() );

            transport = new MamaTransport ();
            transport.create (options.getTransport (), mBridge);
            mMamaSource = new MamaSource ();

            mDictTransportName = options.getDictTransport();

            if (mDictTransportName != null)
            {
                mDictTransport = new MamaTransport ();
                mDictTransport.create (mDictTransportName, mBridge);
            }
            else 
            {
                mDictTransport = transport;
            }

            // Get the Data dictionary.....
            dictionary = buildDataDictionary (mDictTransport,options.getDictSource());

            // Set the fields
            MamdaQuoteFields.setDictionary  (dictionary, null);
            MamdaTradeFields.setDictionary  (dictionary, null);
            MamdaCommonFields.setDictionary (dictionary, null);

            callback.setDictionary (dictionary);

            for (Iterator iterator = options.getSymbolList ().iterator ();
                 iterator.hasNext ();
                )
            {
                String symbol = (String)iterator.next ();

                MamdaSubscription aSubscription = new MamdaSubscription ();
                
                aSubscription.addMsgListener   (callback);
                aSubscription.addStaleListener (callback);
                aSubscription.addErrorListener (callback);
                if (options.getSnapshot())
                    aSubscription.setServiceLevel (MamaServiceLevel.SNAPSHOT, (short)0);

                aSubscription.create (transport,
                                      Mama.getDefaultQueue (mBridge),
                                      options.getSource (),
                                      symbol,
                                      null);
                
                theLogger.fine ("Subscribed to: " + symbol);
            }
           
            System.out.println ("Ctrl-C to exit.");
            Mama.start (mBridge);
            synchronized (MamdaListen.class)
            {
                MamdaListen.class.wait ();
            }
        }
        catch (Exception e)
        {
            e.printStackTrace();
            System.exit( 1 );
        }
    }

    private static class ListenerCallback implements MamdaMsgListener, 
                                                     MamdaStaleListener,
                                                     MamdaErrorListener,
                                                     MamaMsgFieldIterator
    {
        MamaDictionary mDictionary = null;
        MamaInteger    mfid        = new MamaInteger();

        public void onMsg (
            MamdaSubscription   subscription,
            MamaMsg             msg,
            short               msgType)
        {
            System.out.println ("\nUpdate (" + subscription.getSymbol() + ")");
            msg.iterateFields (this, mDictionary, null);
        }

        public void onStale (
            MamdaSubscription   subscription,
            short               quality)
        {
            System.out.println ("Stale (" + subscription.getSymbol() + ")");
        }

        public void onError (
            MamdaSubscription   subscription,
            short               severity,
            short               errorCode,
            String              errorStr)
        {
            System.out.println("Error (" + subscription.getSymbol() + ")");
        }

        public void onField (
            MamaMsg            msg,
            MamaMsgField       field,
            MamaDictionary     dict,
            Object             closure)  
        {
            System.out.print(field.getName());
            for (int i = 0; i<(25-field.getName().length()); i++)
            {
                System.out.print(" ");
            }

            System.out.print ("|");
            System.out.print (" ");
            System.out.print (field.getFid());
            mfid.setValue    (field.getFid());

            for (int i=0; i<(4-mfid.toString().length()); i++) 
            {
                System.out.print(" ");                
            }

            System.out.print ("|");
            System.out.print (" ");
            System.out.print (field.getTypeName ());

            for (int i=0; i< (7-field.getTypeName().length()); i++) 
            {
                System.out.print (" ");                
            }
            System.out.print ("|");
            System.out.print (field.toString()+" \n");
        }

        public void  setDictionary (
            MamaDictionary dictionary)
        {
            mDictionary = dictionary;
        }    
    }
}
