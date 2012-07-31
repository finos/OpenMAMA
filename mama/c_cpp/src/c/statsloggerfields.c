/* $Id: statsloggerfields.c,v 1.1.2.3 2011/08/10 14:53:26 nicholasmarriott Exp $
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

#include <mama/mama.h>
#include <mama/statsloggerfields.h>
#include <mama/reservedfields.h>

const MamaReservedField  MamaSlIntervalStartTime         
    = {"MamaSlIntervalStartTime",   101};
const MamaReservedField  MamaSlIntervalEndTime    
    = {"MamaSlIntervalEndTime",     102};
const MamaReservedField  MamaSlEvents  
    = {"MamaSlEvents",              103};   
const MamaReservedField  MamaSlEventTime    
    = {"MamaSlEventTime",           104};  
const MamaReservedField  MamaSlEventType   
    = {"MamaSlEventType",           105};    
const MamaReservedField  MamaSlEventReason    
    = {"MamaSlReason",              106};  
const MamaReservedField  MamaSlTotalMsgCount    
    = {"MamaSlTotalMsgCount",       107}; 
const MamaReservedField  MamaSlIntervalMsgCount    
    = {"MamaSlIntervalMsgCount",    108};        
const MamaReservedField  MamaStatEvents
    = {"MamaStatEvents",            109};
      
