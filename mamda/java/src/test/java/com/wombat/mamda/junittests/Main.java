package com.wombat.mamda.junittests;

import com.wombat.mama.*;
import com.wombat.mamda.junittests.orderBooks.MamdaBookPublisherTests;
import com.wombat.mamda.junittests.orderImbalances.MamdaOrderImbalanceCallBackTests;
import com.wombat.mamda.junittests.quotes.MamdaQuoteTests;
import com.wombat.mamda.junittests.secstatus.MamdaSecStatusShortSaleTests;
import com.wombat.mamda.junittests.secstatus.MamdaSecStatusTests;
import com.wombat.mamda.junittests.secstatus.MamdaSecurityStatusQualTests;
import com.wombat.mamda.junittests.trades.MamdaTradeIdTests;
import com.wombat.mamda.junittests.trades.MamdaTradeShortSaleTests;
import com.wombat.mamda.junittests.trades.MamdaTradeSideTests;
import org.junit.runner.JUnitCore;
import org.junit.runner.Result;
import org.junit.runner.notification.Failure;

import java.util.logging.Level;

/**
 *
 * This is the main class for the test suite.
 */
public class Main
{
    // Default values
    private static String defaultMiddleware = "qpid";
    private static String defaultDictionary = "../../../OpenMAMA-testdata/dictionaries/data.dict";

    public static String getMiddleware() {
        String middleware = System.getProperty("middleware");
        return middleware == null ? defaultMiddleware : middleware;
    }

    public static String getDictionaryFile() {
        String dictionary = System.getProperty("dictionary");
        return dictionary == null ? defaultDictionary : dictionary;
    }

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args)
    {
        // Each test will load/unload Mama as needed
        Level level = Mama.getLogLevel();            // force load of the shared libs

        // Run the test suite
        Result result = JUnitCore.runClasses(
                MamdaBookPublisherTests.class,
                MamdaOrderImbalanceCallBackTests.class,
                MamdaQuoteTests.class,
                MamdaSecStatusShortSaleTests.class,
                MamdaSecStatusTests.class,
                MamdaSecurityStatusQualTests.class,
                MamdaTradeIdTests.class,
                MamdaTradeShortSaleTests.class,
                MamdaTradeSideTests.class
        );
        for (Failure failure : result.getFailures()) {
            System.out.println(failure.toString());
        }
        System.out.println("Result=="+result.wasSuccessful());

        System.exit(0);                                                // TODO temporary - not exiting JVM - daemon thread?
    }
}
