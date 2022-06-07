#ifndef OPENMAMA_MAMAREGEXCHANNELFILTER_H
#define OPENMAMA_MAMAREGEXCHANNELFILTER_H

#include <vector>
#include <regex>

// LIBC regex is only a last resort for older compilers. Regex support in C++11
// was basically a no-op prior to gcc 4.9 hence this fallback to support
// CentOS 7.
#ifdef REQUIRES_LIBC_REGEX
#include "regex.h"
#endif

namespace Wombat
{
class MamaRegexChannelFilter
{
  public:
    /**
     * Creates a MAMA regex filter to help filter strings into channels (ints). The
     * channel itself has no special meaning - it's simply a number to produce when
     * each regular expression is matched.
     *
     * The main use case is for load balancing dispatcher queues according to topic
     * names.
     *
     * @result A mama_status value to reflect if this operation was successful.
     */
    MamaRegexChannelFilter() : mDefaultChannelId(0)
    {
    }

    /**
     * Sets a default channel to use if no filters produce hits during matching.
     * @param channel The channel to use when no matches are found
     * @result A mama_status value to reflect if this operation was successful.
     */
    void setDefaultChannel (int channelId) {
        mDefaultChannelId = channelId;
    }

    /**
     * Adds a regex string and associated integer. During calls to getChannel, the
     * target string will be matched against this regular expression to see which
     * channel matches it.
     *
     * @param regex   The regex string to add
     * @param channel The channel to route to for this regex
     *
     * @result A mama_status value to reflect if this operation was successful.
     */
    void addRegex (const char* regexStr, int channelId) {
#ifndef REQUIRES_LIBC_REGEX
        mFilters.emplace_back(std::regex(regexStr), channelId);
#else
        regex_t re;
        regcomp(&re, regexStr, REG_EXTENDED);
        mFilters.emplace_back(re, channelId);
#endif
    }

    /**
     * Looks up all channel filters to see which channel this target should be
     * routed to.
     *
     * @param target  The string to try matching according to
     *
     * @result A mama_status value to reflect if this operation was successful.
     */
    int getChannel (const char* target)
    {
        for (const auto& pair : mFilters)
        {
#ifndef REQUIRES_LIBC_REGEX
            if (std::regex_search(target, pair.first))
#else
            if (0 == regexec(&pair.first, target, 0, NULL, 0))
#endif
            {
                return pair.second;
            }
        }
        return mDefaultChannelId;
    }

  private:
#ifndef REQUIRES_LIBC_REGEX
    std::vector<std::pair<std::regex, int>> mFilters;
#else
    std::vector<std::pair<regex_t, int>> mFilters;
#endif
    int mDefaultChannelId;
};
}

#endif // OPENMAMA_MAMAREGEXCHANNELFILTER_H
