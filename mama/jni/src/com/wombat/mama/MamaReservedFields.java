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

package com.wombat.mama;

public class MamaReservedFields
{
    /**
     * Field for the message type.
     *
     * @see com.wombat.mama.MamaMsgType
     */
    public static final MamaReservedFields MsgType =
         new MamaReservedFields( "MdMsgType", 1);
    /**
     * Field for the message status.
     *
     * @see com.wombat.mama.MamaMsgStatus
     */
    public static final MamaReservedFields MsgStatus =
         new MamaReservedFields( "MdMsgStatus", 2 );
    /**
     * Field containing the index in the data dictionary.
     */
    public static final MamaReservedFields FieldIndex =
         new MamaReservedFields( "MdFieldIndex", 3 );
    /**
     * Field containing the fields in the data dictionary. This is an array.
     *
     * @deprecated
     */
    public static final MamaReservedFields MsgFields =
       new MamaReservedFields( "MdMsgFields", 4 );
    /**
     * Used internally for subscription management (deprecated).
     */
    public static final MamaReservedFields SubscMsgTypeOld =
        new MamaReservedFields( "MdSubscType", 5 );
    /**
     * Used internally for subscription management (deprecated).
     */
    public static final MamaReservedFields SubscMsgSubjectOld =
         new MamaReservedFields( "MdSubscSubject", 6 );
    /**
     * Field containing the current message number in a multi-part initial message.
     *
     * @see com.wombat.mama.MamaReservedFields#MsgTotal
     */
    public static final MamaReservedFields MsgNum =
         new MamaReservedFields( "MdMsgNum", 7 );
    /**
     * The total number of messages comprising an initial message.
     */
    public static final MamaReservedFields MsgTotal =
         new MamaReservedFields( "MdMsgTotal", 8 );
    /**
     * The RV result code.
     */
    public static final MamaReservedFields TibrvResult =
        new MamaReservedFields( "MdTibrvResult", 9 );
    /**
     * The sequence number.
     */
    public static final MamaReservedFields SeqNum =
        new MamaReservedFields( "MdSeqNum", 10);
    /**
     * The dictionary source feed name.
     */
    public static final MamaReservedFields FeedName =
        new MamaReservedFields( "MdFeedName", 11);
    /**
     * The dictionary source feed host.
     */
    public static final MamaReservedFields FeedHost =
        new MamaReservedFields( "MdFeedHost", 12);
    /**
     * That pattern that subjects of a sync request should match.
     */
    public static final MamaReservedFields SyncPattern =
        new MamaReservedFields( "MdSyncPattern", 14 );

    /**
     * The item sequence number.
     */
    public static final MamaReservedFields ItemSeqNum =
        new MamaReservedFields ("MdItemSeq", 15);

    /**
     * The Message send time.
     */
    public static final MamaReservedFields SendTime =
        new MamaReservedFields ("MamaSendTime", 16);

    /**
     * The application data type.
     */
    public static final MamaReservedFields AppDataType =
        new MamaReservedFields ("MamaAppDataType", 17);

    /**
     * The application message type.
     */
    public static final MamaReservedFields AppMsgType =
        new MamaReservedFields ("MamaAppMsgType", 18);

    /**
     * The sender id. This identifies the feed handler sending the message. This may change in the event of a fault
     * tolerant take over. Mama handles this situation internally.
     */
    public static final MamaReservedFields SenderId =
        new MamaReservedFields ("MamaSenderId", 20);

    /**
     * The message qualifier
     *
     * @see MamaMsg for details.
     */
    public static final MamaReservedFields MsgQualifier =
        new MamaReservedFields ("wMsgQual", 21);

    /* For Elvin, we put the "source" as a separate field (it's a
 * doublecheck for the other field whose *name* acts as efficient
 * content-based filter). */
    public static final MamaReservedFields ElvinSource =
        new MamaReservedFields( "MdElvinSource", 55 );
    public static final MamaReservedFields ElvinSourceOld =
        new MamaReservedFields( "MdSubscSource", 64 );

    public static MamaReservedFields SubscriptionType =
        new MamaReservedFields( "MdSubscriptionType", 60);

    public static final MamaReservedFields SubscMsgType =
        new MamaReservedFields( "MdSubscMsgType", 61 );

    public static final MamaReservedFields SubscSourceHost =
        new MamaReservedFields( "MdSubscSourceHost", 63 );

    public static MamaReservedFields SubscSourceApp =
        new MamaReservedFields( "MdSubscSourceApp", 64);

    public static final MamaReservedFields SubscSourceUser =
        new MamaReservedFields( "MdSubscSourceUser", 65 );

    public static final MamaReservedFields SubscSymbol =
        new MamaReservedFields( "MdSubscSymbol", 71 );

    public static final MamaReservedFields SymbolList =
        new MamaReservedFields( "MamaSymbolList", 81 );

    public static final MamaReservedFields EntitleCode =
        new MamaReservedFields ( "wEntitleCode", 496);

    private final String name;
    private final int id;
    public static final short MAX_RESERVED_FID = 100;

    public MamaReservedFields( String name, int id )
    {
        this.name = name;
        this.id = id;
    }

    /**
     * Return the name of the reserved field.
     * @return  The name.
     */
    public String getName()
    {
        return name;
    }

    /**
     * Return the id of the reserved field.
     * @return The id.
     */
    public int getId()
    {
        return id;
    }
}
