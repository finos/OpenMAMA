#include <cstdlib>
#include <iostream>
#include <gtest/gtest.h>
#include <string>

#ifndef WOMBAT_CONFIG_NO_NAMESPACES
 using std::string;
#endif

#if !defined(VERSIONSTR)
# define VERSIONSTR    "?.?.?"
# define DATESTR       "date"
# define BUILDSTR      ""
#endif

static string version     ("APPNAMESTR:  Version " VERSIONSTR
                           "  Date " DATESTR "  Build " BUILDSTR);


static const char*       gMiddleware         = "wmw";


const char* getMiddleware (void)
{
	return gMiddleware;
}

static void parseCommandLine (int argc, char** argv)
{
    int i = 1;


    for (i = 1; i < argc;)
    {
		if (strcmp ("-m", argv[i]) == 0)
		{
			gMiddleware = argv[i+1];
			i += 2;
		}
        else
        {
           //unknown arg
            i++;
        }
    }
}


int main(int argc, char **argv)
{
    // create all tests, passing in command line options
    // command line options can be google test command line options
    // e.g. --gtest_filter, --gtest_repeat, etc
    // see google test framework docs for details
    ::testing::InitGoogleTest(&argc, argv);

    parseCommandLine(argc, argv);


    // run all tests
    return RUN_ALL_TESTS();

}

