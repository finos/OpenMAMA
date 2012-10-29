/* $Id: MamdaMultiSecurityManager.cs,v 1.2.32.5 2012/08/24 16:12:11 clintonmcdowell Exp $
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
using System.Collections;
using Wombat.Containers;

namespace Wombat
{
	/// <summary>
	/// MamdaMultiSecurityManager is a class that manages updates on a
	/// group symbol which provides a single subscription to multiple
	/// different securities.
	/// Developers are notified of each element available for the group and can
	/// choose which elements they wish to provide handling for 
	/// (e.g. based on wildcards).
	/// 
	/// Specialized Listeners can be added to the
	/// <code>MamdaMultiSecurityManager</code> in response to the callbacks
	/// on the <code>MamdaMultiSecurityHandler</code> being invoked. 
	/// Alternatively, the Listeners can be added up front if the participants
	/// are known in advance.
	/// </summary>
	public class MamdaMultiSecurityManager : MamdaMsgListener
	{
		/// <summary>
		/// Only constructor for the class. No default available.
		/// </summary>
		/// <param name="symbol">The group symbol for which the corresponding subscription
		/// was created.</param>
		public MamdaMultiSecurityManager(string symbol)
		{
			mListeners = new HashMap();
			mHandlers = new ArrayList();
			mSymbol = symbol;
		}

		/// <summary>
		/// Add a specialized handler for notifications about new
		/// securities.  
		/// </summary>
		/// <param name="handler">Concrete instance of the MamdaMultiSecurityHandler
		/// interface</param>
		public void addHandler(MamdaMultiSecurityHandler handler)
		{
			mHandlers.Add(handler);
		}

		/// <summary>
		/// Add a specialized message listener (e.g. a MamdaQuoteListener,
		/// MamdaTradeListener, etc.) for a security.
		/// 
		/// Multiple listeners for each security can be added.
		/// </summary>
		/// <param name="listener">Concrete instance of the MamdaMsgListener interface.</param>
		/// <param name="symbol">The symbol for the instrument. </param>
		public void addListener(
			MamdaMsgListener  listener,
			string            symbol)
		{
			((ArrayList)mListeners.valueOf(symbol)).Add(listener);
		}

		/// <summary>
		/// Implementation of the MamdaMsgListener Interface.
		/// </summary>
		/// <param name="subscription"></param>
		/// <param name="msg"></param>
		/// <param name="msgType"></param>
		public void onMsg(
			MamdaSubscription subscription,
			MamaMsg           msg,
			mamaMsgType       msgType)
		{
			try
			{
				string symbol = msg.getString(MamdaCommonFields.ISSUE_SYMBOL);
				if(symbol == null)
				{
					return;
				}
				// First handle the message internally. i.e. create listener
				// lists, notify handlers etc.
				switch (msgType)
				{
					case mamaMsgType.MAMA_MSG_TYPE_INITIAL:
					case mamaMsgType.MAMA_MSG_TYPE_RECAP:
						handleFullImage(subscription, msg, msgType, symbol);
						break;
					case mamaMsgType.MAMA_MSG_TYPE_END_OF_INITIALS:/*Don't forward.*/
						return;
					default:
						break;
				}
				// Now pass to other listeners (if any)
				forwardMsg(subscription, msg, msgType, symbol);
			}
			catch (MamdaDataException ex)
			{
			}
		}

		/// <summary>
		/// Process initial images. This is most likely when we will be
		/// notify of the creation of a new security.
		/// </summary>
		/// <param name="subscription"></param>
		/// <param name="msg"></param>
		/// <param name="msgType"></param>
		/// <param name="symbol"></param>
		private void handleFullImage(
			MamdaSubscription subscription,
			MamaMsg           msg,
			mamaMsgType       msgType,
			string            symbol)
		{
			if(!mListeners.containsKey(symbol))
			{
				checkSecurityCreateNotify(subscription, symbol);
			}
		}

		private void checkSecurityCreateNotify(
			MamdaSubscription subscription,
			string            symbol)
		{
			mListeners.put(symbol, new ArrayList());
			foreach (MamdaMultiSecurityHandler handler in mHandlers)
			{
				handler.onSecurityCreate(subscription, this, symbol);
			}
		}

		private void forwardMsg(
			MamdaSubscription  subscription,
			MamaMsg            msg,
			mamaMsgType        msgType,
			String             symbol)
		{
			ArrayList securityListeners = (ArrayList)mListeners.valueOf(symbol);
			if ((securityListeners != null) && (securityListeners.Count > 0))
			{
				forwardMsg(
					securityListeners,
					subscription,
					msg,
					msgType);
			}
			else
			{
				/*Needed for new symbols created intraday*/
				checkSecurityCreateNotify(subscription, symbol);
			}
		}

		/// <summary>
		/// Generic method for dispatching messages to instances of
		/// MamaMsgListeners.
		/// </summary>
		/// <param name="listeners"></param>
		/// <param name="subscription"></param>
		/// <param name="msg"></param>
		/// <param name="msgType"></param>
		private void forwardMsg(
			ArrayList           listeners,
			MamdaSubscription   subscription,
			MamaMsg             msg,
			mamaMsgType         msgType)
		{
			foreach (MamdaMsgListener listener in listeners)
			{
				listener.onMsg(subscription, msg, msgType);
			}
		}

		/*Map containing a list of listeners for each security. 
		  Keyed on the symbol*/
		private Map mListeners;

		/*List of MultiSecurityHandlers for creation callback notification*/
		private ArrayList mHandlers;

		/*Group symbol subscribed to*/
		private string mSymbol;
	}
}
