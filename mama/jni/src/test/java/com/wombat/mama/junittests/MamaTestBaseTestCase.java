package com.wombat.mama.junittests;

import com.wombat.mama.Mama;
import com.wombat.mama.MamaBridge;
import com.wombat.mama.MamaQueueGroup;
import junit.framework.TestCase;

import java.security.InvalidKeyException;
import java.util.logging.Level;

public class MamaTestBaseTestCase {

    private MamaBridge bridge = null;

    public MamaBridge getBridge() {
        return bridge;
    }

    public String getBridgeName() {
        return System.getProperty("openmama.bridge", "qpid");
    }

    protected void setUp()
    {
        Mama.setProperty("mama.payload.keeploaded.qpidmsg", "true");
        // Load the bridge
        String bridgeName = getBridgeName();
        bridge = Mama.loadBridge(bridgeName);
        Mama.open();
    }

    protected void tearDown()
    {
        Mama.close();
    }

    public void testBase() {
    }
}
