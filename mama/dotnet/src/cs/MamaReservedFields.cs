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

using System;

namespace Wombat
{
	/// <summary>
	/// Maintains a cache of common field descriptors.
	/// </summary>
	public class MamaReservedFields
	{
		private MamaReservedFields()
		{
		}

		public static readonly MamaFieldDescriptor MsgStatus = new MamaFieldDescriptor(2, mamaFieldType.MAMA_FIELD_TYPE_I32, "MdMsgStatus");
		public static readonly MamaFieldDescriptor SeqNum = new MamaFieldDescriptor(10, mamaFieldType.MAMA_FIELD_TYPE_I64, "MdSeqNum");
		public static readonly MamaFieldDescriptor MsgNum = new MamaFieldDescriptor(7, mamaFieldType.MAMA_FIELD_TYPE_I32, "MdMsgNum");
		public static readonly MamaFieldDescriptor MsgTotal = new MamaFieldDescriptor(8, mamaFieldType.MAMA_FIELD_TYPE_I32, "MdMsgTotal");

        public static readonly MamaFieldDescriptor AppMsgType = new MamaFieldDescriptor(18, mamaFieldType.MAMA_FIELD_TYPE_I32, "MamaAppMsgType");
        public static readonly MamaFieldDescriptor MsgType = new MamaFieldDescriptor(1, mamaFieldType.MAMA_FIELD_TYPE_I32, "MdMsgType");
		public static readonly MamaFieldDescriptor SenderId = new MamaFieldDescriptor(20,mamaFieldType.MAMA_FIELD_TYPE_U64,"MamaSenderId");
        public static readonly MamaFieldDescriptor SendTime =  new MamaFieldDescriptor(16, mamaFieldType.MAMA_FIELD_TYPE_TIME, "MamaSendTime");
        public static readonly MamaFieldDescriptor SymbolList = new MamaFieldDescriptor(81, mamaFieldType.MAMA_FIELD_TYPE_STRING, "MamaSymbolList");
        public static readonly MamaFieldDescriptor EntitleCode = new MamaFieldDescriptor(496, mamaFieldType.MAMA_FIELD_TYPE_I32, "wEntitleCode");
	}
}
