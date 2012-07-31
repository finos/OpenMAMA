/* $Id: statsloggerfields.h,v 1.1.2.3 2011/09/01 16:34:38 emmapollock Exp $
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

#ifndef StatsLoggerFieldsH__
#define StatsLoggerFieldsH__

#include "mama/mama.h"

#if defined(__cplusplus)
extern "C" {
#endif
MAMAExpDLL
extern const MamaReservedField  MamaSlIntervalStartTime;         
MAMAExpDLL 
extern const MamaReservedField  MamaSlIntervalEndTime; 
MAMAExpDLL 
extern const MamaReservedField  MamaSlEvents;
MAMAExpDLL
extern const MamaReservedField  MamaSlEventTime;  
MAMAExpDLL
extern const MamaReservedField  MamaSlEventType;                 
MAMAExpDLL 
extern const MamaReservedField  MamaSlEventReason;
MAMAExpDLL 
extern const MamaReservedField  MamaSlTotalMsgCount;
MAMAExpDLL 
extern const MamaReservedField  MamaSlIntervalMsgCount;
MAMAExpDLL
extern const MamaReservedField  MamaStatEvents;
    
                        
#if defined(__cplusplus)       
}                              
#endif                         
                               
#endif /* StatsLoggerFieldsH__*/

