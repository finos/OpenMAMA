package com.wombat.mama.examples;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.logging.Logger;
import java.util.logging.Level;
import com.wombat.mama.MamaTimer;
import java.util.Map;
import java.util.Collections;
import java.util.HashMap;
import java.io.*;
import java.util.Random;

import com.wombat.mama.*;

/**
 * MamaListenCached example program is used to test the MamaFieldCache.
 *
 * It creates a local cache for the subscription and prints out all the data each time
 * a delta arrives.
 *
 * Verify option can be used to check the integrity of the cache against the feed cache
 * every configurable interval.
 **/
 
public class MamaListenCached
{
    private static MamaTransport    transport           = null;
    private static String           myMiddleware        = "wmw";
    private static MamaQueue        myDefaultQueue      = null;
    private static MamaBridge       myBridge            = null;
    private static double           throttle            = -1;
    private static MamaDictionary   dictionary          = null;
    private static String           dictSource          = "WOMBAT";
    private static boolean          dumpDataDict        = false;
    private static boolean          buildDataDict       = true;    
    private static boolean          dictionaryComplete  = false;
    private static String           transportName       = "internal";
    private static boolean          requireInitial      = true;
    private static boolean          snapshot            = false;
    private static double           timeout             = 10.0;
    private static int              quietness           = 0;
    private static boolean          printUsingName      = false;
    private static boolean          printUsingFid       = false;
    private static int              numThreads          = 0;
    private static MamaQueueGroup   queueGroup          = null;
    private static boolean          qualityForAll       = true;
    private static final ArrayList  subscriptions       = new ArrayList();
    private static final ArrayList  subjectList         = new ArrayList();
    private static String           filename            = null;
    private static String           mySymbolNamespace   = null;
    private static final Logger     logger              = Logger.getLogger( "com.wombat.mama" );
    private static Level            myLogLevel;
    private static boolean          myGroupSubscription = false;
    private static MamaSource       mySource;   
    private static MamaSource       myDictSource;
    private static boolean          verifyAgainstFeed   = false;
    private static MamaTimer        myVerifyTimer       = null;
    private static double           myVerifyInterval    = 1.0;
    private static String           failedField         = null;
    private static boolean          display             = true;    
    
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
            /*This will only block on JNI*/
            Mama.start (myBridge);
            /*Keep the main thread running if using pure Java MAMA*/
            Thread.currentThread().join();
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

    private static void subscribeToSubjects()
    {
        int howMany = 0;

        queueGroup = new MamaQueueGroup (numThreads, myBridge);

        /*Subscribe to all symbols specified on the commanty line or from the
         * symbol file*/
        for (Iterator iterator = subjectList.iterator(); iterator.hasNext();)
        {
            MamaQueue queue;
            final String symbol = (String) iterator.next();
            
            MamaSubscription subscription = new MamaSubscription ();
            queue= queueGroup.getNextQueue ();
            /*Properties common to all subscription types*/
            subscription.setTimeout (timeout);
            subscription.setRetries (1);
            subscription.setRequiresInitial (requireInitial);
            SubscriptionCallback callback = new SubscriptionCallback(queue, symbol);    
            subscription.createSubscription (callback,
                                             queue,
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
        System.out.flush();
    }

    private static void buildDataDictionary()
        throws InterruptedException
    {
        MamaDictionaryCallback dictionaryCallback = createDictionaryCallback();
        
        MamaSubscription subscription = new MamaSubscription ();
        
        if (buildDataDict)
        {
            synchronized (dictionaryCallback)
            {
                /*The dictionary is obtained through a specialized form of
                * subscription.*/            

                dictionary = subscription.createDictionarySubscription (
                        dictionaryCallback,
                        myDefaultQueue,
                        myDictSource,
                        10.0,
                        2);
                    
                /*Mama.start() will only block for JNI*/
                Mama.start (myBridge);
                if (!dictionaryComplete) dictionaryCallback.wait( 30000 );
                if (!dictionaryComplete)
                {
                    System.err.println( "Timed out waiting for dictionary." );
                    System.exit( 0 );
                }
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
        for (Iterator iterator = subscriptions.iterator(); iterator.hasNext();)
        {
            final MamaSubscription subscription = (MamaSubscription) iterator.next();
            subscription.destroy();
        }
        if (transport != null)
        {
            transport.destroy();
        }
        Mama.close();
    }

    private static void initializeMama()
    {
        try
        {
            myBridge = Mama.loadBridge (myMiddleware);
            /*Always the first API method called. Initialized internal API
             * state*/
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
        /*The name specified here is the name identifying properties in the
         * mama.properties file*/
        transport.create (transportName, myBridge);
       
        /*Receive notification of transport level events*/
        transport.addTransportListener( new MamaTransportListener()
        {
            public void onConnect(short cause, final Object platformInfo)
            {
                System.out.println ("TRANSPORT CONNECTED!");
            }
            public void onDestroy(short cause, final Object platformInfo)
            {
                System.out.println ("TRANSPORT DESTROYED!");
            }
            public void onAccept(short cause, final Object platformInfo)
            {
                System.out.println ("TRANSPORT ACCEPTED!");
            }
            public void onAcceptReconnect(short cause, final Object platformInfo)
            {
                System.out.println ("ACCEPTED RECONNECT!");
            }

            public void onPublisherDisconnect(short cause, final Object platformInfo)
            {
                System.out.println ("PUBLISHER DISCONNECTED!");
            }

            public void onNamingServiceConnect(short cause, final Object platformInfo)
            {
                System.out.println ("NAMING SERVICE CONNECTED!");
            }

            public void onNamingServiceDisconnect(short cause, final Object platformInfo)
            {
                System.out.println ("NAMING SERVICE DISCONNECTED!");
            }

            public void onDisconnect(short cause, final Object platformInfo)
            {
                System.out.println ("TRANSPORT DISCONNECTED!");
            }

            public void onReconnect(short cause, final Object platformInfo)
            {
                System.out.println ("TRANSPORT RECONNECTED!");
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
        myDictSource.setTransport (transport);
        myDictSource.setSymbolNamespace (dictSource);

        if (throttle != -1)
        {
            transport.setOutboundThrottle
                (MamaThrottleInstance.DEFAULT_THROTTLE,throttle);
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
        System.out.flush();
    }

    private static void print (final String what, final int width)
    {
        if(quietness < 1)
        {
            int whatLength = 0;
            if (what!=null)
                whatLength = what.length();

            final int spaces = width - whatLength;
            System.out.print (what);

            for (int i = 0; i < spaces; i++)
                System.out.print(" ");
        }
        System.out.flush();
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
            else if (args[i].equals ("-d"))
            {
                dictSource = args[i + 1];
                i += 2;
            }
            else if (args[i].equals ("-D"))
            {
                dumpDataDict = true;
                i++;
            }
            else if (args[i].equals ("-B"))
            {
                buildDataDict = false;
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
                throttle = Double.parseDouble (args[i + 1]);
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
            else if (args[i].equals ("-A"))
            {
                qualityForAll = false;
                i++;
            }
            else if (args[i].equals ("-printUsingName"))
            {
                printUsingName = true;
                i++;
            }
            else if (args[i].equals ("-printUsingFid"))
            {
                printUsingFid = true;
                i++;
            }
            else if (args[i].equals ("-q"))
            {
                quietness++;
                i++;
            }
            else if (args[i].equals ("-g"))
            {
                myGroupSubscription =  true;
                i++;
            }
            else if (args[i].equals ("-m"))
            {
                myMiddleware = args[i + 1];
                i += 2;
            }
            else if (args[i].equals ("-v"))
            {
                myLogLevel = myLogLevel == null
                            ? Level.FINE    :
                            myLogLevel == Level.FINE    
                            ? Level.FINER   : 
                            Level.FINEST;

                Mama.enableLogging (myLogLevel);
                i++;
            }
            else if (args[i].equals ("-verify"))
            {
                verifyAgainstFeed = true;                
                i++;
            }
            else if (args[i].equals ("-verifyInterval"))
            {
                myVerifyInterval = Double.parseDouble (args[i + 1]);
                i += 2;
            }           
        }
    }

    /*Class for processing all event callbacks for all subscriptions*/
    private static class SubscriptionCallback implements MamaSubscriptionCallback
    {
        MamaFieldCache  myFieldCache = new MamaFieldCache();
        MamaTimer       myVerifyTimer;  
        int             myRandomNo;             
        MamaQueue       myQueue;      

        public SubscriptionCallback (MamaQueue queue, String symbol)
        {
            Random generator = new Random();
            myRandomNo = generator.nextInt (10);
            myQueue = queue;
            if (verifyAgainstFeed)
            {                
                if (myVerifyInterval > 0)
                {
                    try
                    {                
                        myVerifyTimer = new MamaTimer();                    
                        myVerifyTimer.create (myQueue, new VerifyCallback(this,symbol), 
                        myVerifyInterval+1);
                    }                 
                    catch (Exception e)
                    {
                        e.printStackTrace( );
                        System.err.println( "Error creating timer: " + e );
                        System.exit(1);  
                    }
                }                
            }
        }
        
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
                    case MamaMsgType.TYPE_SNAPSHOT:
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
            
            //Apply the message to the FieldCache
            myFieldCache.apply(msg,dictionary,null);
            displayAllFields (subscription, msg);
        }

        /*Class for processing fields within a message - for the message
         * iterator*/
        private class FieldIterator implements MamaMsgFieldIterator
        {
            private boolean fieldHasName = true;
            public void onField (MamaMsg        msg,
                                 MamaMsgField   field,
                                 MamaDictionary dictionary,
                                 Object         closure)
            {    
                String fieldName = null;
                try
                {
                    if (fieldHasName)
                    {
                        fieldName = field.getName ();
                    }
                }
                catch (Exception e)
                {
                    fieldHasName = false;
                }
                    
                print (" \t ", 0);
                print (fieldName,20);
                print (" | ", 0);
                print ("" + field.getFid(),4);
                print (" | ", 0);
                print ("" + field.getTypeName(),10);
                print (" | ", 0);

                if (printUsingName == true)
                {
                    if (buildDataDict)
                    {
                        printUsingName (field);
                    }
                    else 
                    {
                        System.out.println ("CANNOT ACCESS FIELDS BY NAME");
                        System.exit(1);                            
                    }
                }
                else
                {                                               
                    printUsingFid (field);
                }
                print (" \n ", 0);
            }

            /*Access the data from the field objects*/
            private void printUsingFid (MamaMsgField field)
            {                
                short fieldType = field.getType ();
                switch (fieldType)
                {
                    case MamaFieldDescriptor.BOOL:
                        MamaFieldCacheBool mfcb = (MamaFieldCacheBool) 
                                            myFieldCache.find (field.getFid());
                        print ("" + mfcb.get(), 20);
                        break;
                    case MamaFieldDescriptor.CHAR:
                        MamaFieldCacheChar mfcc = (MamaFieldCacheChar) 
                                            myFieldCache.find (field.getFid());
                        print ("" + mfcc.get(), 20);
                        break;
                    case MamaFieldDescriptor.I8:
                        MamaFieldCacheInt8 mfci8 = (MamaFieldCacheInt8) 
                                            myFieldCache.find (field.getFid());
                        print ("" + mfci8.get(), 20);
                        break;
                    case MamaFieldDescriptor.U8:
                        MamaFieldCacheUint8 mfcui8 = (MamaFieldCacheUint8) 
                                            myFieldCache.find (field.getFid());
                        print ("" + mfcui8.get(), 20);
                        break;
                    case MamaFieldDescriptor.I16:
                        MamaFieldCacheInt16 mfci16 = (MamaFieldCacheInt16) 
                                            myFieldCache.find (field.getFid());
                        print ("" + mfci16.get(), 20);
                        break;
                    case MamaFieldDescriptor.U16:
                        MamaFieldCacheUint16 mfcui16 = (MamaFieldCacheUint16) 
                                            myFieldCache.find (field.getFid());
                        print ("" + mfcui16.get(), 20);
                        break;
                    case MamaFieldDescriptor.I32:
                        MamaFieldCacheInt32 mfci32 = (MamaFieldCacheInt32) 
                                            myFieldCache.find (field.getFid());
                        print ("" + mfci32.get(), 20);
                        break;
                    case MamaFieldDescriptor.U32:
                        MamaFieldCacheUint32 mfcui32 = (MamaFieldCacheUint32) 
                                            myFieldCache.find (field.getFid());
                        print ("" + mfcui32.get(), 20);
                        break;
                    case MamaFieldDescriptor.I64:
                        MamaFieldCacheInt64 mfci64 = (MamaFieldCacheInt64) 
                                            myFieldCache.find (field.getFid());
                        print ("" + mfci64.get(), 20);
                        break;
                    case MamaFieldDescriptor.U64:
                        MamaFieldCacheUint64 mfcui64 = (MamaFieldCacheUint64) 
                                            myFieldCache.find (field.getFid());
                        print ("" + mfcui64.get(), 20);
                        break;
                    case MamaFieldDescriptor.F32:
                        MamaFieldCacheFloat32 mfcf32 = (MamaFieldCacheFloat32) 
                                            myFieldCache.find (field.getFid());
                        print ("" + mfcf32.get(), 20);
                        break;
                    case MamaFieldDescriptor.F64:
                        MamaFieldCacheFloat64 mfcf64 = (MamaFieldCacheFloat64) 
                                            myFieldCache.find (field.getFid());
                        print ("" + mfcf64.get(), 20);
                        break;
                    case MamaFieldDescriptor.STRING:
                        MamaFieldCacheString mfcs = (MamaFieldCacheString) 
                                            myFieldCache.find (field.getFid());
                        print (mfcs.get(), 20);
                        break;
                    case MamaFieldDescriptor.TIME:
                        MamaFieldCacheDateTime mfcdt = (MamaFieldCacheDateTime) 
                                            myFieldCache.find (field.getFid());
                        print ("" + mfcdt.get(), 20);
                        break;
                    case MamaFieldDescriptor.PRICE:
                        MamaFieldCachePrice mfcp = (MamaFieldCachePrice) 
                                            myFieldCache.find (field.getFid());
                        print ("" + mfcp.get(), 20);
                        break;
                    default:
                        print ("Unknown type: " + fieldType, 20);
                }
            }

            /*Access the data from the message object - random access*/
            private void printUsingName (MamaMsgField field)
            {                
                short fieldType = field.getType();
                switch (fieldType)
                {
                    case MamaFieldDescriptor.BOOL:
                        MamaFieldCacheBool mfcb = (MamaFieldCacheBool) 
                                            myFieldCache.find (field.getName());
                        print ("" + mfcb.get(), 20);
                        break;
                    case MamaFieldDescriptor.CHAR:
                        MamaFieldCacheChar mfcc = (MamaFieldCacheChar) 
                                            myFieldCache.find (field.getName());
                        print ("" + mfcc.get(), 20);
                        break;
                    case MamaFieldDescriptor.I8:
                        MamaFieldCacheInt8 mfci8 = (MamaFieldCacheInt8) 
                                            myFieldCache.find (field.getName());
                        print ("" + mfci8.get(), 20);
                        break;
                    case MamaFieldDescriptor.U8:
                        MamaFieldCacheUint8 mfcui8 = (MamaFieldCacheUint8) 
                                            myFieldCache.find (field.getName());
                        print ("" + mfcui8.get(), 20);
                        break;
                    case MamaFieldDescriptor.I16:
                        MamaFieldCacheInt16 mfci16 = (MamaFieldCacheInt16) 
                                            myFieldCache.find (field.getName());
                        print ("" + mfci16.get(), 20);
                        break;
                    case MamaFieldDescriptor.U16:
                        MamaFieldCacheUint16 mfcui16 = (MamaFieldCacheUint16) 
                                            myFieldCache.find (field.getName());
                        print ("" + mfcui16.get(), 20);
                        break;
                    case MamaFieldDescriptor.I32:
                        MamaFieldCacheInt32 mfci32 = (MamaFieldCacheInt32) 
                                            myFieldCache.find (field.getName());
                        print ("" + mfci32.get(), 20);
                        break;
                    case MamaFieldDescriptor.U32:
                        MamaFieldCacheUint32 mfcui32 = (MamaFieldCacheUint32) 
                                            myFieldCache.find (field.getName());
                        print ("" + mfcui32.get(), 20);
                        break;
                    case MamaFieldDescriptor.I64:
                        MamaFieldCacheInt64 mfci64 = (MamaFieldCacheInt64) 
                                            myFieldCache.find (field.getName());
                        print ("" + mfci64.get(), 20);
                        break;
                    case MamaFieldDescriptor.U64:
                        MamaFieldCacheUint64 mfcui64 = (MamaFieldCacheUint64) 
                                            myFieldCache.find (field.getName());
                        print ("" + mfcui64.get(), 20);
                        break;
                    case MamaFieldDescriptor.F32:
                        MamaFieldCacheFloat32 mfcf32 = (MamaFieldCacheFloat32) 
                                            myFieldCache.find (field.getName());
                        print ("" + mfcf32.get(), 20);
                        break;
                    case MamaFieldDescriptor.F64:
                        MamaFieldCacheFloat64 mfcf64 = (MamaFieldCacheFloat64) 
                                            myFieldCache.find (field.getName());
                        print ("" + mfcf64.get(), 20);
                        break;
                    case MamaFieldDescriptor.STRING:
                        MamaFieldCacheString mfcs = (MamaFieldCacheString) 
                                            myFieldCache.find (field.getName());
                        print (mfcs.get(), 20);
                        break;
                    case MamaFieldDescriptor.TIME:
                        MamaFieldCacheDateTime mfcdt = (MamaFieldCacheDateTime) 
                                            myFieldCache.find (field.getName());
                        print ("" + mfcdt.get(), 20);
                        break;
                    case MamaFieldDescriptor.PRICE:
                        MamaFieldCachePrice mfcp = (MamaFieldCachePrice) 
                                            myFieldCache.find (field.getName());
                        print ("" + mfcp.get(), 20);
                        break;
                    default:
                        print ("Unknown type: " + fieldType, 20);
                }              
            }
        }

        /* Class for processing fields and asserting the value of the cache 
         * against the feed*/
        private class FieldAsserter implements MamaMsgFieldIterator
        {
            public void onField (MamaMsg        msg,
                                 MamaMsgField   field,
                                 MamaDictionary dictionary,
                                 Object         closure)
            { 
                try
                {
                    assertField (field);
                }
                catch (Exception ex)
                {
                    ex.printStackTrace();
                }                
            }
            
            /* Assert the data from the snapshot message against the cache*/
            private void assertField (MamaMsgField field)
            {
                short fieldType = field.getType ();
                if (field.getFid () == MamaReservedFields.MsgType.getId() ||
                    field.getFid () == MamaReservedFields.AppMsgType.getId())
                {
                    return;
                }
                
                switch (fieldType)
                {
                    case MamaFieldDescriptor.BOOL:
                         MamaFieldCacheBool mfcb = (MamaFieldCacheBool) 
                                            myFieldCache.find (field.getFid());
                         if (mfcb.get() != field.getBoolean())   
                            failedField = failedField + " " + field.getName();                            
                         break;
                    case MamaFieldDescriptor.CHAR:
                         MamaFieldCacheChar mfcc = (MamaFieldCacheChar) 
                                            myFieldCache.find (field.getFid());
                         if (mfcc.get() != field.getChar())
                            failedField = failedField + " " + field.getName();                            
                         break;
                    case MamaFieldDescriptor.I8:
                         MamaFieldCacheInt8 mfci8 = (MamaFieldCacheInt8) 
                                            myFieldCache.find (field.getFid());
                         if (mfci8.get() != field.getI8())
                            failedField = failedField + " " + field.getName();                            
                         break;
                    case MamaFieldDescriptor.U8:
                         MamaFieldCacheUint8 mfcui8 = (MamaFieldCacheUint8) 
                                            myFieldCache.find (field.getFid());
                         if (mfcui8.get() != field.getU8())
                            failedField = failedField + " " + field.getName();                            
                         break;
                    case MamaFieldDescriptor.I16:
                         MamaFieldCacheInt16 mfci16 = (MamaFieldCacheInt16) 
                                            myFieldCache.find (field.getFid());
                         if (mfci16.get() != field.getI16())             
                            failedField = failedField + " " + field.getName();
                         break;
                    case MamaFieldDescriptor.U16:
                         MamaFieldCacheUint16 mfcui16 = (MamaFieldCacheUint16) 
                                            myFieldCache.find (field.getFid());
                         if (mfcui16.get() != field.getU16())
                           failedField = failedField + " " + field.getName();
                         break;
                    case MamaFieldDescriptor.I32:
                         MamaFieldCacheInt32 mfci32 = (MamaFieldCacheInt32) 
                                            myFieldCache.find (field.getFid());
                         if (mfci32.get() != field.getI32())
                            failedField = failedField + " " + field.getName();                         
                        break;
                    case MamaFieldDescriptor.U32:
                         MamaFieldCacheUint32 mfcui32 = (MamaFieldCacheUint32) 
                                            myFieldCache.find (field.getFid()); 
                 
                         if (mfcui32.get() != field.getU32())
                         failedField = failedField + " " + field.getName();                            
                         break;
                    case MamaFieldDescriptor.I64:
                         MamaFieldCacheInt64 mfci64 = (MamaFieldCacheInt64) 
                                            myFieldCache.find (field.getFid());
                         if (mfci64.get() != field.getI64())
                             failedField = failedField + " " + field.getName();                            
                         break;
                    case MamaFieldDescriptor.U64:
                         MamaFieldCacheUint64 mfcui64 = (MamaFieldCacheUint64) 
                                            myFieldCache.find (field.getFid());
                         if (mfcui64.get() != field.getU64())
                            failedField = failedField + " " + field.getName();                            
                         break;
                    case MamaFieldDescriptor.F32:
                         MamaFieldCacheFloat32 mfcf32 = (MamaFieldCacheFloat32) 
                                            myFieldCache.find (field.getFid());
                         if (mfcf32.get() != field.getF32())
                            failedField = failedField + " " + field.getName();                            
                         break;
                    case MamaFieldDescriptor.F64:
                         MamaFieldCacheFloat64 mfcf64 = (MamaFieldCacheFloat64) 
                                            myFieldCache.find (field.getFid());
                         if (mfcf64.get() != field.getF64())
                            failedField = failedField + " " + field.getName();                            
                         break;
                    case MamaFieldDescriptor.STRING:
                         MamaFieldCacheString mfcs = (MamaFieldCacheString) 
                                            myFieldCache.find (field.getFid());
                         if (!mfcs.get().equals (field.getString()))
                            failedField = failedField + " " + field.getName();
                         break;
                    case MamaFieldDescriptor.TIME:
                         MamaFieldCacheDateTime mfcdt = (MamaFieldCacheDateTime)
                                            myFieldCache.find (field.getFid());
                         if (!mfcdt.get().equals (field.getDateTime()))
                            failedField = failedField + " " + field.getName();                        
                         break;
                    case MamaFieldDescriptor.PRICE:
                         MamaFieldCachePrice mfcp = (MamaFieldCachePrice) 
                                            myFieldCache.find (field.getFid());
                         if (!mfcp.get().equals (field.getPrice()))
                            failedField = failedField + " " + field.getName();                            
                        break;
                    default:
                        print ("Unknown type: " + fieldType, 20);    
                }                        
            }
        }        
      
        private synchronized void displayAllFields(
                final MamaSubscription subscription,
                final MamaMsg          msg )
        {   
             if (quietness < 1)
             {
                  System.out.println (subscription.getSymbol () +
                         " Type: " + MamaMsgType.stringForType (msg) +
                         " Status: " + MamaMsgStatus.stringForStatus (msg));       
             }
             
             if (quietness < 2)
             {
                if (printUsingFid || printUsingName)
                {
                    msg.iterateFields (new FieldIterator(), dictionary, "Closure");    
                }
                
                Iterator fieldCacheIterator = myFieldCache.getIterator ();
                while (fieldCacheIterator.hasNext())
                {
                    MamaFieldCacheField myField = (MamaFieldCacheField)
                                                    fieldCacheIterator.next();
                    MamaFieldDescriptor myDesc  = myField.getDescriptor();
                    print (" \t ", 0);
                    print (myDesc.getName(),20);
                    print (" | ", 0);
                    print ("" + myDesc.getFid(),4);
                    print (" | ", 0);
                    print ("" + myDesc.getTypeName(myDesc.getType()),10);
                    print (" | ", 0);
                    print ("" + myField.getAsString(), 20);
                    print (" \n ", 0);                    
                }
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

        public void onDestroy(MamaSubscription mamaSub)
        {
            System.out.println("SUBSCRIPTION DESTROYED!");
        }
    }
    
    /* Callback used to verify the data in the fieldcache against the feed*/
    private static class VerifyCallback implements MamaTimerCallback
    {
        SubscriptionCallback myCallback;
        String mySymbol;
        
        public VerifyCallback (SubscriptionCallback callback,String symbol)
        {
            myCallback = callback;
            mySymbol   = symbol;
        }
        
        public synchronized void onTimer (MamaTimer timer)
        {
            MamaSubscription subscription = new MamaSubscription ();
                           
            //Properties common to all subscription types
            subscription.setTimeout (timeout);
            subscription.setRetries (1);                                
            subscription.createSnapshotSubscription (
                                                    myCallback,
                                                    queueGroup.getNextQueue (),
                                                    mySource,
                                                    mySymbol,
                                                    null);                     
        }

        public void onDestroy(MamaTimer mamaTimer)
        {
            System.out.println("TIMER DESTROYED!");
        }
    }
}
