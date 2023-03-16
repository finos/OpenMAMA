using NUnit.Framework;
using Wombat;

namespace NUnitTest
{
    public class MamaRegexChannelFilterTest
    {
        [Test]
        public void TestCheckDefaults() {
            MamaRegexChannelFilter filter = new MamaRegexChannelFilter();
            Assert.AreEqual(0, filter.getChannel("banana"));
            filter.setDefaultChannel(66);
            Assert.AreEqual(66, filter.getChannel("banana"));

        }

        [Test]
        public void TestCheckFilteringIncludingMiss() {
            MamaRegexChannelFilter filter = new MamaRegexChannelFilter();
            filter.addRegex ("^[A-Z]", 1);
            filter.addRegex ("^[a-z]", 2);
            filter.addRegex ("^[0-9]", 3);
            filter.addRegex ("^[0-9]", 3);
            filter.setDefaultChannel (66);
            Assert.AreEqual(2, filter.getChannel("banana"));
            Assert.AreEqual(1, filter.getChannel("BANANA"));
            Assert.AreEqual(3, filter.getChannel("6 bananas"));
            Assert.AreEqual(66, filter.getChannel("!bananas :("));
        }
    }
}