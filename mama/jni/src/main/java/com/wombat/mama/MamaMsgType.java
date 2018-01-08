/* $Id:
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

/**
 * A utility class for retrieving and interpreting type information from
 * Mama Messages.
 */
public class MamaMsgType
{
    /** general update (indices, funds).  */
    public static final short TYPE_UPDATE         = 0;
    /** initial value.                     */
    public static final short TYPE_INITIAL        = 1;
    /** trade cancel.                      */
    public static final short TYPE_CANCEL         = 2;
    /** trade error.                       */
    public static final short TYPE_ERROR          = 3;
    /** trade correction.                  */
    public static final short TYPE_CORRECTION     = 4;
    /** closing summary.                   */
    public static final short TYPE_CLOSING        = 5;
    /** refresh/recap of some/all fields.  */
    public static final short TYPE_RECAP          = 6;
    /** symbol deleted.                    */
    public static final short TYPE_DELETE         = 7;
    /** expired. (e.g. option, future).     */
    public static final short TYPE_EXPIRE         = 8;
    /** full-record snapshot (no updates). */
    public static final short TYPE_SNAPSHOT       = 9;
    /** pre-opening summary. (e.g. morning "roll"). */
    public static final short TYPE_PREOPENING     = 12;
    /** quote updates.                     */
    public static final short TYPE_QUOTE          = 13;
    /** trade updates.                    */
    public static final short TYPE_TRADE          = 14;
    /** order updates.                     */
    public static final short TYPE_ORDER          = 15;
    /** order book initial.                */
    public static final short TYPE_BOOK_INITIAL   = 16;
    /** order book update.                 */
    public static final short TYPE_BOOK_UPDATE    = 17;
    /** order book clear.                  */
    public static final short TYPE_BOOK_CLEAR     = 18;
    /** order book recap.                  */
    public static final short TYPE_BOOK_RECAP     = 19;
    /** order book snapshot.               */
    public static final short TYPE_BOOK_SNAPSHOT  = 20;
    /** not permissioned (on feed).        */
    public static final short TYPE_NOT_PERMISSIONED = 21;
    /** not found (but may show up later). */
    public static final short TYPE_NOT_FOUND      = 22;
    /** end of group of initial values. */
    public static final short TYPE_END_OF_INITIALS = 23;
    /** service request. */
    public static final short TYPE_WOMBAT_REQUEST = 24;
    /** calculated result. */
    public static final short TYPE_WOMBAT_CALC    = 25;
    /** security status update */
    public static final short TYPE_SEC_STATUS     = 26;
    /** data dictionary.                   */
    public static final short TYPE_DDICT_SNAPSHOT = 50;
    /** miscellaneous.                    */
    public static final short TYPE_MISC           = 100;

    /* Returned if a platform specific error is encountered the MsgStatus will be
    * the platform specific status code.
    */
    public static final short TYPE_PLATFORM       = 101;

    /* Mama applications do NOT need to process the following
    * message types. They are all internal messages.
    */

    /* set of features and related params for a particular
     * publisher (sent by publisher) */
    public static final short TYPE_FEATURE_SET     = 150;
    /* request for feature set (sent by subscriber). */
    public static final short TYPE_FEATURE_SET_REQ = 151;
    /* publisher subscription synchronization request (sent by publisher) */
    public static final short TYPE_SYNC_REQUEST    = 170;
    /* subscription refresh (sent by subscriber) */
    public static final short TYPE_REFRESH         = 171;
    /* "keepalive" message (sent by publisher) */
    public static final short TYPE_NULL            = 175;

     
    /* World View message types */
    public static final short WV_REQUEST_SYMBOLOGIES        = 180;
    public static final short WV_REPLY_SYMBOLOGIES          = 181;
    public static final short WV_UPDATE_SYMBOLOGIES         = 182;
    
    public static final short WV_REQUEST_SOURCES            = 183;
    public static final short WV_REPLY_SOURCES              = 184;
    public static final short WV_UPDATE_SOURCES             = 185;
    
    public static final short WV_REQUEST_SOURCES_GROUPS     = 186;
    public static final short WV_REPLY_SOURCES_GROUPS       = 187;
    public static final short WV_UPDATE_SOURCES_GROUPS      = 188;
    
    public static final short WV_REQUEST_SYMBOL_RESOLUTION  = 189;
    public static final short WV_REPLY_SYMBOL_RESOLUTION    = 190;
   
    /**
     * Extract the type from the supplied message.
     *
     * @param msg
     * @return The type.
     */
    public static short typeForMsg( final MamaMsg msg )
    {
        return msg.getI16 (MamaReservedFields.MsgType.getName(),
                           MamaReservedFields.MsgType.getId());
    }

    /**
     * Return the type name.
     * @return The type name.
     *
     */
    public static  String stringForType( final MamaMsg msg )
    {
        return stringForType( typeForMsg( msg ) ) ;
    }

    /**
     * Return the type name.
     * @return The type name.
     */
    public  static String stringForType( final short type )
    {
        switch ( type )
        {
        case TYPE_UPDATE:           return "UPDATE";
        case TYPE_INITIAL:          return "INITIAL";
        case TYPE_CANCEL:           return "CANCEL";
        case TYPE_ERROR:            return "ERROR";
        case TYPE_CORRECTION:       return "CORRECTION";
        case TYPE_CLOSING:          return "CLOSING";
        case TYPE_RECAP:            return "RECAP";
        case TYPE_DELETE:           return "DELETE";
        case TYPE_EXPIRE:           return "EXPIRE";
        case TYPE_SNAPSHOT:         return "SNAPSHOT";
        case TYPE_PREOPENING:       return "PREOPENING";
        case TYPE_QUOTE:            return "QUOTE";
        case TYPE_TRADE:            return "TRADE";
        case TYPE_ORDER:            return "ORDER";
        case TYPE_BOOK_INITIAL:     return "BOOK_INITIAL";
        case TYPE_BOOK_UPDATE:      return "BOOK_UPDATE";
        case TYPE_BOOK_CLEAR:       return "BOOK_CLEAR";
        case TYPE_BOOK_RECAP:       return "BOOK_RECAP";
        case TYPE_BOOK_SNAPSHOT:    return "BOOK_SNAPSHOT";
        case TYPE_NOT_PERMISSIONED: return "NOT_PERMISSIONED";
        case TYPE_NOT_FOUND:        return "NOT_FOUND";
        case TYPE_END_OF_INITIALS:  return "END_OF_INITIALS";
        case TYPE_WOMBAT_REQUEST:   return "WOMBAT_REQUEST";
        case TYPE_WOMBAT_CALC:      return "WOMBAT_CALC";
        case TYPE_SEC_STATUS:       return "SECURITY_STATUS";
        case TYPE_DDICT_SNAPSHOT:   return "DDICT_SNAPSHOT";
        case TYPE_MISC:             return "MISC";
        case TYPE_FEATURE_SET:      return "FEATURE_SET";
        case TYPE_SYNC_REQUEST:     return "SYNC_REQUEST";
        case TYPE_REFRESH:          return "REFRESH";
        case TYPE_PLATFORM:         return "PLATFORM";
        case TYPE_NULL:             return "NULL";
        default:                    return "unknown";
        }

    }

}
