#include <stdint.h>
#include "mama/regexchannelfilter.h"
#include "../cpp/mama/MamaRegexChannelFilter.h"

using namespace Wombat;

typedef struct mamaRegexChannelFilterImpl_
{
    Wombat::MamaRegexChannelFilter x;
} mamaRegexChannelFilterImpl;

MAMAExpDLL
extern mama_status
mamaRegexChannelFilter_create (
    mamaRegexChannelFilter* filter
) {
    if (filter == NULL) return MAMA_STATUS_NULL_ARG;
    *filter = reinterpret_cast<mamaRegexChannelFilter> (new MamaRegexChannelFilter());
    return MAMA_STATUS_OK;
}

MAMAExpDLL
extern mama_status
mamaRegexChannelFilter_destroy (
    mamaRegexChannelFilter filter
) {
    if (filter == NULL) return MAMA_STATUS_NULL_ARG;
    delete reinterpret_cast<MamaRegexChannelFilter*>(filter);
    return MAMA_STATUS_OK;
}

MAMAExpDLL
extern mama_status
mamaRegexChannelFilter_setDefaultChannel (
    mamaRegexChannelFilter filter,
    int channel
) {
    if (filter == NULL) return MAMA_STATUS_NULL_ARG;
    auto impl = reinterpret_cast<MamaRegexChannelFilter*>(filter);
    impl->setDefaultChannel (channel);
    return MAMA_STATUS_OK;
}

MAMAExpDLL
extern mama_status
mamaRegexChannelFilter_addRegex(
    mamaRegexChannelFilter filter,
    const char* regex,
    int channel
) {
    if (filter == NULL) return MAMA_STATUS_NULL_ARG;
    auto impl = reinterpret_cast<MamaRegexChannelFilter*>(filter);
    impl->addRegex (regex, channel);
    return MAMA_STATUS_OK;
}

MAMAExpDLL
extern mama_status
mamaRegexChannelFilter_getChannel (
    mamaRegexChannelFilter filter,
    const char* target,
    int* channel
) {
    if (filter == NULL) return MAMA_STATUS_NULL_ARG;
    auto impl = reinterpret_cast<MamaRegexChannelFilter*>(filter);
    *channel = impl->getChannel (target);
    return MAMA_STATUS_OK;
}