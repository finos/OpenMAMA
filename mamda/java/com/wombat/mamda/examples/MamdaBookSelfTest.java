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

//
// The purpose of this example is to illustrate use of the
// MamdaOrderBookChecker utility class, which periodically takes a
// snapshot of an order book that is also being managed by a
// MamdaOrderBookListener in real time.
//

package com.wombat.mamda.examples;

import java.util.logging.Level;
import java.util.logging.Logger;
import com.wombat.mamda.orderbook.*;
import com.wombat.mamda.*;
import com.wombat.mama.MamaDictionary;
import com.wombat.mama.MamaDictionaryCallback;
import com.wombat.mama.MamaSubscription;
import com.wombat.mama.MamaTransport;
import com.wombat.mama.MamaSource;
import com.wombat.mama.MamaMsg;
import com.wombat.mama.Mama;
import com.wombat.mama.MamaBridge;
import java.util.Iterator;

public class MamdaBookSelfTest
{
    private static MamaSource  mSource      = null;
    private static Level       mLogLevel    = Level.INFO;
    private static MamaBridge  mBridge      = null;
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
                    System.err.println ("Timed out waiting for dictionary");
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
             mSource.setTransport (transport);
             mSource.setSymbolNamespace (dictSource);
             
             MamaDictionary dictionary = 
                subscription.createDictionarySubscription (
                                                dictionaryCallback,
                                                Mama.getDefaultQueue (mBridge),
                                                mSource);
             
             Mama.start (mBridge);
             if (!gotDict[0]) dictionaryCallback.wait (30000);
             if (!gotDict[0])
             {
                 System.err.println ("Timed out waiting for dictionary.");
                 System.exit (0);
             }
             return dictionary;
         }
    }

    private static class BookSelfTest implements MamdaOrderBookCheckerHandler
    {
        public void onSuccess (MamdaOrderBookCheckType  checkType,
                               final MamdaOrderBook     realTimeBook)
        {
            //Successfull Check
            System.out.println ("Successful check (" + checkType.toString() +")");      
        }
    
        public void onInconclusive (MamdaOrderBookCheckType  checkType,
                                final String             reason)
        {
            //Inconclusive Check
            System.out.println ("OnInconclusive Check (" + checkType.toString() + reason + ")");
        }
    
        public void onFailure (MamdaOrderBookCheckType  checkType,
                               final String             reason,
                               final MamaMsg            msg,
                               final MamdaOrderBook     realTimeBook,
                               final MamdaOrderBook     checkBook)
        {
            //Failed Check
            System.out.println ("Failure Check (" + checkType.toString() + reason + ")");
            System.out.println ("msg: " + msg.toString());
            System.out.println ("Failed Current Book: ");  
            realTimeBook.dump();
            System.out.println ("Failed Checking Book: ");  
            checkBook.dump();
        }
    }

    public static void main (final String[] args)
    {
        CommandLineProcessor options      = new CommandLineProcessor (args);
        MamaTransport        transport    = null;
        MamaDictionary       dictionary   = null;
        double               intervalSecs = options.getTimerInterval ();
        mLogLevel                        = options.getLogLevel();
        
        if (mLogLevel != null)
        {
            Mama.enableLogging (mLogLevel);
        }
        
        try
        { 
            mBridge = options.getBridge();
            Mama.open ();     
            transport = new MamaTransport();
            transport.create (options.getTransport (), mBridge);
            mSource = new MamaSource ();
            
            // We might as well also enforce strict checking of order book updates
            MamdaOrderBook.setStrictChecking (true);
            
            /*Get the Data Dictionary*/
            dictionary = buildDataDictionary (transport,"WOMBAT");
            MamdaOrderBookFields.setDictionary (dictionary, null);
            MamdaCommonFields.setDictionary (dictionary, null);

            if (intervalSecs == 0)
            {
                intervalSecs = 5;
            }
            
            for (Iterator iterator = options.getSymbolList().iterator();
                 iterator.hasNext();
                )
            {
                final String symbol                = (String)iterator.next ();
                BookSelfTest aSelfTest             = new BookSelfTest ();
                MamdaOrderBookChecker aBookChecker = new MamdaOrderBookChecker (transport,
                                                                                Mama.getDefaultQueue (mBridge),
                                                                                aSelfTest,
                                                                                options.getSource(),
                                                                                symbol,
                                                                                intervalSecs);
            }
            Mama.start(mBridge);
        }
        catch (Exception ex)
        {
            ex.printStackTrace ();
            System.exit(1);
        }
    }
}

