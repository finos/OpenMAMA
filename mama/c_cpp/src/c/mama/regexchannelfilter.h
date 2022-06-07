#ifndef MamaRegexChannelFilterH__
#define MamaRegexChannelFilterH__

#if defined(__cplusplus)
extern "C"
{
#endif

#include <mama/config.h>
#include <mama/status.h>
#include <mama/types.h>


/**
 * Creates a MAMA regex filter to help filter strings into channels (ints). The
 * channel itself has no special meaning - it's simply a number to produce when
 * each regular expression is matched.
 *
 * The main use case is for load balancing dispatcher queues according to topic
 * names.
 *
 * @param filter A pointer to populate with the newly allocated filter.
 * @result A mama_status value to reflect if this operation was successful.
 */
MAMAExpDLL
extern mama_status
mamaRegexChannelFilter_create (
    mamaRegexChannelFilter* filter
);


/**
 * Destroys a MAMA regex filter and all underlying resources
 *
 * @result A mama_status value to reflect if this operation was successful.
 */
MAMAExpDLL
extern mama_status
mamaRegexChannelFilter_destroy (
    mamaRegexChannelFilter filter
);


/**
 * Sets a default channel to use if no filters produce hits during matching.
 * @param filter  The filter to operate on
 * @param channel The channel to use when no matches are found
 * @result A mama_status value to reflect if this operation was successful.
 */
MAMAExpDLL
extern mama_status
mamaRegexChannelFilter_setDefaultChannel (
    mamaRegexChannelFilter filter,
    int channel
);


/**
 * Adds a regex string and associated integer. During calls to getChannel, the
 * target string will be matched against this regular expression to see which
 * channel matches it.
 *
 * @param filter  The filter to operate on
 * @param regex   The regex string to add
 * @param channel The channel to route to for this regex
 *
 * @result A mama_status value to reflect if this operation was successful.
 */
MAMAExpDLL
extern mama_status
mamaRegexChannelFilter_addRegex(
    mamaRegexChannelFilter filter,
    const char* regex,
    int channel
);


/**
 * Looks up all channel filters to see which channel this target should be
 * routed to.
 *
 * @param filter  The filter to operate on
 * @param target  The string to try matching according to
 * @param channel Will be populated with the channel which should be used
 *
 * @result A mama_status value to reflect if this operation was successful.
 */
MAMAExpDLL
extern mama_status
mamaRegexChannelFilter_getChannel (
    mamaRegexChannelFilter filter,
    const char* target,
    int* channel
);

#if defined(__cplusplus)
}
#endif

#endif /* MamaRegexChannelFilterH__ */