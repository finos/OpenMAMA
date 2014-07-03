// GTest & unit test Headers
#include <gtest/gtest.h>
#include "MainUnitTestC.h"

// STL Headers`
#include <map>
#include <string>

// MAMA Headers
#include "mama/middleware.h"
#include "mama/msg.h"

class MamaEnumTestsC : public ::testing::Test
{
protected:
   MamaEnumTestsC();
   virtual ~MamaEnumTestsC();

   virtual void SetUp(void);

   virtual void TearDown(void);

   typedef std::pair <mamaPayloadType, std::string> MamaPayloadPair;
   typedef std::map  <mamaPayloadType, std::string> MamaPayloadMapType;

   typedef std::pair <mamaMiddleware,  std::string> MamaMiddlewarePair;
   typedef std::map  <mamaMiddleware,  std::string> MamaMiddlewareMapType;

   MamaPayloadMapType     payloadTestData;
   MamaMiddlewareMapType  middlewareTestData;

private:   
   void CreateTestData();

};

MamaEnumTestsC::MamaEnumTestsC() : testing::Test() 
{
   CreateTestData();
}


MamaEnumTestsC::~MamaEnumTestsC() {}

void
MamaEnumTestsC::SetUp()
{
}

void
MamaEnumTestsC::TearDown()
{
}

void
MamaEnumTestsC::CreateTestData()
{
    payloadTestData.insert (MamaPayloadPair (MAMA_PAYLOAD_V5,         "V5"));
    payloadTestData.insert (MamaPayloadPair (MAMA_PAYLOAD_AVIS,       "AVIS"));
    payloadTestData.insert (MamaPayloadPair (MAMA_PAYLOAD_TICK42BLP,  "TICK42BLP"));
    payloadTestData.insert (MamaPayloadPair (MAMA_PAYLOAD_FAST,       "FAST"));
    payloadTestData.insert (MamaPayloadPair (MAMA_PAYLOAD_RAI,        "rai"));
    payloadTestData.insert (MamaPayloadPair (MAMA_PAYLOAD_KWANTUM,    "KWANTUM"));
    payloadTestData.insert (MamaPayloadPair (MAMA_PAYLOAD_UMS,        "UMS"));
    payloadTestData.insert (MamaPayloadPair (MAMA_PAYLOAD_INRUSH,     "INRUSH"));
    payloadTestData.insert (MamaPayloadPair (MAMA_PAYLOAD_TICK42RMDS, "TICK42RMDS"));
    payloadTestData.insert (MamaPayloadPair (MAMA_PAYLOAD_QPID,       "QPID"));
    payloadTestData.insert (MamaPayloadPair (MAMA_PAYLOAD_TIBRV,      "TIBRV"));
    payloadTestData.insert (MamaPayloadPair (MAMA_PAYLOAD_IBMWFO,     "ibmwfo"));
    payloadTestData.insert (MamaPayloadPair (MAMA_PAYLOAD_ACTIV,      "activ"));
    payloadTestData.insert (MamaPayloadPair (MAMA_PAYLOAD_VULCAN,     "Vulcan"));
    payloadTestData.insert (MamaPayloadPair (MAMA_PAYLOAD_WOMBAT_MSG, "WombatMsg"));
    payloadTestData.insert (MamaPayloadPair (MAMA_PAYLOAD_EXEGY,      "EXEGY"));
    payloadTestData.insert (MamaPayloadPair (MAMA_PAYLOAD_SOLACE,     "solacemsg"));
    payloadTestData.insert (MamaPayloadPair (MAMA_PAYLOAD_UNKNOWN,    "unknown"));

    middlewareTestData.insert (MamaMiddlewarePair (MAMA_MIDDLEWARE_WMW,       "wmw"));
    middlewareTestData.insert (MamaMiddlewarePair (MAMA_MIDDLEWARE_LBM,       "lbm"));
    middlewareTestData.insert (MamaMiddlewarePair (MAMA_MIDDLEWARE_TIBRV,     "tibrv"));
    middlewareTestData.insert (MamaMiddlewarePair (MAMA_MIDDLEWARE_AVIS,      "AVIS"));
    middlewareTestData.insert (MamaMiddlewarePair (MAMA_MIDDLEWARE_TICK42BLP, "tick42blp"));
    middlewareTestData.insert (MamaMiddlewarePair (MAMA_MIDDLEWARE_SOLACE,    "SOLACE"));
    middlewareTestData.insert (MamaMiddlewarePair (MAMA_MIDDLEWARE_RAI,       "rai"));
    middlewareTestData.insert (MamaMiddlewarePair (MAMA_MIDDLEWARE_QPID,      "QPID"));
    middlewareTestData.insert (MamaMiddlewarePair (MAMA_MIDDLEWARE_EXEGY,     "exegy"));
    middlewareTestData.insert (MamaMiddlewarePair (MAMA_MIDDLEWARE_IBMWFO,    "ibmwfo"));
    middlewareTestData.insert (MamaMiddlewarePair (MAMA_MIDDLEWARE_ACTIV,     "activ"));
    middlewareTestData.insert (MamaMiddlewarePair (MAMA_MIDDLEWARE_TICK42RMDS, "tick42rmds"));
    middlewareTestData.insert (MamaMiddlewarePair (MAMA_MIDDLEWARE_UMS,       "ums"));
    middlewareTestData.insert (MamaMiddlewarePair (MAMA_MIDDLEWARE_VULCAN,    "vulcan"));
    middlewareTestData.insert (MamaMiddlewarePair (MAMA_MIDDLEWARE_INRUSH,    "inrush"));
    middlewareTestData.insert (MamaMiddlewarePair (MAMA_MIDDLEWARE_LBMKOMODO, "lbmkomodo"));
    middlewareTestData.insert (MamaMiddlewarePair (MAMA_MIDDLEWARE_UNKNOWN,   "unknown"));
}


TEST_F (MamaEnumTestsC, testPayloadConvertToString)
{
    MamaPayloadMapType::iterator itr;
    bool passed = true;

    for(itr = payloadTestData.begin(); itr != payloadTestData.end(); ++itr) {
    
       mamaPayloadType payload   = (*itr).first;
       std::string     expected  = (*itr).second;

       std::string actual = mamaPayload_convertToString (payload); 

       EXPECT_STREQ(actual.c_str(), expected.c_str());

       if (actual != expected) 
           passed = false;
    }

    ASSERT_EQ( passed, true );
}

TEST_F (MamaEnumTestsC, testMiddlewareConvertToString)
{
    MamaMiddlewareMapType::iterator itr;
    bool passed = true;

    for(itr = middlewareTestData.begin(); itr != middlewareTestData.end(); ++itr) {
    
       mamaMiddleware middleware = (*itr).first;
       std::string    expected   = (*itr).second;

       std::string actual = mamaMiddleware_convertToString (middleware); 

       EXPECT_STREQ(actual.c_str(), expected.c_str());

       if (actual != expected) 
           passed = false;
    }

    ASSERT_EQ (passed, true);
}

TEST_F (MamaEnumTestsC, testMiddlewareConvertFromString)
{
    MamaMiddlewareMapType::iterator itr;
    bool passed = true;

    for (itr = middlewareTestData.begin(); itr != middlewareTestData.end(); itr++) {

        mamaMiddleware expected    = (*itr).first;
        std::string    middleware  = (*itr).second;

        mamaMiddleware actual      = mamaMiddleware_convertFromString (middleware.c_str());

        EXPECT_EQ (actual, expected);

        if (actual != expected)
            passed = false;
    }

    ASSERT_EQ (passed, true);
}
