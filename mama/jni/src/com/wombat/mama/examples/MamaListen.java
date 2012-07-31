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

package com.wombat.mama.examples;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.logging.Logger;
import java.util.logging.Level;
import java.io.*;

import com.wombat.mama.*;

/**
 * MamaListen - Generic Mama API subscriber.
 *
 * <pre>
 * Illustrates the following MAMA API features:
 *   1. Data dictionary initialization.
 *   2. Examining standard fields (e.g. MdMsgType, MdMsgStatus).
 *   3. Walking all received fields in messages.
 *   4. Picking specific fields received in messages.
 *
 * Examples:
 *
 * 1. Display data dictionary with dictionary requested from symbolNamespace
 *    "ORDER" (default dictionary symbolNamespace is "WOMBAT" and used in
 *    subsequent examples, below).
 *
 *    java com.wombat.mama.MamaListen -D -d ORDER -s MSFT.ISLD
 *
 * 2. Subscribing to top-of-book MSFT quote from Island ITCH feed (all
 *    fields) where the feed handler symbolNamespace is SOURCE:
 *
 *    java com.wombat.mama.MamaListen -S SOURCE -s MSFT.ISLD
 *
 * 3. Subscribing to top-of-book MSFT & ORCL from Island ITCH feed
 *    (specific fields):
 *
 *    java com.wombat.mama.MamaListen -s MSFT.ISLD -s ORCL.ISLD wBidPrice wBidSize wAskPrice wAskSize wTradePrice wTradeVolume wTotalVolume
 *
 * 4. Subscribing to a list of symbols from a file (all fields):
 *
 *    java com.wombat.mama.MamaListen -S WOMABT -f file
 * <pre>
 */
public class MamaListen
{
    private static MamaTransport    transport;
    private static MamaTransport    myDictTransport;
    private static String           myMiddleware        = "wmw";
    private static MamaBridge       myBridge            = null;
    private static MamaQueue        myDefaultQueue      = null;
    private static long             myThrottleRate            = -1;
    private static long             myRecapThrottleRate       = -1;

    private static long             myHighWatermark     = 0;
    private static long             myLowWatermark     = 0;

    private static MamaDictionary   dictionary;
    private static String           dictSource          = "WOMBAT";
    private static boolean          dumpDataDict        = false;
    private static final ArrayList  fieldList           = new ArrayList();
    private static boolean          dictionaryComplete  = false;

    private static String           transportName       = "internal";
    private static String 	        myDictTportName	= null;
    private static boolean          requireInitial      = true;
    private static boolean          snapshot            = false;
    private static double           timeout             = 10.0;
    private static int              quietness           = 0;

    private static boolean          printFromMessage    = false;

    private static int              numThreads          = 0;
    private static MamaQueueGroup   queueGroup          = null;

    private static boolean          qualityForAll       = true;

    private static final ArrayList  subscriptions       = new ArrayList();
    private static final ArrayList  subjectList         = new ArrayList();
    private static       String     filename            = null;
    private static int              retryAttempts       = MamaSubscription.DEFAULT_RETRIES;

    private static String           mySymbolNamespace   = null;

    private static final SubscriptionCallback   
        callback            = new SubscriptionCallback();

    private static final Logger     logger              = 
                                    Logger.getLogger( "com.wombat.mama" );
    private static Level            myLogLevel;
    private static boolean          myGroupSubscription = false;
    private static MamaSource       mySource            = null;   
    private static MamaSource       myDictSource        = null;

    /* Contains the amount of time that the example program will run for, if set to 0 then it
     * will run indefinitely.
     */
    private static int myShutdownTime = 0;

    private static boolean newIterators = false; 
    public static void main (final String[] args)
    {
        parseCommandLine (args);

        try
        {
            if (subjectList.isEmpty())
            {
                readSubjectsFromFile();
            }

            initializeMama ();
            buildDataDictionary ();
            dumpDataDictionary ();
            subscribeToSubjects ();
            
            System.out.println( "Type CTRL-C to exit." );
            
            // Create the shutdown timer, note that it will be destroyed in the callback            
            if(myShutdownTime > 0)
            {
				// Create a new callback object
                ShutdownTimerCallback timerCallback = new ShutdownTimerCallback(myBridge, queueGroup);
				
				// Create the timer
				MamaTimer shutdownTimer = new MamaTimer();
                shutdownTimer.create(myDefaultQueue, timerCallback, (double)myShutdownTime);
            }
                        
            Mama.start (myBridge);
        }
        catch (Exception e)
        {
            if (e.getCause() != null)
            {
                e.getCause ().printStackTrace ();
            }

            e.printStackTrace ();
            System.exit (1);
        }
        finally
        {
            shutdown ();
        }
    }

    private static void setQueueMonitor (MamaQueueGroup queueGroup,
                                         int            numThreads)
    {
        /* Check if queue monitoring has been enabled*/
        if (myHighWatermark>0 || myLowWatermark>0)
        {            
            for (int i=0;i<numThreads;i++)
            {

                MamaQueue queue = queueGroup.getNextQueue();

                queue.setQueueName("QUEUE-"+i);
                
                
                
                queue.setQueueMonitorCallback (new MamaQueueMonitorCallback () 
                {
                        public void onHighWatermarkExceeded (MamaQueue queue,
                            long      size)
                        {
                            System.out.println ("High WM exceeded for: " +
                                                queue.getQueueName() + " size: " +
                                                size);
                        }

                        public void onLowWatermark (MamaQueue queue,
                            long      size)
                        {

                            System.out.println ("Low WM hit for: " +
                                                queue.getQueueName() + " size: " +
                                                size);
                        }

                    });

                if (myHighWatermark>0)
                {
                    queue.setHighWatermark (myHighWatermark);
                }

                if (myLowWatermark>0)
                {
                    /*Only supported on Wombat TCP middleware*/
                    try
                    {
                        queue.setLowWatermark (myLowWatermark);
                    }
                    catch (Exception ex)
                    {
                        System.out.println (ex);
                    }
                }
            }
        }
    }

    private static void subscribeToSubjects()
    {
        int howMany = 0;

        queueGroup = new MamaQueueGroup (numThreads, myBridge);

        try 
        {
            setQueueMonitor (queueGroup, numThreads);
        }
        catch (Exception ex)
        {
            System.out.println (ex);
        }

        /*Subscribe to all symbols specified on the command line or from the
          symbol file*/
        for (Iterator iterator = subjectList.iterator(); iterator.hasNext();)
        {
            final String symbol = (String) iterator.next();
            
            MamaSubscription subscription = new MamaSubscription ();
           
            /*Properties common to all subscription types*/
            subscription.setTimeout (timeout);
            subscription.setRetries (retryAttempts);

            if (snapshot)
            {
                subscription.setServiceLevel (MamaServiceLevel.SNAPSHOT,0);
            }

            if (myGroupSubscription)
            {
                subscription.setSubscriptionType (MamaSubscriptionType.GROUP);
            }

            subscription.setRequiresInitial (requireInitial);
            subscription.createSubscription (
                callback,
                queueGroup.getNextQueue (),
                mySource,
                symbol,
                null);

            logger.fine ("Subscribed to: " + symbol);

            if (++howMany % 1000 == 0)
            {
                System.out.println ("Subscribed to " + howMany + " symbols.");
            }
        }
    }

    private static void dumpDataDictionary()
    {
        if (dumpDataDict)
        {
            for (int i = 0; i < dictionary.getSize(); i++)
            {
                MamaFieldDescriptor fd = dictionary.getFieldByIndex (i);
                print (""+fd.getFid(),5);
                System.out.print (": ");
                print (""+fd.getType(),3);
                System.out.println (": "+fd.getName());
            }
        }
    }

    private static void buildDataDictionary()
        throws InterruptedException
    {
        MamaDictionaryCallback dictionaryCallback = createDictionaryCallback();

        synchronized (dictionaryCallback)
        {
            /*The dictionary is obtained through a specialized form of
              subscription.*/
            MamaSubscription subscription = new MamaSubscription ();

            dictionary = subscription.createDictionarySubscription (
                    dictionaryCallback,
                    myDefaultQueue,
                    myDictSource,
                    10.0,
                    2);
            Mama.start (myBridge);
            if (!dictionaryComplete) dictionaryCallback.wait( 30000 );
            if (!dictionaryComplete)
            {
                System.err.println( "Timed out waiting for dictionary." );
                System.exit( 0 );
            }
        }
    }

    private static MamaDictionaryCallback createDictionaryCallback()
    {
        return new MamaDictionaryCallback()
        {
            public void onTimeout()
            {
                System.err.println( "Timed out waiting for dictionary" );
                System.exit(1);
            }

            public void onError (final String s)
            {
                System.err.println ("Error getting dictionary: " + s);
                System.exit (1);
            }

            public synchronized void onComplete()
            {
                dictionaryComplete = true;
                Mama.stop(myBridge);
                notifyAll();
            }
        };
    }

    private static void shutdown()
    {
        try
        {
            // Destroy all the subscriptions. */
        for (Iterator iterator = subscriptions.iterator(); iterator.hasNext();)
        {
            final MamaSubscription subscription = (MamaSubscription) iterator.next();

                try
                {
            subscription.destroy();
        }

                catch(Throwable t)
                {
                    t.printStackTrace();
                }
            }
            
            /* Destroy all the queues. */
            if((queueGroup != null) && (numThreads > 0))
            {
                queueGroup.destroyWait();
            }

            /* Destroy the transport. */
        if (transport != null)
        {
            transport.destroy();
        }

            // Perform remaining cleanup
        Mama.close();
    }

        catch(Throwable ex)
        {
            ex.printStackTrace();
        }
    }

    private static void initializeMama()
    {
        try
        {
            myBridge = Mama.loadBridge (myMiddleware);
            /*Always the first API method called after loadBridge. 
              Initialized internal API state*/
            Mama.open ();
            myDefaultQueue = Mama.getDefaultQueue (myBridge);
        }
        catch (Exception e)
        {
            e.printStackTrace ();
            System.out.println ("Failed to initialize MAMA");
            System.exit (1);
        }
        
        transport = new MamaTransport ();

	if (myDictTportName != null)
        {
            myDictTransport = new MamaTransport ();
            myDictTransport.create (myDictTportName, myBridge);
        }else {
            myDictTransport = transport;
        }

        /*Receive notification of transport level events*/
        transport.addTransportListener( new MamaTransportListener()
        {
            public void onConnect(short cause, final Object platformInfo)
            {
                System.out.println ("TRANSPORT CONNECTED!");
            }

            public void onDisconnect(short cause, final Object platformInfo)
            {
                System.out.println ("TRANSPORT DISCONNECTED!");
            }

            public void onReconnect(short cause, final Object platformInfo)
            {
                System.out.println ("TRANSPORT RECONNECTED!");
            }

            public void onPublisherDisconnect(short cause, final Object platformInfo)
            {
                System.out.println ("PUBLISHER DISCONNECTED!");
            }

            public void onAccept(short cause, final Object platformInfo)
            {
                System.out.println ("TRANSPORT ACCEPTED!");
            }

            public void onAcceptReconnect(short cause, final Object platformInfo)
            {
                System.out.println ("TRANSPORT RECONNECT ACCEPTED!");
            }

            public void onNamingServiceConnect(short cause, final Object platformInfo)
            {
                System.out.println ("NSD CONNECTED!");
            }

            public void onNamingServiceDisconnect(short cause, final Object platformInfo)
            {
                System.out.println ("NSD DISCONNECTED!");
            }

            public void onQuality(short cause, final Object platformInfo)
            {
                System.out.println ("TRANSPORT QUALITY!");
                short quality = transport.getQuality();
                System.out.println ("Transport quality is now " +
                                    MamaQuality.toString(quality) +
                                    ", cause " + MamaDQCause.toString (cause) +
                                    ", platformInfo: " + platformInfo);
            }
        } );

        /*The name specified here is the name identifying properties in the
         * mama.properties file*/
        transport.create (transportName, myBridge);

        if (!qualityForAll)
        {
            transport.setInvokeQualityForAllSubscs (false);
        }

        /*MamaSource for all subscriptions*/
        mySource     = new MamaSource ();
        mySource.setTransport (transport);
        mySource.setSymbolNamespace (mySymbolNamespace);

        /*MamaSource for dictionary subscription*/
        myDictSource = new MamaSource ();
        myDictSource.setTransport (myDictTransport);
        myDictSource.setSymbolNamespace (dictSource);

        if (myThrottleRate != -1)
        {
            transport.setOutboundThrottle
                (MamaThrottleInstance.DEFAULT_THROTTLE, myThrottleRate
            );
        }

        if (myRecapThrottleRate != -1)
        {
            transport.setOutboundThrottle (
                    MamaThrottleInstance.RECAP_THROTTLE, 
                    myRecapThrottleRate);
        }
    }

    private static void readSubjectsFromFile() throws IOException
    {
        InputStream input;
        if (filename != null)
        {
            input = new FileInputStream (filename);
        }
        else
        {
            input = System.in;
            System.out.println ("Enter one symbol per line and terminate with a .");
            System.out.print ("SUBJECT>");
        }

        final BufferedReader reader =
            new BufferedReader (new InputStreamReader (input));

        String symbol;
        while (null != (symbol = reader.readLine()))
        {
            if (!symbol.equals(""))
            {
                if (symbol.equals( "." ))
                {
                    break;
                }
                subjectList.add (symbol);
            }

            if (input == System.in)
            {
                System.out.print ("SUBJECT>");
            }
        }

        if (subjectList.isEmpty())
        {
            System.err.println ("No subjects specified");
            System.exit (1);
        }
    }

    private static void print (final String what, final int width)
    {
        if(quietness < 1)
        {
            int whatLength = 0;
            if (what!=null)
                whatLength = what.length();

            StringBuffer sb = new StringBuffer (what);

            final int spaces = width - whatLength;

            for (int i = 0; i < spaces; i++) sb.append (" ");
            
            System.out.print (sb.toString());
        }
    }

    private static void parseCommandLine (final String[] args)
    {
        for(int i = 0; i < args.length;)
        {
            if (args[i].equals ("-source") || args[i].equals("-S"))
            {
                mySymbolNamespace = args[i +1];
                i += 2;
            }
            else if (args[i].equals ("-d") || args[i].equals("-dict_source"))
            {
                dictSource = args[i + 1];
                i += 2;
            }
	    else if (args[i].equals ("-dict_tport"))
 	    {
		myDictTportName = args[i + 1];
		i += 2;
	    }
            else if (args[i].equals ("-D"))
            {
                dumpDataDict = true;
                i++;
            }
            else if (args[i].equals ("-I"))
            {
                requireInitial = false;
                i++;
            }
            else if (args[i].equals ("-s"))
            {
                subjectList.add (args[i + 1]);
                i += 2;
            }
            else if (args[i].equals ("-f"))
            {
                filename = args[i + 1];
                i += 2;
            }
            else if (args[i].equals ("-1"))
            {
                snapshot = true;
                i++;
            }
            else if (args[i].equals ("-r"))
            {
                myThrottleRate = Long.parseLong (args[i + 1]);
                i += 2;
            }
            else if (args[i].equals ("-rr"))
            {
                myRecapThrottleRate = Long.parseLong (args[i + 1]);
                i += 2;
            }
            else if (args[i].equals ("-hw"))
            {
                myHighWatermark = Long.parseLong (args[i + 1]);
                i += 2;
            }
            else if (args[i].equals ("-lw"))
            {
                myLowWatermark = Long.parseLong (args[i + 1]);
                i += 2;
            }
            else if (args[i].equals ("-t"))
            {
                timeout = Double.parseDouble (args[i + 1]);
                i += 2;
            }
            else if (args[i].equals ("-tport"))
            {
                transportName = args[i + 1];
                i += 2;
            }
            else if (args[i].equals ("-threads"))
            {
                numThreads = Integer.parseInt (args[i+1]);
                i += 2;
            }
            else if (args[i].equals ("-shutdown"))
            {
                myShutdownTime = Integer.parseInt (args[i+1]);
                i += 2;
            }
            else if (args[i].equals ("-A"))
            {
                qualityForAll = false;
                i++;
            }
            else if (args[i].equals ("-printmessage"))
            {
                printFromMessage = true;
                i ++;
            }
            else if (args[i].equals ("-q"))
            {
                myLogLevel = myLogLevel == null
                            ? Level.WARNING : myLogLevel == Level.WARNING   
                            ? Level.SEVERE  : Level.OFF;
                
                Mama.enableLogging (myLogLevel);
                quietness++;
                i++;
            }
            else if (args[i].equals ("-g"))
            {
                myGroupSubscription =  true;
                i++;
            }
            else if (args[i].equals ("-ni"))
            {
                newIterators =true;
                i++;
            }
            else if (args[i].equals ("-v"))
            {
                myLogLevel = myLogLevel == null
                            ? Level.FINE    : myLogLevel == Level.FINE    
                            ? Level.FINER   : Level.FINEST;

                Mama.enableLogging (myLogLevel);
                i++;
            }
            else if (args[i].equals ("-m"))
            {
                myMiddleware = args[i + 1];
                i += 2;
            }
            else if (args[i].equals ("-ra"))
            {
                retryAttempts = Integer.parseInt (args[i + 1]);
                i += 2;
            }
            else
            {
                fieldList.add (args[i]);
                i++;
            }

        }
    }
    
    /* Class for processing the shutdown timer callback. */
    private static class ShutdownTimerCallback implements MamaTimerCallback
    {
		// The main bridge
		private MamaBridge m_bridge;
		
        // The queue group
        private MamaQueueGroup m_queueGroup;

        private ShutdownTimerCallback(MamaBridge bridge, MamaQueueGroup queueGroup)
		{
			// Save arguments in member variables
			m_bridge = bridge;
            m_queueGroup = queueGroup;
		}
		
		public void onTimer(MamaTimer timer)
		{
			// Destroy the timer
			timer.destroy();
		
            // Stop all the queues
            if(numThreads > 0)
            {
                m_queueGroup.stopDispatch();
            }

			// Shutdown the mama
			Mama.stop(m_bridge);
		}

        public void onDestroy (MamaTimer timer)
        {
            System.out.println ("Timer destroyed");
        }
    }

    /*Class for processing all event callbacks for all subscriptions*/
    private static class SubscriptionCallback implements MamaSubscriptionCallback
    {
        private int indent = 1;

        public void onMsg (final MamaSubscription subscription, final MamaMsg msg)
        {
            try
            {
                switch (MamaMsgType.typeForMsg (msg))
                {
                    case MamaMsgType.TYPE_DELETE:
                    case MamaMsgType.TYPE_EXPIRE:
                        subscription.destroy ();
                        subscriptions.remove (subscription);
                        return;
                }

                switch (MamaMsgStatus.statusForMsg (msg))
                {
                    case MamaMsgStatus.STATUS_BAD_SYMBOL:
                    case MamaMsgStatus.STATUS_EXPIRED:
                    case MamaMsgStatus.STATUS_TIMEOUT:
                        subscription.destroy();
                        subscriptions.remove (subscription);
                        return;
                }
            }
            catch (Exception ex) 
            {
                ex.printStackTrace ();
                System.exit (0);
            }
            
            if (quietness < 1)
            {
                System.out.println (subscription.getSymbol () +
                    " Type: " + MamaMsgType.stringForType (msg) +
                    " Status: " + MamaMsgStatus.stringForStatus (msg));
            }

            
            if (fieldList.size() == 0)
            {
                displayAllFields (msg);
            }
            else
            {
                displayFields (msg, fieldList);

            }
        }

        /*Class for processing fields within a message - for the message
         * iterator*/
        private class FieldIterator implements MamaMsgFieldIterator
        {
            public void onField (MamaMsg        msg,
                                 MamaMsgField   field,
                                 MamaDictionary dictionary,
                                 Object         closure)
            {
                try
                {
                    indent();
                    print (field.getName(),20);
                    print (" | ", 0);
                    print ("" + field.getFid(),4);
                    print (" | ", 0);
                    print ("" + field.getTypeName(),10);
                    print (" | ", 0);

                    if (printFromMessage==true)
                    {
                        printFromMessage (msg, field);
                    }
                    else
                    {
                        printFromField (field);
                    }

                    // if it was a VECTOR_MSG field, we've already 'newlined'
                    if (field.getType() != MamaFieldDescriptor.VECTOR_MSG)
                    print (" \n ", 0);

                }
                catch (Exception ex)
                {
                    ex.printStackTrace();
                }
            }

            /*Access the data from the field objects*/
            private void printFromField (MamaMsgField field)
            {
                short fieldType = field.getType ();
                switch (fieldType)
                {
                    case MamaFieldDescriptor.BOOL:
                        print ("" + field.getBoolean(), 20);
                        break;
                    case MamaFieldDescriptor.CHAR:
                        print ("" + field.getChar(), 20);
                        break;
                    case MamaFieldDescriptor.I8:
                        print ("" + field.getI8(), 20);
                        break;
                    case MamaFieldDescriptor.U8:
                        print ("" + field.getU8(), 20);
                        break;
                    case MamaFieldDescriptor.I16:
                        print ("" + field.getI16(), 20);
                        break;
                    case MamaFieldDescriptor.U16:
                        print ("" + field.getU16(), 20);
                        break;
                    case MamaFieldDescriptor.I32:
                        print ("" + field.getI32(), 20);
                        break;
                    case MamaFieldDescriptor.U32:
                        print ("" + field.getU32(), 20);
                        break;
                    case MamaFieldDescriptor.I64:
                        print ("" + field.getI64(), 20);
                        break;
                    case MamaFieldDescriptor.U64:
                        print ("" + field.getU64(), 20);
                        break;
                    case MamaFieldDescriptor.F32:
                        print ("" + field.getF32(), 20);
                        break;
                    case MamaFieldDescriptor.F64:
                        print ("" + field.getF64(), 20);
                        break;
                    case MamaFieldDescriptor.STRING:
                        print (field.getString(), 20);
                        break;
                    case MamaFieldDescriptor.TIME:
                        print ("" + field.getDateTime (), 20);
                        break;
                    case MamaFieldDescriptor.PRICE:
                        print ("" + field.getPrice (), 20);
                        break;
						  case MamaFieldDescriptor.VECTOR_MSG:
                        printVectorMessage(field);
                        break;
                    default:
                        print ("Unknown type: " + fieldType, 20);
                }
            }

            /*Access the data from the message object - random access*/
            private void printFromMessage (MamaMsg msg, MamaMsgField field)
            {
                short fieldType = field.getType();
                switch (fieldType)
                {
                    case MamaFieldDescriptor.BOOL:
                        print ("" + msg.getBoolean(field.getName(),
                                    field.getFid()), 20);
                        break;
                    case MamaFieldDescriptor.CHAR:
                        print ("" + msg.getChar(field.getName(),
                                    field.getFid()), 20);
                        break;
                    case MamaFieldDescriptor.I8:
                        print ("" + msg.getI8(field.getName(),
                                    field.getFid()), 20);
                        break;
                    case MamaFieldDescriptor.U8:
                        print ("" + msg.getU8(field.getName(),
                                    field.getFid()), 20);
                        break;
                    case MamaFieldDescriptor.I16:
                        print ("" + msg.getI16(field.getName(),
                                    field.getFid()), 20);
                        break;
                    case MamaFieldDescriptor.U16:
                        print ("" + msg.getU16(field.getName(),
                                    field.getFid()), 20);
                        break;
                    case MamaFieldDescriptor.I32:
                        print ("" + msg.getI32(field.getName(),
                                    field.getFid()), 20);
                        break;
                    case MamaFieldDescriptor.U32:
                        print ("" + msg.getU32(field.getName(),
                                    field.getFid()), 20);
                        break;
                    case MamaFieldDescriptor.I64:
                        print ("" + msg.getI64(field.getName(),
                                    field.getFid()), 20);
                        break;
                    case MamaFieldDescriptor.U64:
                        print ("" + msg.getU64(field.getName(),
                                    field.getFid()), 20);
                        break;
                    case MamaFieldDescriptor.F32:
                        print ("" + msg.getF32(field.getName(),
                                    field.getFid()), 20);
                        break;
                    case MamaFieldDescriptor.F64:
                        print ("" + msg.getF64(field.getName(),
                                    field.getFid()), 20);
                        break;
                    case MamaFieldDescriptor.STRING:
                        print ("" + msg.getString(field.getName(),
                                    field.getFid()), 20);
                        break;
                    case MamaFieldDescriptor.TIME:
                        print ("" + msg.getDateTime (field.getName (),
                                    field.getFid ()), 20);
                        break;
                case MamaFieldDescriptor.PRICE:
                        print ("" + msg.getPrice (field.getName (),
                                    field.getFid ()), 20);
                        break;
                    default:
                        print ("Unknown type: " + fieldType, 20);
                }
                return;
            }
        }

        private void displayFields (final MamaMsg          msg,
                                    final ArrayList        fieldList)
        {

            for (Iterator iterator = fieldList.iterator(); iterator.hasNext();)
            {
                final String name = (String) iterator.next();

                MamaFieldDescriptor field = dictionary.getFieldByName (name);

                displayField (field, msg);
            }
        }

        private synchronized void displayField (MamaFieldDescriptor field,
                                                final MamaMsg msg)
        {
            if (field == null                                   ||
                field.getType() == MamaFieldDescriptor.U32ARRAY ||
                field.getType() == MamaFieldDescriptor.U16ARRAY ||
                field.getType() == MamaFieldDescriptor.MSG )
            {
                return;
            }

            String fieldName = field.getName();

            if (fieldName == null || fieldName.length() == 0)
            {
                MamaFieldDescriptor tmpField =
                    dictionary.getFieldByFid (field.getFid());
                if (tmpField != null) fieldName = tmpField.getName();
            }

            if (fieldName == null) fieldName = "unknown";

            if (quietness < 1)
            {
                System.out.print ("\t");
                print (fieldName, 20);
                System.out.print (" | ");
                print ("" + field.getFid(), 4);
                System.out.print (" | ");
                print (MamaFieldDescriptor.getTypeName( field.getType() ), 10);
                System.out.print (" | ");
                try
                {
                    switch (field.getType())
                    {
                        case MamaFieldDescriptor.CHAR:
                            System.out.println (msg.getChar(field));
                            break;
                        case MamaFieldDescriptor.U8:
                            System.out.println (msg.getU8(field));
                            break;
                        case MamaFieldDescriptor.I16:
                            System.out.println (msg.getI16(field));
                            break;
                        case MamaFieldDescriptor.I32:
                            System.out.println (msg.getI32(field));
                            break;
                        case MamaFieldDescriptor.U32:
                            System.out.println (msg.getU32(field));
                            break;
                        case MamaFieldDescriptor.I64:
                            System.out.println (msg.getI64(field));
                            break;
                        case MamaFieldDescriptor.U64:
                            System.out.println (msg.getU64(field));
                            break;
                        case MamaFieldDescriptor.F64:
                            System.out.println (msg.getF64(field));
                            break;
                        case MamaFieldDescriptor.STRING:
                            System.out.println (msg.getString(field));
                            break;
                        case MamaFieldDescriptor.TIME:
                            System.out.println (msg.getDateTime (field));
                            break;
                        case MamaFieldDescriptor.PRICE:
                            System.out.println (msg.getPrice (field));
                            break;
                        default:
                            System.out.println (
                                    msg.getFieldAsString(field.getFid(),dictionary));
                    }
                }
                catch (MamaFieldNotFoundException e)
                {
                    System.out.println ("Field not found in message.");
                }
            }
        }

        private synchronized void displayAllFields(
                final MamaMsg          msg )
        {
            if (quietness < 2)
            {
                if (!(newIterators))
                {
                    msg.iterateFields (new FieldIterator(), dictionary, "Closure");
                }
                else
                {
                    for (Iterator iterator=msg.iterator(dictionary); iterator.hasNext();)
                    {
                        MamaMsgField field = (MamaMsgField) iterator.next();
                        try
                        {
                            indent();
                            print (field.getName(),20);
                            print (" | ", 0);
                            print ("" + field.getFid(),4);
                            print (" | ", 0);
                            print ("" + field.getTypeName(),10);
                            print (" | ", 0);
                            displayMamaMsgField (field);

                            // if it was a VECTOR_MSG field, we've already 'newlined'
                            if (field.getType() != MamaFieldDescriptor.VECTOR_MSG)
                            print (" \n ", 0);
                        }
                        catch (Exception ex)
                        {
                            ex.printStackTrace();
                        }
                    }
                }
            }
        }

        private void indent()
        {
            for (int i=0;i<indent;i++)
                print("   ", 0);
        }

        private void displayMamaMsgField (MamaMsgField field)
        {
            short fieldType = field.getType ();
            switch (fieldType)
            {
                case MamaFieldDescriptor.BOOL:
                    print ("" + field.getBoolean(), 20);
                    break;
                case MamaFieldDescriptor.CHAR:
                    print ("" + field.getChar(), 20);
                    break;
                case MamaFieldDescriptor.I8:
                    print ("" + field.getI8(), 20);
                    break;
                case MamaFieldDescriptor.U8:
                    print ("" + field.getU8(), 20);
                    break;
                case MamaFieldDescriptor.I16:
                    print ("" + field.getI16(), 20);
                    break;
                case MamaFieldDescriptor.U16:
                    print ("" + field.getU16(), 20);
                    break;
                case MamaFieldDescriptor.I32:
                    print ("" + field.getI32(), 20);
                    break;
                case MamaFieldDescriptor.U32:
                    print ("" + field.getU32(), 20);
                    break;
                case MamaFieldDescriptor.I64:
                    print ("" + field.getI64(), 20);
                    break;
                case MamaFieldDescriptor.U64:
                    print ("" + field.getU64(), 20);
                    break;
                case MamaFieldDescriptor.F32:
                    print ("" + field.getF32(), 20);
                    break;
                case MamaFieldDescriptor.F64:
                    print ("" + field.getF64(), 20);
                    break;
                case MamaFieldDescriptor.STRING:
                    print (field.getString(), 20);
                    break;
                case MamaFieldDescriptor.TIME:
                    print ("" + field.getDateTime (), 20);
                    break;
                case MamaFieldDescriptor.PRICE:
                    print ("" + field.getPrice (), 20);
                    break;
                case MamaFieldDescriptor.VECTOR_MSG:
                    printVectorMessage(field);
                    break;
                default:
                    print ("Unknown type: " + fieldType, 20);
            }
        }

        private void printVectorMessage(MamaMsgField field)
        {
                    MamaMsg[] vMsgs = field.getArrayMsg();
            print("\n",0);
                    for (int i =0; i!= vMsgs.length; i++)
                    {
                indent();
                print("{", 0);
                print("\n",0);
                indent++;
                        displayAllFields (vMsgs[i]);
                indent--;
                indent();
                print("}\n", 0);

            }
        }
            
        /*Invoked once the subscrption request has been dispatched from the
         * throttle queue.*/
        public void onCreate (MamaSubscription subscription)
        {
            subscriptions.add (subscription);
        }

        /*Invoked if any errors are encountered during subscription processing*/
        public void onError(MamaSubscription subscription,
                            short            mamaStatus,
                            int              tibrvStatus, 
                            String           subject, 
                            Exception        e)
        {
            System.err.println ("Symbol=[" + subscription.getSymbol() + "] : " +
                                "An error occurred creating subscription: " +
                                MamaStatus.stringForStatus (mamaStatus));

        }

        /*Invoked if the quality status for the subscription changes*/
        public void onQuality (MamaSubscription subscription, short quality,
                               short cause, final Object platformInfo)
        {
            System.err.println( subscription.getSymbol () + 
                                ": quality is now " +
                                MamaQuality.toString (quality) +
                                ", cause " + cause +
                                ", platformInfo: " + platformInfo);
        }
        
        public void onGap (MamaSubscription subscription)
        {
            System.err.println (subscription.getSymbol () + ": gap detected ");
        }
        
        public void onRecapRequest (MamaSubscription subscription)
        {
            System.err.println (subscription.getSymbol () + ": recap requested ");
        }

        public void onDestroy (MamaSubscription subscription)
        {
            System.out.println ("Subscription destroyed");
        }
    }
}
