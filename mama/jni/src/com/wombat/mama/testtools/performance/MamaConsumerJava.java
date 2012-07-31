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

import java.text.SimpleDateFormat;
import com.wombat.mama.*;
import com.wombat.common.WombatException;
import java.util.Iterator;
import java.util.Date;

 /**
 * This sample application demonstrates how to send mamaMsg's from an inbox,
 * and receive the reply.
 *
 * It accepts the following command line arguments:
 *      [-s topic]         The topic to which to subscribe. Default value
 *                         is "MAMA_INBOUND_TOPIC".
 *      [-tport name]      The transport parameters to be used from
 *                         mama.properties.
 *      [-q]               Quiet mode. Suppress output.
 *      [-t]               Time interval in uSec to print stats
 *
 */
public class MamaConsumerJava
{
    private static final String longFormat = "%12s";
    private static final String doubleFormat = " %12.4f";

    private static MamaTransport            myTransport     = null;
    private static MamaBasicSubscription    mySubscription  = null;
    private static MamaBridge               myBridge        = null;
    private static MamaQueue                myDefaultQueue  = null;
    private static String                   myMiddleware    = "wmw";
    private static String                   myTopic         = "MAMA_TOPIC";
    private static String                   myTransportName = null;
    private static int                      myQuietLevel = 0;
    private static int                      myTimeInterval = 1000;
    private static boolean myCollectStats  = false, mySD = false;
    private static long             myTransLatencyArray[]  = null;
    private static int myStartLogging = 0, myBucketCount = 30, myBucketWidth = 1000;
    private static MamaTimer myEndTimer = null;
    private static int   myDuration = 0;
    private static boolean iterate = false;
    private static boolean extract = false;

    private static class TermStats implements MamaTimerCallback
    {
        public void onTimer (MamaTimer timer)
        {
            Mama.stop (myBridge);
        }

        public void onDestroy (MamaTimer timer)
        {}
    }

    private static void latencyCount(int lowerBound, int upperBound, StatHolder holder)
    {
        int bintotal = 0, i;

        for (i=lowerBound; i<=upperBound; i++)
            bintotal += myTransLatencyArray[i];

        holder.setLastMessages(bintotal);
        holder.prettyPrint(lowerBound, (upperBound!=myBucketCount*myBucketWidth -1) ? upperBound : 0);
	 }

    public static void main( String [] args ) throws InterruptedException
    {
        parseCommandLine( args );

        initializeMama();

        if (myDuration > 0)
        {
            myEndTimer = new MamaTimer ();
            myEndTimer.create (myDefaultQueue, new TermStats (), myDuration);
        }

        Mama.start (myBridge);

        int totalMessages = 0;
        for (int i=0; i<(myBucketCount*myBucketWidth); i++)
        {
            totalMessages += myTransLatencyArray[i];
        }


        StatHolder holder = new StatHolder(totalMessages);
        holder.prettyPrintTitle();
        for (int i=0; i<(myBucketCount*myBucketWidth); i+=myBucketWidth)
        {
            latencyCount(i,i+myBucketWidth-1,holder);
        }

        mySubscription.destroy();
        myTransport.destroy();

        Mama.close();
    }

    private static void initializeMama( )
    {
        try
        {
            myBridge = Mama.loadBridge (myMiddleware);
            Mama.open();
            myDefaultQueue = Mama.getDefaultQueue (myBridge);
            myTransport = new MamaTransport ();
            myTransport.create (myTransportName, myBridge);
            mySubscription = new MamaBasicSubscription ();
            mySubscription.createBasicSubscription(
                new SubscriptionCallback(),
                myTransport,
                myDefaultQueue,
                myTopic,
                null );
        }
        catch( WombatException e )
        {
            e.printStackTrace( );
            System.err.println( "Error initializing mama: "  + e );
            System.exit(1);
        }

    }

    private static void parseCommandLine( String [] args )
    {
        for ( int i = 0; i < args.length; )
        {
            String arg = args[ i ];

            if( "-s".equals( arg ) )
            {
                myTopic = args[i+1];
                i += 2;
            }
            else if( "-tport".equals( arg ) )
            {
                myTransportName = args[i+1];
                i += 2;
            }
            else if( "-q".equals( arg ) )
            {
                myQuietLevel++;
                i++;
            }
            else if ( "-m".equals( arg ))
            {
                myMiddleware = args[i+1];
                i += 2;
            }
            else if ( "-t".equals( arg ))
            {
                myTimeInterval = Integer.parseInt(args[i+1]) * 1000;
                i += 2;
            }
            else if ( "-start-stats".equals( arg ))
            {
                myStartLogging = Integer.parseInt(args[i+1]);
                i += 2;
            }
            else if ( "-stats".equals( arg ))
            {
                myCollectStats = true;
                i++;
            }
            else if ( "-sd".equals( arg ))
            {
                mySD = true;
                i++;
            }
            else if ( "-bucketCount".equals( arg ))
            {
                myBucketCount = Integer.parseInt(args[i+1]);
                i += 2;
            }
            else if ( "-bucketWidth".equals( arg ))
            {
                myBucketWidth = Integer.parseInt(args[i+1]);
                i += 2;
            }
            else if ( "-duration".equals( arg ))
            {
                myDuration = Integer.parseInt(args[i+1]);
                i += 2;
            }
            else if ("-iterate".equals(arg))
            {
                iterate = true;
                i++;
            }
            else if ("-extract".equals(arg))
            {
                extract = true;
                i++;
            }
        }
        System.out.println(
            "Starting Consumer with:\n" +
            "   middleware:         " + myMiddleware    + "\n" +
            "   topic:              " + myTopic         + "\n" +
            "   transport:          " + myTransportName + "\n" +
            "   time interval:      " + myTimeInterval  + "\n" );
    }

    private static class SubscriptionCallback implements MamaBasicSubscriptionCallback
    {
        private static final SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss:SSSS");

        private MamaDateTime myTimeNow = new MamaDateTime("");
        private long highLatency = java.lang.Long.MIN_VALUE;
        private long lowLatency = java.lang.Long.MAX_VALUE;
        private long runningLatency =0;
        private long endtoendrunningLatency =0;

        private long nowLast, last = 0, now=0;
        private Date myDateNow = new Date();
        private double msgnum = 0;
        private double myCalcSD = 0, mySDX2= 0, mySDTot =0;
        private long totalCallbackTimeInMicro = 0;
        private long callbackCount = 0;
        private long latency = 0;
        private long endtoendlatency = 0;
        private long nanoTimeStart = 0;
        private double callbackLatency = 0;
        long nanoEndTime = 0;
         char myChar;
        byte myI8;
        short myU8;
        short myI16 ;
        int myU16 ;
        int myI32 ;
        long myU32 ;
        long myI64 ;
        long myU64 ;
        float myF32 ;
        double myF64 ;
        String myString;
        MamaDateTime myDateTime;
        MamaPrice myPrice;
        boolean myBool;
        SubscriptionCallback ()
        {
            outputTitle();
            myTransLatencyArray = new long[myBucketCount * myBucketWidth +1];
        }


        public void onCreate( MamaBasicSubscription subscription )
        {
            last = System.currentTimeMillis();
        }

        public void onError( MamaBasicSubscription subscription,
                             short status,
                             int platformError,
                             String subject )
        {
            System.out.println( "Error creating subscriptionBridge: %s\n" + status );
            System.exit(1);
        }

        public void onDestroy (MamaBasicSubscription subscription)
        {
            System.out.println ("Subscription destroyed\n");
        }

        private void extractField (MamaMsgField field)
        {
            switch (field.getType ())
            {
                case MamaFieldDescriptor.BOOL:myBool =  field.getBoolean(); break;
                case MamaFieldDescriptor.CHAR:myChar = field.getChar();  break;
                case MamaFieldDescriptor.I8: myI8 = field.getI8(); break;
                case MamaFieldDescriptor.U8:myU8 = field.getU8();break;
                case MamaFieldDescriptor.I16:myI16 = field.getI16();break;
                case MamaFieldDescriptor.U16:myU16 = field.getU16();break;
                case MamaFieldDescriptor.I32:myI32 = field.getI32();break;
                case MamaFieldDescriptor.U32:myU32 = field.getU32();break;
                case MamaFieldDescriptor.I64:myI64 = field.getI64();break;
                case MamaFieldDescriptor.U64:myU64 = field.getU64();break;
                case MamaFieldDescriptor.F32:myF32 = field.getF32();break;
                case MamaFieldDescriptor.F64:myF64 = field.getF64();break;
                case MamaFieldDescriptor.STRING: myString = field.getString();break;
                case MamaFieldDescriptor.TIME:myDateTime = field.getDateTime ();break;
                case MamaFieldDescriptor.PRICE:myPrice = field.getPrice ();break;
                case MamaFieldDescriptor.VECTOR_MSG:
                    MamaMsg[] vMsgs = field.getArrayMsg();
                    for (int i =0; i!= vMsgs.length; i++)
		    {
                         for (Iterator iter = vMsgs[i].iterator(); iter.hasNext(); )
                	{
                    		MamaMsgField vfield = (MamaMsgField)iter.next();
                        	extractField(vfield);
                	}

                    break;
			}
                default:
                    System.out.println("Unknown type: " + field.getType ());
            }
        }
        public void onMsg(MamaBasicSubscription subscription, MamaMsg msg)
        {
            nanoTimeStart = System.nanoTime();
            now = System.currentTimeMillis();
            myTimeNow.setToNow();
            latency = MamaDateTime.diffMicroseconds(myTimeNow, msg.getDateTime(null, 16));

            runningLatency += latency;
            if (latency < lowLatency)  lowLatency= latency;
            if (latency > highLatency) highLatency= latency;


            if (iterate)
            {
                for (Iterator iter = msg.iterator(); iter.hasNext(); )
                {
                    MamaMsgField field = (MamaMsgField)iter.next();
                    if (extract)
                        extractField(field);
                }
		        myTimeNow.setToNow();
            	endtoendlatency = MamaDateTime.diffMicroseconds(myTimeNow, msg.getDateTime(null, 500));
            	endtoendrunningLatency += endtoendlatency;
            }


            if (myCollectStats & (myStartLogging == 0))
            {
                if (latency <= 0)
                {
                    myTransLatencyArray[0]++;
                }
                else if (latency >= myBucketWidth * myBucketCount)
                {
                    myTransLatencyArray[(myBucketWidth * myBucketCount)-1]++;
                }
                else
                {
                    myTransLatencyArray[(int) latency]++;
                }
            }

            if (mySD)
                mySDX2 += latency * latency;


            nowLast = (now - last);
            msgnum++;

            if (nowLast >= myTimeInterval)
            {
                myDateNow.setTime(now);

                if (mySD)
                {
                    mySDTot = (runningLatency*runningLatency) / msgnum;
                    myCalcSD = Math.sqrt ( (mySDX2 -  mySDTot) /  ( msgnum - 1)) ;
                }

                callbackLatency = ((double)totalCallbackTimeInMicro/(double)(msgnum -1));

                // get the stats ready for reporting
                String time = sdf.format(myDateNow);
                long rate = (long)((float)(msgnum)/((float)((float)nowLast/1000)));
                double cbLatency = callbackLatency;
                long low = lowLatency;
                long avg = (long)((float)(runningLatency)/(float)(msgnum));
                long e2e = iterate ? (long) ((float) (endtoendrunningLatency) / (float) (msgnum)) : 0;
                long high = highLatency;
                double sd = mySD ? myCalcSD : 0;

                outputStats(time, rate, low, avg, e2e, high, sd, cbLatency);

                latency = runningLatency = endtoendlatency = endtoendrunningLatency =0;
                last = now;
                highLatency = java.lang.Long.MIN_VALUE;
                lowLatency = java.lang.Long.MAX_VALUE;
                msgnum = 0;
                mySDX2 = 0;

                // reset the callback latency stats
                totalCallbackTimeInMicro = 0;
            }
            else
            {
                // add this average and increment your count.
                nanoEndTime = System.nanoTime();
                totalCallbackTimeInMicro += ((nanoEndTime - nanoTimeStart) / 1000);
            }
        }
    }

    private static void outputTitle()
    {
        System.out.format("%-15s", "Time");
        System.out.format(longFormat,"Rate");
        System.out.format(" %12s", "Ave C'back");
        System.out.format(longFormat, "Low");
        System.out.format(longFormat, "Avg");
        System.out.format(longFormat, "High");
        if (iterate)
            System.out.format(longFormat, "E2E Lat");
        if (mySD)
            System.out.format(" %12s", "SD");

        System.out.println("\n");

    }

    private static void outputStats(String time, long rate, long low,
        long avg,long end2end, long high, double sd, double cbLatency)
    {
        System.out.format("%-15s", time);
        System.out.format(longFormat,rate);
        System.out.format(doubleFormat, cbLatency);
        System.out.format(longFormat, low);
        System.out.format(longFormat, avg);
        System.out.format(longFormat, high);
        if (iterate)
            System.out.format(longFormat, end2end);
        if (mySD)
            System.out.format(doubleFormat, sd);
        System.out.println("\n");
    }

    /**
     * Holder to contain the statistics gathering at the end of the program
     */
    private static class StatHolder
    {
        /**
         * This will be the total number of messages - all %s will be based off this number
         */
        private final long totalNumMessages;

        private long lastMessages;
        private long runningTotalMessages;

        public StatHolder(long totalNumMessages)
        {
            this.totalNumMessages = totalNumMessages;
        }

        public void setLastMessages(long lastMessages)
        {
            this.lastMessages = lastMessages;
            runningTotalMessages += lastMessages;
        }

        private float getLastPercentage()
        {
            return ((float)lastMessages/(float)totalNumMessages) *100;
        }

        private float getCumulativePercentage()
        {
            return ((float)runningTotalMessages/(float)totalNumMessages) *100;
        }

        public void prettyPrintTitle()
        {
            System.out.println();
            System.out.println("Final Statistics");
            System.out.println();
            System.out.format("%6s", "Lower");
            System.out.println(" - ");
            System.out.format("%-6s", "Upper");
            System.out.format("%8s", "#msgs");
            System.out.format("%15s", "%age");
            System.out.format("%15s", "Cumulative%");
            System.out.println("\n");
        }

        /**
         * @param lowerBound lower bound for array
         * @param upperBound upper bound for array - 0 if no upper bound
         */
        public void prettyPrint(int lowerBound, int upperBound)
        {
            System.out.format("%6s", lowerBound == 0 ? "" :lowerBound);
            System.out.println(" - ");
            System.out.format("%-6s", upperBound == 0 ? "" :upperBound);
            System.out.format("%8s", lastMessages);
            System.out.format("%15.8s", getLastPercentage());
            System.out.format("%15.8s", getCumulativePercentage());
            System.out.println("\n");
        }
    }
}
