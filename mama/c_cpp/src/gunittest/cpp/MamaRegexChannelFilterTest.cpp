#include <gtest/gtest.h>
#include "mama/mama.h"
#include "wombat/property.h"
#include "mama/MamaRegexChannelFilter.h"
#include <cstring>

using namespace Wombat;

class MamaRegexChannelFilterTestCPP : public ::testing::Test
{	
protected:
    MamaRegexChannelFilterTestCPP () {};
    ~MamaRegexChannelFilterTestCPP () {};

    void SetUp() override {};
    void TearDown () override {};

};

TEST_F (MamaRegexChannelFilterTestCPP, CreateDestroy)
{
    // No crashing expected
    auto* filter = new MamaRegexChannelFilter;
    delete filter;
}

TEST_F (MamaRegexChannelFilterTestCPP, CheckDefaults)
{
    auto* filter = new MamaRegexChannelFilter;
    ASSERT_EQ (0, filter->getChannel("banana"));
    filter->setDefaultChannel(66);
    ASSERT_EQ (66, filter->getChannel("banana"));
    delete filter;
}

TEST_F (MamaRegexChannelFilterTestCPP, CheckFilteringIncludingMiss)
{
    auto* filter = new MamaRegexChannelFilter;
    filter->addRegex ("^[A-Z]", 1);
    filter->addRegex ("^[a-z]", 2);
    filter->addRegex ("^[0-9]", 3);
    filter->addRegex ("^[0-9]", 3);
    filter->setDefaultChannel (66);
    ASSERT_EQ (2, filter->getChannel("banana"));
    ASSERT_EQ (1, filter->getChannel("BANANA"));
    ASSERT_EQ (3, filter->getChannel("6 bananas"));
    ASSERT_EQ (66, filter->getChannel("!bananas :("));
    delete filter;
}