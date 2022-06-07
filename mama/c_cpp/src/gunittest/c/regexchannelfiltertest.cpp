#include <gtest/gtest.h>
#include "mama/regexchannelfilter.h"
#include <cstring>

class MamaRegexChannelFilterTestC : public ::testing::Test
{
protected:
    MamaRegexChannelFilterTestC() {}
    ~MamaRegexChannelFilterTestC() {}
    void SetUp() override {};
    void TearDown () override {};
};

TEST_F (MamaRegexChannelFilterTestC, CreateDestroy)
{
    mamaRegexChannelFilter filter;
    ASSERT_EQ (MAMA_STATUS_OK, mamaRegexChannelFilter_create(&filter));
    ASSERT_EQ (MAMA_STATUS_OK, mamaRegexChannelFilter_destroy(filter));
}

TEST_F (MamaRegexChannelFilterTestC, CheckDefaults)
{
    mamaRegexChannelFilter filter;
    ASSERT_EQ (MAMA_STATUS_OK, mamaRegexChannelFilter_create(&filter));
    int channel;
    ASSERT_EQ (MAMA_STATUS_OK, mamaRegexChannelFilter_getChannel (filter, "banana", &channel));
    ASSERT_EQ (0, channel);
    ASSERT_EQ (MAMA_STATUS_OK, mamaRegexChannelFilter_setDefaultChannel(filter, 66));
    ASSERT_EQ (MAMA_STATUS_OK, mamaRegexChannelFilter_getChannel (filter, "banana", &channel));
    ASSERT_EQ (66, channel);
    ASSERT_EQ (MAMA_STATUS_OK, mamaRegexChannelFilter_destroy(filter));
}

TEST_F (MamaRegexChannelFilterTestC, CheckFilteringIncludingMiss)
{
    mamaRegexChannelFilter filter;
    ASSERT_EQ (MAMA_STATUS_OK, mamaRegexChannelFilter_create(&filter));
    ASSERT_EQ (MAMA_STATUS_OK, mamaRegexChannelFilter_addRegex(filter, "^[A-Z]", 1));
    ASSERT_EQ (MAMA_STATUS_OK, mamaRegexChannelFilter_addRegex(filter, "^[a-z]", 2));
    ASSERT_EQ (MAMA_STATUS_OK, mamaRegexChannelFilter_addRegex(filter, "^[0-9]", 3));
    ASSERT_EQ (MAMA_STATUS_OK, mamaRegexChannelFilter_addRegex(filter, "^[0-9]", 3));
    ASSERT_EQ (MAMA_STATUS_OK, mamaRegexChannelFilter_setDefaultChannel(filter, 66));
    int channel;
    ASSERT_EQ (MAMA_STATUS_OK, mamaRegexChannelFilter_getChannel (filter, "banana", &channel));
    ASSERT_EQ (2, channel);
    ASSERT_EQ (MAMA_STATUS_OK, mamaRegexChannelFilter_getChannel (filter, "BANANA", &channel));
    ASSERT_EQ (1, channel);
    ASSERT_EQ (MAMA_STATUS_OK, mamaRegexChannelFilter_getChannel (filter, "6 bananas", &channel));
    ASSERT_EQ (3, channel);
    ASSERT_EQ (MAMA_STATUS_OK, mamaRegexChannelFilter_getChannel (filter, "!bananas :(", &channel));
    ASSERT_EQ (66, channel);
    ASSERT_EQ (MAMA_STATUS_OK, mamaRegexChannelFilter_destroy(filter));
}