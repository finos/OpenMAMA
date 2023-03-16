package com.wombat.mama;

import java.util.HashMap;
import java.util.Map;
import java.util.regex.Pattern;

public class MamaRegexChannelFilter
{
    private int mDefaultChannelId = 0;

    private final Map<Pattern, Integer> mFilters = new HashMap<>();

    /**
     * Creates a MAMA regex filter to help filter strings into channels (ints). The
     * channel itself has no special meaning - it's simply a number to produce when
     * each regular expression is matched.
     *
     * The main use case is for load balancing dispatcher queues according to topic
     * names.
     */
    public MamaRegexChannelFilter ()
    {
    }

    /**
     * Sets a default channel to use if no filters produce hits during matching.
     *
     * @param channelId The channel to use when no matches are found
     */
    public void setDefaultChannel(int channelId)
    {
        mDefaultChannelId = channelId;
    }

    /**
     * Adds a regex string and associated integer. During calls to getChannel, the
     * target string will be matched against this regular expression to see which
     * channel matches it.
     *
     * @param regexStr   The regex string to add
     * @param channelId  The channel to route to for this regex
     */
    public void addRegex (final String regexStr, final int channelId)
    {
        mFilters.put(Pattern.compile(regexStr), channelId);
    }

    /**
     * Looks up all channel filters to see which channel this target should be
     * routed to.
     *
     * @param target  The string to try matching according to
     */
    public int getChannel (final String target)
    {
        for (Map.Entry<Pattern, Integer> entry : mFilters.entrySet()) {
            if (entry.getKey().matcher(target).find()) {
                return entry.getValue();
            }
        }
        return mDefaultChannelId;
    }
}
