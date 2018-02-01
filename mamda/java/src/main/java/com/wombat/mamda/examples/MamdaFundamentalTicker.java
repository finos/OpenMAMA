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
import com.wombat.mama.MamaSource;
import com.wombat.mama.MamaBridge;
import com.wombat.mama.MamaMsg;
import com.wombat.mama.MamaTransport;
import com.wombat.mama.MamaDictionary;
import com.wombat.mama.MamaDictionaryCallback;
import com.wombat.mama.MamaSubscription;

public class MamdaFundamentalTicker
{
    private static Logger theLogger = 
                          Logger.getLogger( "com.wombat.mamda.examples" );
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
                System.exit(1);
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
            MamaSource       mDictSource  = new MamaSource ();
            mDictSource.setTransport       (transport);
            mDictSource.setSymbolNamespace (dictSource);

            MamaDictionary   dictionary   = 
                    subscription.createDictionarySubscription (
                                dictionaryCallback,
                                Mama.getDefaultQueue (mBridge),
                                mDictSource);

            Mama.start(mBridge);
            if (!gotDict[0]) dictionaryCallback.wait (30000);
            if (!gotDict[0])
            {
                System.err.println ("Timed out waiting for dictionary.");
                System.exit (0);
            }
            return dictionary;
        }
    }    

    public static void main (final String[] args)
    {
        MamaTransport        transport      = null;
        MamaTransport	     mDictTransport = null;
        MamaDictionary       dictionary     = null;
        CommandLineProcessor options        = new CommandLineProcessor (args);

        Level level = options.getLogLevel();
        if (level != null)
        {
            theLogger.setLevel (level);
            Mama.enableLogging (level);
        }
        theLogger.info( "Source: " + options.getSource() );

        try
        {
            // Initialize MAMDA
            mBridge = options.getBridge();
            Mama.open ();
            transport = new MamaTransport ();
            transport.create (options.getTransport(), mBridge);

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
            
            MamdaFundamentalFields.setDictionary (dictionary,null);
            MamdaCommonFields.setDictionary      (dictionary, null);

            for (Iterator iterator = options.getSymbolList().iterator();
                 iterator.hasNext();
                )
            {
                final String symbol = (String) iterator.next();
                MamdaSubscription        aSubscription        = new MamdaSubscription ();
                MamdaFundamentalListener aFundamentalListener = new MamdaFundamentalListener();
                FundamentalTicker        aTicker              = new FundamentalTicker();

                aFundamentalListener.addHandler (aTicker);
                aSubscription.addMsgListener    (aFundamentalListener);
                aSubscription.addStaleListener  (aTicker);
                aSubscription.addErrorListener  (aTicker);

                aSubscription.create (transport,
                                      Mama.getDefaultQueue (mBridge),
                                      options.getSource(),
                                      symbol,
                                      null);

                theLogger.fine ("Subscribed to: " + symbol);
            }
            
            Mama.start (mBridge);
            Thread.currentThread ().join ();
        }
        catch (Exception e)
        {
            e.printStackTrace ();
            System.exit (1);
        }
    }

    private static class FundamentalTicker implements MamdaFundamentalHandler,
                                                      MamdaStaleListener,
                                                      MamdaErrorListener
    {
        public void onFundamentals (
            MamdaSubscription         subscription,
            MamdaFundamentalListener  listener,
            MamaMsg                   msg,
            MamdaFundamentals         fundas)
        {
            System.out.println ( "Fundamental update (" + subscription.getSymbol() +
                                 "    wDivFreq: " + fundas.getDividendFrequency() +
                                 "    wMarketSegment: " + fundas.getMarketSegment() +
                                 "    wMarketSector: " + fundas.getMarketSector() + ")");
        }

        public void onStale (
            MamdaSubscription   subscription,
            short               quality)
        {
            System.out.println ("Stale (" + subscription.getSymbol() + "): ");
        }

        public void onError (
            MamdaSubscription   subscription,
            short               severity,
            short               errorCode,
            String              errorStr)
        {
            System.out.println("Error (" + subscription.getSymbol() + "): ");
        }
    }
}
