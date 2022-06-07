package com.wombat.mama.junittests;

import com.wombat.mama.MamaRegexChannelFilter;
import org.junit.Test;

import static org.junit.Assert.assertEquals;

public class MamaRegexChannelFilterTest {
    @Test
    public void testCheckDefaults() {
        MamaRegexChannelFilter filter = new MamaRegexChannelFilter();
        assertEquals(0, filter.getChannel("banana"));
        filter.setDefaultChannel(66);
        assertEquals(66, filter.getChannel("banana"));

    }
    @Test
    public void testCheckFilteringIncludingMiss() {
        MamaRegexChannelFilter filter = new MamaRegexChannelFilter();
        filter.addRegex ("^[A-Z]", 1);
        filter.addRegex ("^[a-z]", 2);
        filter.addRegex ("^[0-9]", 3);
        filter.addRegex ("^[0-9]", 3);
        filter.setDefaultChannel (66);
        assertEquals (2, filter.getChannel("banana"));
        assertEquals (1, filter.getChannel("BANANA"));
        assertEquals (3, filter.getChannel("6 bananas"));
        assertEquals (66, filter.getChannel("!bananas :("));
    }
}
