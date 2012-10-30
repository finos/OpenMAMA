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
	/// MamdaTradeHandler is an interface for applications that want to
	/// have an easy way to handle trade updates.  The interface defines
	/// callback methods for different types of trade-related events:
	/// trades, errors/cancels, corrections, recaps and closing reports.
	/// </summary>
	public interface MamdaTradeHandler
	{
		/// <summary>
		/// Method invoked when the current last-trade information for the
		/// security is available.  The reason for the invocation may be
		/// any of the following:
		/// - Initial image.
		/// - Recap update (e.g., after server fault tolerant event or data
		///   quality event.)
		/// - After stale status removed.
		/// </summary>
		/// <param name="subscription">The subscription which received the update.</param>
		/// <param name="listener">The listener which invoked this callback.</param>
		/// <param name="msg">The MamaMsg that triggered this invocation.</param>
		/// <param name="recap">Access to the trade fields in the recap update.</param>
		void onTradeRecap(
			MamdaSubscription   subscription,
			MamdaTradeListener  listener,
			MamaMsg             msg,
			MamdaTradeRecap     recap);

		/// <summary>
		/// Method invoked when a trade is reported.
		/// </summary>
		/// <param name="subscription">The subscription which received the update.</param>
		/// <param name="listener">The listener which invoked this callback.</param>
		/// <param name="msg">The MamaMsg that triggered this invocation.</param>
		/// <param name="reportEvent">Access to the trade data from the update.</param>
		/// <param name="recap">Access to complete trade data.</param>
		void onTradeReport(
			MamdaSubscription   subscription,
			MamdaTradeListener  listener,
			MamaMsg             msg,
			MamdaTradeReport    reportEvent,
			MamdaTradeRecap     recap);

		/// <summary>
		/// Method invoked when a gap in trade reports is discovered.
		/// </summary>
		/// <param name="subscription">The subscription which received the update.</param>
		/// <param name="listener">The listener which invoked this callback.</param>
		/// <param name="msg">The MamaMsg that triggered this invocation.</param>
		/// <param name="gapEvent">Access to details from the trade gap event.</param>
		/// <param name="recap">Access to complete trade data.</param>
		void onTradeGap(
			MamdaSubscription   subscription,
			MamdaTradeListener  listener,
			MamaMsg             msg,
			MamdaTradeGap       gapEvent,
			MamdaTradeRecap     recap);

		/// <summary>
		/// Method invoked when a trade cancel or error is reported.
		/// </summary>
		/// <param name="subscription">The subscription which received the update.</param>
		/// <param name="listener">The listener which invoked this callback.</param>
		/// <param name="msg">The MamaMsg that triggered this invocation.</param>
		/// <param name="cancelOrErrorEvent">Access to the details from the cancel/error event.</param>
		/// <param name="recap">Access to complete trade data.</param>
		void onTradeCancelOrError(
			MamdaSubscription        subscription,
			MamdaTradeListener       listener,
			MamaMsg                  msg,
			MamdaTradeCancelOrError  cancelOrErrorEvent,
			MamdaTradeRecap          recap);

		/// <summary>
		/// Method invoked when a trade correction is reported.
		/// </summary>
		/// <param name="subscription">The subscription which received the update.</param>
		/// <param name="listener">The listener which invoked this callback.</param>
		/// <param name="msg">The MamaMsg that triggered this invocation.</param>
		/// <param name="correctionEvent">Access to the details from the trade correction event.</param>
		/// <param name="recap">Access to complete trade data.</param>
		void onTradeCorrection(
			MamdaSubscription        subscription,
			MamdaTradeListener       listener,
			MamaMsg                  msg,
			MamdaTradeCorrection     correctionEvent,
			MamdaTradeRecap          recap);

		/// <summary>
		/// Method invoked for a closing report.
		/// </summary>
		/// <param name="subscription">The subscription which received the update.</param>
		/// <param name="listener">The listener which invoked this callback.</param>
		/// <param name="msg">The MamaMsg that triggered this invocation.</param>
		/// <param name="closingEvent">Access to the details from the trade closing event.</param>
		/// <param name="recap">Access to complete trade data.</param>
		void onTradeClosing(
			MamdaSubscription        subscription,
			MamdaTradeListener       listener,
			MamaMsg                  msg,
			MamdaTradeClosing        closingEvent,
			MamdaTradeRecap          recap);
	}
}

