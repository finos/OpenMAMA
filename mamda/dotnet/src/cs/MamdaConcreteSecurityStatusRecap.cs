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
	public class MamdaConcreteSecurityStatusRecap : MamdaSecurityStatusRecap
	{
		/// <summary>
		/// Clear the recap data
		/// </summary>
		public void clear()
		{
			mSecurityStatus              = 0;
			mSecurityStatusEnum			 = MamdaSecurityStatus.mamdaSecurityStatus.SECURITY_STATUS_NONE;
			mSecurityStatusQualifier     = 0;
			mSecurityStatusQualifierEnum = MamdaSecurityStatusQual.mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NONE;
			mSecurityStatusStr           = null;
            mShortSaleCircuitBreaker     = ' ';
			mSecurityStatusQualifierStr  = null;
			mActTime                     = DateTime.MinValue;
			mSrcTime                     = DateTime.MinValue;
            mReason                      = null;
		}
    
		/// <summary>
		/// Returns the security status 
		/// <see cref="MamdaSecurityStatusUpdate.getSecurityStatus()"/>
		/// </summary>
		/// <returns></returns>
		public long getSecurityStatus()
		{
			return mSecurityStatus;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getSecurityStatusFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		/// Set the security status
		/// <see cref="MamdaSecurityStatusUpdate.getSecurityStatus()"/>
		/// </summary>
		/// <param name="securityStatus"></param>
		public void setSecurityStatus(long securityStatus)
		{
			mSecurityStatus = securityStatus;
		}

		/// <summary>
		/// Returns the security	
		/// <see cref="MamdaSecurityStatusUpdate.getSecurityStatus()"/>
		/// </summary>
		/// <returns></returns>
		public long getSecurityStatusQualifier()
		{
			return mSecurityStatusQualifier;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getSecurityStatusQualifierFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		/// <see cref="MamdaSecurityStatusUpdate.getSecurityStatus()"/>
		/// </summary> 
		/// <param name="secQualifier"></param>
		public void setSecurityStatusQualifier(long secQualifier)
		{
			mSecurityStatusQualifier = secQualifier;
		}

		/// <summary>
		/// <see cref="MamdaSecurityStatusUpdate.getSecurityStatusEnum()"/>
		/// </summary>
		/// <returns></returns>
		public MamdaSecurityStatus.mamdaSecurityStatus getSecurityStatusEnum()
		{
			return mSecurityStatusEnum;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getSecurityStatusEnumFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		/// <see cref="MamdaSecurityStatusUpdate.getSecurityStatusEnum()"/>
		/// </summary>
		/// <returns</returns>
		public void setSecurityStatusEnum(MamdaSecurityStatus.mamdaSecurityStatus secStatus)
		{
			mSecurityStatusEnum = secStatus;
		}

		/// <summary>
		/// <see cref="MamdaSecurityStatusUpdate.getSecurityStatusQualifierEnum()"/>
		/// </summary>
		/// <returns></returns>
		public MamdaSecurityStatusQual.mamdaSecurityStatusQual getSecurityStatusQualifierEnum()
		{
			return mSecurityStatusQualifierEnum;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getSecurityStatusQualifierEnumFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		/// <see cref="MamdaSecurityStatusUpdate.getSecurityStatusQualifierEnum()"/>
		/// </summary>
		/// <param name="secStatusQual"></param>
		public void setSecurityStatusQualifierEnum(MamdaSecurityStatusQual.mamdaSecurityStatusQual secStatusQual)
		{
			mSecurityStatusQualifierEnum = secStatusQual;
		}

		/// <summary>
		/// Returns the security status string
		/// <see cref="MamdaSecurityStatusUpdate.getSecurityStatusQualifier()"/>
		/// </summary>
		/// <returns></returns>
		public string getSecurityStatusStr()
		{
			return mSecurityStatusStr;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getSecurityStatusStrFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

        /// <summary>
        /// Returns the short sale CircuitBreaker
        /// <see cref="MamdaSecurityStatusUpdate.getShortSaleCircuitBreaker()"/>
        /// </summary>
        /// <returns></returns>
        public char getShortSaleCircuitBreaker()
        {
            return mShortSaleCircuitBreaker;
        }

        /// <summary>
        /// Set the short sale Circuit Breaker.
        /// <see cref="MamdaSecurityStatusUpdate.getShortSaleCircuitBreakerFieldState()"/>
        /// </summary>
        /// <param name="shortSaleCircuitBreaker"></param>
        public void setShortSaleCircuitBreaker(char shortSaleCircuitBreaker)
        {
            mShortSaleCircuitBreaker = shortSaleCircuitBreaker;
        }

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getShortSaleCircuitBreakerFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

        public string getSecurityStatusOrigStr()
        {
            return mSecurityStatusOrigStr;
        }

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getSecurityStatusOrigStrFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

        public void setSecurityStatusOrigStr(string securityStatusOrigStr)
		{
			if (securityStatusOrigStr != null)
				mSecurityStatusOrigStr = securityStatusOrigStr;
		}
        
		/// <summary>
		/// Set the security status string
		/// <see cref="MamdaSecurityStatusUpdate.getSecurityStatusQualifier()"/>
		/// </summary>
		/// <param name="securityStatusStr"></param>
		public void setSecurityStatusStr(string securityStatusStr)
		{
			if (securityStatusStr != null)
				mSecurityStatusStr = securityStatusStr;
		}

		/// <summary>
		/// Returns the security status qualifier string
		/// <see cref="MamdaSecurityStatusUpdate.getSecurityStatusQualifier()"/>
		/// </summary>
		/// <returns></returns>
		public string getSecurityStatusQualifierStr()
		{
        
			return mSecurityStatusQualifierStr;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getSecurityStatusQualifierStrFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		/// Set the security status qualifier string
		/// <see cref="MamdaSecurityStatusUpdate.getSecurityStatusQualifier()"/>
		/// </summary>
		/// <param name="securityStatusQualifierStr"></param>
		public void setSecurityStatusQualifierStr (
			String securityStatusQualifierStr)
		{
			if (securityStatusQualifierStr != null)
				mSecurityStatusQualifierStr = securityStatusQualifierStr;
		}
        
        ///< summary>
        /// Returns the security status reason
        /// <see cref="MamdaSecurityStatusRecap.getReason()"/>
        /// </summary>
        /// <returns></returns>
        public string getReason()
        {
            return mReason;
        }

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getReasonFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

        ///<summary>
        /// Set the security status reason
        /// <see cref="MamdaSecurityStatusRecap.getReason()"/>
        /// </summary>
        /// <param name="reason"></param>
        public void setReason (
            string reason)
        {
            if (reason != null)
                mReason = reason;
        }

		/// <summary>
		/// <see cref="MamdaBasicRecap.getActivityTime"/>
		/// </summary>
		/// <returns></returns>
		public DateTime getActivityTime()
		{
			return mActTime;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getActivityTimeFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		/// 
		/// </summary>
		/// <param name="actTime"></param>
		public void setActivityTime(DateTime actTime) 
		{
			if (actTime != DateTime.MinValue)
				mActTime = actTime;
		}

		public void setSrcTime(DateTime srcTime)
		{
			if (srcTime != DateTime.MinValue)
				mSrcTime = srcTime;  
		}

		public DateTime getSrcTime()
		{
			return mSrcTime ;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getSrcTimeFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		private long mSecurityStatus;
		private long mSecurityStatusQualifier;
		private MamdaSecurityStatus.mamdaSecurityStatus mSecurityStatusEnum;
		private MamdaSecurityStatusQual.mamdaSecurityStatusQual mSecurityStatusQualifierEnum;
		private string mSecurityStatusStr;
        private char mShortSaleCircuitBreaker;
        private string mSecurityStatusOrigStr;
		private string mSecurityStatusQualifierStr;
		private DateTime mActTime = DateTime.MinValue;
		private DateTime mSrcTime = DateTime.MinValue;
        private string mReason = null;
	}
}
