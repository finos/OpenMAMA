
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;

namespace Wombat
{
	/// <summary>
    /// A MamaResourcePool is a convenience wrapper to allow easy subscription and management
    /// of OpenMAMA related resources
	/// </summary>
	public class MamaRegexChannelFilter
	{
		private int mDefaultChannelId = 0;

		private Dictionary<Regex, int> mFilters;

        /// <summary>
        /// Creates a MAMA regex filter to help filter strings into channels (ints). The
        /// channel itself has no special meaning - it's simply a number to produce when
        /// each regular expression is matched.
        /// </summary>
        public MamaRegexChannelFilter ()
        {
	        mFilters = new Dictionary<Regex, int>();
        }

        /// <summary>
        /// Sets a default channel to use if no filters produce hits during matching.
        /// </summary>
        public void setDefaultChannel(int channelId)
        {
	        mDefaultChannelId = channelId;
        }

        /// <summary>
        /// Adds a regex string and associated integer. During calls to getChannel, the
        /// target string will be matched against this regular expression to see which
        /// channel matches it.
        /// </summary>
        public void addRegex (string regexStr, int channelId)
        {
	        mFilters.Add(new Regex(regexStr), channelId);
        }

        /// <summary>
        /// Looks up all channel filters to see which channel this target should be routed to.
        /// </summary>
        public int getChannel (string target)
        {
	        foreach (KeyValuePair<Regex, int> kv in mFilters)
	        {
		        if (kv.Key.IsMatch(target))
		        {
			        return kv.Value;
		        }
	        }
	        return mDefaultChannelId;
        }

	}
}
