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

package com.wombat.mama.testtools.performance;

import com.wombat.mama.*;
import com.wombat.common.WombatException;
import java.util.ArrayList;
import java.util.logging.Level;
import java.util.Random;
import java.lang.Thread;
import java.util.Random;

public class MamaProducerJava
{
    private static String                myOutboundTopic = "MAMA_TOPIC";
    private static String                myMiddleware    = "wmw";
    private static MamaBridge            myBridge        = null;
    private static MamaQueue             myQueue         = null;
    private static String                myTransportName = null;
    private static MamaTransport    myTransport     = null;
    private static MamaPublisher         myPublisher     = null;
    private static long                  myMsgNumber     = 0;

    private static int myMsgSize = 200;
    private static int myMsgVar = 0;
    private static int mySleepTime = 10;
    private static double myRefreshRate = 1000;
    private static int myMsgTimeStampCount = 1;
    private static int MP_MIN_MSG_SIZE  = 35;
    private static MamaPrice myPrice = null; //new MamaPrice(1.104);
    private static int msgTest = 0;
    private static Level            myLogLevel;
    private static MamaMsg[] msgs = null;
     private static MamaMsg cannedMsg = null;
    private static MamaDateTime     mamaTime       =  null;
    private static int msgIndexStart = 500;
    private static int pricelevels = 10;
    private static long myRunningPublishTime=0;
    
    private static Thread startThread = null;
    /**
     * Initialize Mama and create a basic transport.
     */
    private static void initializeMama( )
    {
        try
        {
            myBridge = Mama.loadBridge (myMiddleware);
            Mama.open();
            myQueue = Mama.getDefaultQueue (myBridge);
            myTransport = new MamaTransport ();
            myTransport.create (myTransportName, myBridge);
            myPublisher = new MamaPublisher();
            myPublisher.create( myTransport, myOutboundTopic );
        }
        catch( WombatException e )
        {
            e.printStackTrace( );
            System.err.println( "Exception occurred initializing mama: " + e );
            System.exit(1);
        }
    }


    
    private static void generateMessageCanned(MamaDateTime mamaTime)
    {
        int index = msgIndexStart;
        cannedMsg = new MamaMsg();
        myPrice = new MamaPrice(1.104);
        cannedMsg.addDateTime(null, 16, mamaTime);  // linetime
        cannedMsg.addDateTime(null, index++, mamaTime);  // createtime

        cannedMsg.addU32(null, 10,0);  //seqnum

        // add in the parts from the template message
        cannedMsg.addI32(null, index++, 1);
        cannedMsg.addString(null,index++, "AGG");
        cannedMsg.addString(null,index++,"AUDCAD.SPOT");
        cannedMsg.addDateTime(null, index++, mamaTime);
        cannedMsg.addI32(null,index++,0);
        cannedMsg.addChar(null, index++,'a');
        cannedMsg.addI32(null, index++,9);
        cannedMsg.addI32(null, index++,9);
        cannedMsg.addDateTime(null, index++, mamaTime);
        
        for (int i=0;i<pricelevels;i++)
        {
            // ask price
            cannedMsg.addPrice(null, index++, myPrice);
            // ask size
            cannedMsg.addI64(null, index++, 2000000);
            // buy price
            cannedMsg.addPrice(null, index++,myPrice);
            // buy size
            cannedMsg.addI64(null,index++,20000);
        }
        
        cannedMsg.addChar(null, index++,'0');
        cannedMsg.addChar(null, index++,'0');
        cannedMsg.addChar(null, index++,'0');
        cannedMsg.addChar(null, index++,'0');
        cannedMsg.addChar(null, index++,'0');
        cannedMsg.addChar(null, index++,'0');
        cannedMsg.addI64(null, index++, 1247007799);
        cannedMsg.addI64(null, index++, 93094);
    }
    
    private static void updateMessageCanned(MamaDateTime mamaTime)
    {
        int index = msgIndexStart;
        cannedMsg.updateDateTime(null, 16, mamaTime);  // linetime
        cannedMsg.updateDateTime(null, index++, mamaTime);  // createtime
        cannedMsg.updateU32(null, 10,0);  //seqnum

        // add in the parts from the template message
        cannedMsg.updateI32(null, index++, 1);
        cannedMsg.updateString(null,index++, "AGG");
        cannedMsg.updateString(null,index++,"AUDCAD.SPOT");
        cannedMsg.updateDateTime(null, index++, mamaTime);
        cannedMsg.updateI32(null,index++,0);
        cannedMsg.updateChar(null, index++,'a');
        cannedMsg.updateI32(null, index++,9);
        cannedMsg.updateI32(null, index++,9);
        cannedMsg.updateDateTime(null, index++, mamaTime);
        
        for (int i=0;i<pricelevels;i++)
        {
            // ask price
            cannedMsg.updatePrice(null, index++, myPrice);
            // ask size
            cannedMsg.updateI64(null, index++, 2000000);
            // buy price
            cannedMsg.updatePrice(null, index++,myPrice);
            // buy size
            cannedMsg.updateI64(null,index++,20000);
        }
        
        cannedMsg.updateChar(null, index++,'0');
        cannedMsg.updateChar(null, index++,'0');
        cannedMsg.updateChar(null, index++,'0');
        cannedMsg.updateChar(null, index++,'0');
        cannedMsg.updateChar(null, index++,'0');
        cannedMsg.updateChar(null, index++,'0');
        cannedMsg.updateI64(null, index++, 1247007799);
        cannedMsg.updateI64(null, index++, 93094);
    }
    
    private static void rebuildMessageCanned(MamaDateTime mamaTime)
    {
        int index = msgIndexStart;
        cannedMsg.clear();
        cannedMsg.addDateTime(null, 16, mamaTime);  // linetime
        cannedMsg.addDateTime(null, index++, mamaTime);  // createtime

        cannedMsg.addU32(null, 10,0);  //seqnum

        // add in the parts from the template message
        cannedMsg.addDateTime(null, index++, mamaTime);  // linetime
        cannedMsg.addI32(null, index++, 1);
        cannedMsg.addString(null,index++, "AGG");
        cannedMsg.addString(null,index++,"AUDCAD.SPOT");
        cannedMsg.addDateTime(null, index++, mamaTime);
        cannedMsg.addI32(null,index++,0);
        cannedMsg.addChar(null, index++,'a');
        cannedMsg.addI32(null, index++,9);
        cannedMsg.addI32(null, index++,9);
        cannedMsg.addDateTime(null, index++, mamaTime);
        
        for (int i=0;i<pricelevels;i++)
        {
            // ask price
            cannedMsg.addPrice(null, index++, myPrice);
            // ask size
            cannedMsg.addI64(null, index++, 2000000);
            // buy price
            cannedMsg.addPrice(null, index++,myPrice);
            // buy size
            cannedMsg.addI64(null,index++,20000);
        }
        
        cannedMsg.addChar(null, index++,'0');
        cannedMsg.addChar(null, index++,'0');
        cannedMsg.addChar(null, index++,'0');
        cannedMsg.addChar(null, index++,'0');
        cannedMsg.addChar(null, index++,'0');
        cannedMsg.addChar(null, index++,'0');
        cannedMsg.addI64(null, index++, 1247007799);
        cannedMsg.addI64(null, index++, 93094);
    }
    
    private static void generateMessagePadded(int msgSize, int msgVar, MamaDateTime mamaTime )
    { 
        byte[] padding = null;
        int paddingSize = msgSize - MP_MIN_MSG_SIZE;
        padding = new byte[paddingSize];

        msgs = new MamaMsg[(2*msgVar)+1];
        mamaTime.setToNow();

        for (int i=0; i<=2*msgVar; i++)
        {
            msgs[i]= new MamaMsg();
            msgs[i].addU32(null, 10,0);
            msgs[i].addDateTime(null, 16, mamaTime);

            msgs[i].addArrayI8(null, 10004, padding);
        }
    }




    private static void parseCommandLine( String [] args )
    {
        for ( int i = 0; i < args.length; )
        {
            String arg = args[ i ];

            if( "-s".equals( arg ) )
            {
                myOutboundTopic = args[i+1];
                i += 2;
            }
            else if( "-l".equals( arg ) )
            {
                myMsgSize = Integer.parseInt( args[i+1] );
                i += 2;
            }
            else if( "-sleep".equals( arg ) )
            {
                mySleepTime = Integer.parseInt( args[i+1]);
                i += 2;
            }
            else if( "-i".equals( arg ) )
            {
                myRefreshRate = Double.parseDouble( args[i+1] );
                i += 2;
            }
            else if( "-t".equals( arg ) )
            {
                myMsgTimeStampCount = Integer.parseInt( args[i+1] );
                i += 2;
            }
            else if( "-d".equals( arg ) )
            {
                myMsgVar = Integer.parseInt( args[i+1] );
                i += 2;
            }
            else if( "-tport".equals( arg ) )
            {
                myTransportName = args[i+1];
                i += 2;
            }
            else if ( "-m".equals( arg ))
            {
                myMiddleware = args[i+1];
                i += 2;
            }  
            else if (args[i].equals ("-v"))
            {
                myLogLevel = myLogLevel == null
                            ? Level.FINE    : myLogLevel == Level.FINE    
                            ? Level.FINER   : Level.FINEST;

                Mama.enableLogging (myLogLevel);
                i++;
            }
            else if (args[i].equals("-updateMsg"))
            {
                msgTest = 1;
                i++;
            }
            else if (args[i].equals("-rebuildMsg"))
            {
                msgTest = 2;
                i++;
            }
            else if (args[i].equals("-generateMsg"))
            {
                msgTest = 3;
                i++;
            }
            else
            {
                System.err.println( "Exception occurred parsing command line: " + args[i] );
                System.exit(1);
                
            }
        }
    }
    
    static void publishCannedMessage(long msgNum, MamaDateTime mamaTime)
    {
        switch (msgTest)
        {
            case 1: updateMessageCanned(mamaTime);  break;
            case 2: rebuildMessageCanned(mamaTime);  break;
            case 3: generateMessageCanned(mamaTime);  break;
        }
        cannedMsg.updateU32(null, 10, msgNum);
        mamaTime.setToNow(); 
        cannedMsg.updateDateTime(null, 16, mamaTime);
        myPublisher.send (cannedMsg);
    	myRunningPublishTime+=  MamaDateTime.diffMicroseconds(mamaTime, cannedMsg.getDateTime(null, 500));
    }
    
    static void publishMessage(  long msgNum,  MamaDateTime mamaTime,  int msgSample)
    {  
        msgs[msgSample].updateU32(null, 10, msgNum);
        msgs[msgSample].updateDateTime(null, 16, mamaTime);
        myPublisher.send ( msgs[msgSample]);
    }
  
    public static void main( String [] args ) throws InterruptedException
    {
        int randValCounts=0,i=0,loops=0,randVal=0;
        double diff=0,rate=0;
        Random            generator   = new Random();
        long last=0, now=0;
	double updatetime = 0;
        
        parseCommandLine( args );
        initializeMama();
        
        mamaTime       =  new MamaDateTime();
                
        if (msgTest != 0)
            generateMessageCanned(mamaTime);
        else
            generateMessagePadded(myMsgSize, myMsgVar, mamaTime);
        
        startThread = new Thread(new Runnable() {
					public void run() {
                        Mama.start (myBridge);
                    }});
                    
        startThread.start();
        
        last = System.currentTimeMillis();
        
        if (myMsgVar !=0)
            randValCounts = (2*myMsgVar)+1;
        
        for(;;)
        {
            if (myMsgVar !=0)
                randVal = generator.nextInt((int) System.currentTimeMillis()) % randValCounts;
         
            for (i=0; i<myRefreshRate;i++)
            { 
                if (i%myMsgTimeStampCount==0)   mamaTime.setToNow();
                
                if (msgTest != 0)
                    publishCannedMessage (myMsgNumber++,mamaTime);
                else
                    publishMessage (myMsgNumber++,mamaTime,randVal);
            Thread.sleep ((long)0, mySleepTime);  
            }
                 
            loops = loops + 1;
                
            now = System.currentTimeMillis();
            diff = (double)(now - last);
 
            if (diff > 1000)
            {
                rate = (myRefreshRate * loops) / (diff /1000.0);
		updatetime = myRunningPublishTime / (myRefreshRate * loops);
                System.out.println("RATE: " + rate + "  Avg Pub time: " + updatetime );
                last = now;
                loops = 0;
		myRunningPublishTime = 0;
            }
        }
  
    }



}
