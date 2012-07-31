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

import com.wombat.mama.*;
import com.wombat.common.WombatException;
import java.util.Iterator;
import java.util.logging.Logger;
import java.util.logging.Level;

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
 *
 */


public class MamaSubscriberJava
{
    private static MamaTransport            myTransport     = null;
    private static MamaBasicSubscription    mySubscription  = null;
    private static MamaBridge               myBridge        = null;
    private static MamaQueue                myDefaultQueue  = null;
    private static String                   myMiddleware    = "wmw";
    private static String                   myTopic         = "MAMA_TOPIC";
    private static String                   myTransportName = null;
    private static int                      myQuietLevel = 0;
    private static final Logger             logger              =
                                            Logger.getLogger( "com.wombat.mama" );
    private static Level                    myLogLevel;

    public static void main( String [] args ) throws InterruptedException
    {
        parseCommandLine( args );

        initializeMama();
        createSubscriber();

        Mama.start (myBridge);
        synchronized (MamaSubscriberJava.class)
        {
            MamaSubscriberJava.class.wait();
        }

        mySubscription.destroy();
        myTransport.destroy();

        Mama.close();
    }


       /*Class for processing fields within a message - for the message
         * iterator*/
        private static class FieldIterator implements MamaMsgFieldIterator
        {
            public void onField (MamaMsg        msg,
                                 MamaMsgField   field,
                                 MamaDictionary dictionary,
                                 Object         closure)
            {
                try
                {
                    print (field.getName(),20);
                    print (" | ", 0);
                    print ("" + field.getFid(),4);
                    print (" | ", 0);
                    print ("" + field.getTypeName(),10);
                    print (" | ", 0);
                    printFromField (field);

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


    private static void initializeMama( )
    {
        try
        {
            myBridge = Mama.loadBridge (myMiddleware);
            Mama.open();
            myDefaultQueue = Mama.getDefaultQueue (myBridge);
            myTransport = new MamaTransport ();
            myTransport.create (myTransportName, myBridge);
        }
        catch( WombatException e )
        {
            e.printStackTrace( );
            System.err.println( "Error initializing mama: "  + e );
            System.exit(1);
        }
    }

    private static void createSubscriber( )
    {

        try
        {
            MamaBasicSubscription subsc = new MamaBasicSubscription ();
            subsc.createBasicSubscription(
                new SubscriptionCallback(),
                myTransport,
                myDefaultQueue,
                myTopic,
                null );
        }
        catch( WombatException e )
        {
            e.printStackTrace( );
            System.err.println( "Error creating subscription: " + e );
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
            else if (args[i].equals ("-v"))
            {
                myLogLevel = myLogLevel == null
                             ? Level.FINE    : myLogLevel == Level.FINE
                             ? Level.FINER   : Level.FINEST;

                Mama.enableLogging (myLogLevel);
                i++;
            }
			else
			{
				i++;
			}
        }

        if( myQuietLevel < 2 )
        {
            System.out.println(
                "Starting Publisher with:\n" +
                "   middleware:         " + myMiddleware    + "\n" +
                "   topic:              " + myTopic         + "\n" +
                "   transport:          " + myTransportName + "\n" );
        }
    }

    private static void print (final String what, final int width)
    {
        int whatLength = 0;
        if (what!=null)
            whatLength = what.length();
    
        StringBuffer sb = new StringBuffer(what);
    
        final int spaces = width - whatLength;
    
        for(int i = 0; i < spaces; i++) 
            sb.append(" ");
        
        System.out.print(sb.toString());
    }
    
    private static class SubscriptionCallback 
        implements MamaBasicSubscriptionCallback
    {
        public void onCreate( MamaBasicSubscription subscription )
        {
            if( myQuietLevel < 2 )
            {
                mySubscription = subscription;
                System.out.println( "Created inbound subscription.\n" );
            }

        }

        public void onError( MamaBasicSubscription subscription,
                             short status,
                             int platformError,
                             String subject )
        {
            System.out.println( "Error creating subscriptionBridge: %s\n" + status );
            System.exit(1);
        }

        public void onMsg(MamaBasicSubscription subscription, MamaMsg msg)
        {
            if( myQuietLevel < 2 )
            {
                System.out.println ("Reveived msg: Topic=" +
                                   msg.getString ("PublisherTopic",10002) +
                                   ", Sequence Nunber="+msg.getU32 ("MdSeqNum", 10001));
                System.out.println("Received msg:");
                displayAllFields(msg);
            }
        }

        public void onDestroy (MamaBasicSubscription subscription)
        {
            System.out.println("Subscription destroyed\n");
        }

        private synchronized void displayAllFields(final MamaMsg msg)
        {

			msg.iterateFields(new FieldIterator(), null, "Closure");
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
                case MamaFieldDescriptor.MSG:
                    System.out.println ("{ " );
                    displayAllFields(field.getMsg());
                    System.out.println ("}" );
                    break;
                case MamaFieldDescriptor.VECTOR_MSG:
                    MamaMsg[] vMsgs = field.getArrayMsg();
                    for (int i =0; i!= vMsgs.length; i++)
                    {
                        System.out.println ("{ " );
                        displayAllFields (vMsgs[i]);
                        System.out.println ("}" );
                    }
                    ;
                    break;

                default:
                    print ("Unknown type: " + fieldType, 20);
            }
        }
        
    }
}

