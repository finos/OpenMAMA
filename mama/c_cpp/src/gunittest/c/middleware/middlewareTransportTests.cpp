/* $Id: middlewareTransportTests.cpp,v 1.1.2.4 2012/12/11 16:24:23 matthewmulhern Exp $
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Technologies, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include <gtest/gtest.h>
#include "mama/mama.h"
#include "MainUnitTestC.h"
#include <iostream>
#include "bridge.h"
#include "mama/types.h"

using std::cout;
using std::endl;


class MiddlewareTransportTests : public ::testing::Test
{
protected:
    MiddlewareTransportTests(void);
    virtual ~MiddlewareTransportTests(void);

    virtual void SetUp(void);
    virtual void TearDown(void);

    mamaBridge      mBridge;
    transportBridge tport;
    const char*     name;
    const char*     parentName;
    mamaTransport   parentTport;
};

MiddlewareTransportTests::MiddlewareTransportTests(void)
    : tport (NULL),
      name  ("test_tport"),
      parentName ("test_tport"),
      parentTport (NULL)
{

    mama_loadBridge (&mBridge, getMiddleware());
    mamaTransport_allocate (&parentTport);

    mamaTransport_create (parentTport, parentName, mBridge);
    mamaTransport_getNativeTransport (parentTport, 0, (void**)&tport);
}

MiddlewareTransportTests::~MiddlewareTransportTests(void)
{
    mamaTransport_destroy (parentTport);
}

void MiddlewareTransportTests::SetUp(void)
{
}

void MiddlewareTransportTests::TearDown(void)
{
}



/*===================================================================
 =               mamaTransport bridge functions                      =
 ====================================================================*/

TEST_F (MiddlewareTransportTests, isValidInvalid)
{
    ASSERT_EQ(0, mBridge->bridgeMamaTransportIsValid(NULL));
}

TEST_F (MiddlewareTransportTests, isValid)
{
    ASSERT_EQ(1,mBridge->bridgeMamaTransportIsValid(tport));
}

TEST_F (MiddlewareTransportTests, destroyInvalid)
{
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportDestroy(NULL));
}

TEST_F (MiddlewareTransportTests, createInvalidTransportBridge)
{
    const char*     name      = (char*) NOT_NULL;
    mamaTransport   mamaTport = (mamaTransport) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportCreate(NULL,name,
                                                  mamaTport));
}

TEST_F (MiddlewareTransportTests, createInvalidName)
{
    transportBridge result    = (transportBridge) NOT_NULL;
    mamaTransport   mamaTport = (mamaTransport)   NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportCreate(&result,NULL,
                                                  mamaTport));
}

TEST_F (MiddlewareTransportTests, createInvalidTport)
{
    transportBridge result    = (transportBridge) NOT_NULL;
    const char*     name      = (char*)           NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportCreate(&result,name,
                                                  NULL));
}

TEST_F (MiddlewareTransportTests, forceClientDisconnectInvalidTransportBridge)
{
    int              numTransports =                   0;
    const char*      ipAddress     = (char*)           NOT_NULL;
    uint16_t         port          =                   80;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportForceClientDisconnect(NULL,
                                                                 numTransports,
                                                                 ipAddress,
                                                                 port));
}

TEST_F (MiddlewareTransportTests, forceClientDisconnect)
{
    int             numTransports = 1;
    const char*     ip            = "127.0.0.1";
    uint16_t        port          = 80;

    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaTransportForceClientDisconnect(&tport,
                                                                 numTransports,
                                                                 ip,
                                                                 port));
}


TEST_F (MiddlewareTransportTests, forceClientDisconnectInvalidNumTransports)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    const char*      ipAddress     = (char*)           NOT_NULL;
    uint16_t         port          =                   80;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportForceClientDisconnect(&transports,
                                                                 NULL,
                                                                 ipAddress,
                                                                 port));
}

TEST_F (MiddlewareTransportTests, forceClientDisconnectInvalidIPAddress)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    int              numTransports =                   0;
    uint16_t         port          =                   80;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportForceClientDisconnect(&transports,
                                                                 numTransports,
                                                                 NULL,
                                                                 port));
}

TEST_F (MiddlewareTransportTests, forceClientDisconnectInvalidPort)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    int              numTransports =                   0;
    const char*      ipAddress     = (char*)           NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportForceClientDisconnect(&transports,
                                                                 numTransports,
                                                                 ipAddress,
                                                                 NULL));
}

TEST_F (MiddlewareTransportTests, findConnection)
{
    int             numTransports = 1;
    const char*     ip            = "127.0.0.1";
    uint16_t        port          = 80;
    mamaConnection result         = NULL;

    ASSERT_EQ (MAMA_STATUS_OK,
               mBridge->bridgeMamaTransportFindConnection(&tport,
                                                          numTransports,
                                                          &result,
                                                          ip,
                                                          port));
}

TEST_F (MiddlewareTransportTests, findConnectionInvalidTransportBridge)
{
    int              numTransports =                   0;
    mamaConnection   result        = (mamaConnection)  NOT_NULL;
    const char*      ipAddress     = (char*)           NOT_NULL;
    uint16_t         port          =                   80;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportFindConnection(NULL,
                                                          numTransports,
                                                          &result,
                                                          ipAddress,
                                                          port));
}

TEST_F (MiddlewareTransportTests, findConnectionInvalidNumTransports)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    mamaConnection   result        = (mamaConnection)  NOT_NULL;
    const char*      ipAddress     = (char*)           NOT_NULL;
    uint16_t         port          =                   80;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportFindConnection(&transports,
                                                          NULL,
                                                          &result,
                                                          ipAddress,
                                                          port));
}

TEST_F (MiddlewareTransportTests, findConnectionInvalidResult)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    int              numTransports =                   0;
    const char*      ipAddress     = (char*)           NOT_NULL;
    uint16_t         port          =                   80;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportFindConnection(&transports,
                                                          numTransports,
                                                          NULL,
                                                          ipAddress,
                                                          port));
}

TEST_F (MiddlewareTransportTests, findConnectionInvalidIPAddress)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    int              numTransports =                   0;
    mamaConnection   result        = (mamaConnection)  NOT_NULL;
    uint16_t         port          =                   80;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportFindConnection(&transports,
                                                          numTransports,
                                                          &result,
                                                          NULL,
                                                          port));
}

TEST_F (MiddlewareTransportTests, findConnectionInvalidPort)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    int              numTransports =                   0;
    mamaConnection   result        = (mamaConnection)  NOT_NULL;
    const char*      ipAddress     = (char*)           NOT_NULL;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportFindConnection(&transports,
                                                          numTransports,
                                                          &result,
                                                          ipAddress,
                                                          NULL));
}

TEST_F (MiddlewareTransportTests, getAllConnections)
{
    int             numTransports = 1;
    mamaConnection* result        = NULL;
    uint32_t        len           = 1;

    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaTransportGetAllConnections(&tport,
                                                             numTransports,
                                                             &result,
                                                             &len));
}

TEST_F (MiddlewareTransportTests, getAllConnectionsInvalidTransportBridge)
{   
    int              numTransports =                   0;
    mamaConnection*  result        = (mamaConnection*) NOT_NULL;
    uint32_t         len           =                   0;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportGetAllConnections(NULL,
                                                             numTransports,
                                                             &result,
                                                             &len));
}

TEST_F (MiddlewareTransportTests, getAllConnectionsInvalidNumTransports)
{   
    transportBridge  transports    = (transportBridge) NOT_NULL;
    mamaConnection*  result        = (mamaConnection*) NOT_NULL;
    uint32_t         len           =                   0;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportGetAllConnections(&transports,
                                                             NULL,
                                                             &result,
                                                             &len));
}

TEST_F (MiddlewareTransportTests, getAllConnectionsInvalidResult)
{   
    transportBridge  transports    = (transportBridge) NOT_NULL;
    int              numTransports =                   0;
    uint32_t         len           =                   0;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportGetAllConnections(&transports,
                                                             numTransports,
                                                             NULL,
                                                             &len));
}

TEST_F (MiddlewareTransportTests, getAllConnectionsInvalidLen)
{   
    transportBridge  transports    = (transportBridge) NOT_NULL;
    int              numTransports =                   0;
    mamaConnection*  result        = (mamaConnection*) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportGetAllConnections(&transports,
                                                             numTransports,
                                                             &result,
                                                             NULL));
}

TEST_F (MiddlewareTransportTests, getAllConnectionsForTopicInvalidTransportBridge)
{
    int              numTransports =                   0;
    const char*      topic         = (char*)           NOT_NULL;
    mamaConnection*  result        = (mamaConnection*) NOT_NULL;
    uint32_t         len           =                   0;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportGetAllConnectionsForTopic(NULL,
                                                                     numTransports,
                                                                     topic,
                                                                     &result,
                                                                     &len));
}

TEST_F (MiddlewareTransportTests, getAllConnectionsForTopicInvalidNumTransports)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    const char*      topic         = (char*)           NOT_NULL;
    mamaConnection*  result        = (mamaConnection*) NOT_NULL;
    uint32_t         len           =                   0;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportGetAllConnectionsForTopic(&transports,
                                                                     NULL,
                                                                     topic,
                                                                     &result,
                                                                     &len));
}



TEST_F (MiddlewareTransportTests, getAllConnectionsForTopic)
{
    int             numTransports = 1;
    mamaConnection* result        = NULL;
    uint32_t        len           = 1;
    const char*     topic         = "topic";

    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaTransportGetAllConnectionsForTopic(&tport,
                                                             numTransports,
                                                             topic,
                                                             &result,
                                                             &len));
}

TEST_F (MiddlewareTransportTests, getAllConnectionsForTopicInvalidTopic)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    int              numTransports =                   0;
    mamaConnection*  result        = (mamaConnection*) NOT_NULL;
    uint32_t         len           =                   0;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportGetAllConnectionsForTopic(&transports,
                                                                     numTransports,
                                                                     NULL,
                                                                     &result,
                                                                     &len));
}

TEST_F (MiddlewareTransportTests, getAllConnectionsForTopicInvalidResult)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    int              numTransports =                   0;
    const char*      topic         = (char*)           NOT_NULL;
    uint32_t         len           =                   0;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportGetAllConnectionsForTopic(&transports,
                                                                     numTransports,
                                                                     topic,
                                                                     NULL,
                                                                     &len));
}

TEST_F (MiddlewareTransportTests, getAllConnectionsForTopicInvalidLen)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    int              numTransports =                   0;
    const char*      topic         = (char*)           NOT_NULL;
    mamaConnection*  result        = (mamaConnection*) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportGetAllConnectionsForTopic(&transports,
                                                                     numTransports,
                                                                     topic,
                                                                     &result,
                                                                     NULL));
}

TEST_F (MiddlewareTransportTests, requestConflation)
{
    int             numTransports = 1;

    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaTransportRequestConflation(&tport,
                                                             numTransports));
}

TEST_F (MiddlewareTransportTests, requestConflationInvalidTransportBridge)
{
    int numTransports = 0;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportRequestConflation(NULL, numTransports));
}

TEST_F (MiddlewareTransportTests, requestConflationInvalidNumTransports)
{
    transportBridge transports = (transportBridge) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportRequestConflation(&transports, NULL));
}


TEST_F (MiddlewareTransportTests, requestEndConflation)
{
    int             numTransports = 1;

    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaTransportRequestEndConflation(&tport, numTransports));
    
    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaTransportRequestConflation(&tport, numTransports));
}

TEST_F (MiddlewareTransportTests, requestEndConflationInvalidTransportBridge)
{
    int numTransports = 0;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportRequestEndConflation(NULL,numTransports));
}

TEST_F (MiddlewareTransportTests, requestEndConflationInvalidNumTransports)
{
    transportBridge transports = (transportBridge) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportRequestEndConflation(&transports,NULL));
}

TEST_F (MiddlewareTransportTests, getAllServerConnectionsInvalidTransportBridge)
{
    int                    numTransports =                         0;
    mamaServerConnection*  result        = (mamaServerConnection*) NOT_NULL;
    uint32_t               len           =                         0;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportGetAllServerConnections(NULL,
                                                                   numTransports,
                                                                   &result,
                                                                   &len));
}

TEST_F (MiddlewareTransportTests, getAllServerConnectionsInvalidNumTransports)
{
    transportBridge        transports    = (transportBridge)       NOT_NULL;
    mamaServerConnection*  result        = (mamaServerConnection*) NOT_NULL;
    uint32_t               len           =                         0;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportGetAllServerConnections(&transports,
                                                                   NULL,
                                                                   &result,
                                                                   &len));
}

TEST_F (MiddlewareTransportTests, getAllServerConnectionsInvalidResult)
{
    transportBridge        transports    = (transportBridge)       NOT_NULL;
    int                    numTransports =                         0;
    uint32_t               len           =                         0;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportGetAllServerConnections(&transports,
                                                                   numTransports,
                                                                   NULL,
                                                                   &len));
}

TEST_F (MiddlewareTransportTests, getAllServerConnectionsInvalidLen)
{
    transportBridge        transports    = (transportBridge)       NOT_NULL;
    int                    numTransports =                         0;
    mamaServerConnection*  result        = (mamaServerConnection*) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportGetAllServerConnections(&transports,
                                                                   numTransports,
                                                                   &result,
                                                                   NULL));
}

TEST_F (MiddlewareTransportTests, freeAllServerConnectionsInvalidTransportBridge)
{
    int                   numTransports =                        0;
    mamaServerConnection  result        = (mamaServerConnection) NOT_NULL;
    uint32_t              len           =                        0;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportFreeAllServerConnections(NULL,
                                                                    numTransports,
                                                                    &result,
                                                                    len));
}

TEST_F (MiddlewareTransportTests, freeAllServerConnectionsInvalidNumTransports)
{
    transportBridge       transports    = (transportBridge)      NOT_NULL;
    mamaServerConnection  result        = (mamaServerConnection) NOT_NULL;
    uint32_t              len           =                        0;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportFreeAllServerConnections(&transports,
                                                                    NULL,
                                                                    &result,
                                                                    len));
}

TEST_F (MiddlewareTransportTests, freeAllServerConnectionsInvalidServerConnection)
{
    transportBridge       transports    = (transportBridge)      NOT_NULL;
    int                   numTransports =                        0;
    uint32_t              len           =                        0;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportFreeAllServerConnections(&transports,
                                                                    numTransports,
                                                                    NULL,
                                                                    len));
}

TEST_F (MiddlewareTransportTests, freeAllServerConnectionsInvalidLen)
{
    transportBridge       transports    = (transportBridge)      NOT_NULL;
    int                   numTransports =                        0;
    mamaServerConnection  result        = (mamaServerConnection) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportFreeAllServerConnections(&transports,
                                                                    numTransports,
                                                                    &result,
                                                                    NULL));
}

TEST_F (MiddlewareTransportTests, freeAllConnectionsInvalidTransportBridge)
{   
    int              numTransports =                   0;
    mamaConnection   result        = (mamaConnection)  NOT_NULL;
    uint32_t         len           =                   0;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportFreeAllConnections(NULL,
                                                              numTransports,
                                                              &result,
                                                              len));
}

TEST_F (MiddlewareTransportTests, freeAllConnectionsInvalidNumTransports)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    mamaConnection   result        = (mamaConnection)  NOT_NULL;
    uint32_t         len           =                   0;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportFreeAllConnections(&transports,
                                                              NULL,
                                                              &result,
                                                              len));
}

TEST_F (MiddlewareTransportTests, freeAllConnectionsInvalidConnection)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    int              numTransports =                   0;
    uint32_t         len           =                   0;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportFreeAllConnections(&transports,
                                                              numTransports,
                                                              NULL,
                                                              len));
}

TEST_F (MiddlewareTransportTests, freeAllConnectionsInvalidLen)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    int              numTransports =                   0;
    mamaConnection   result        = (mamaConnection)  NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportFreeAllConnections(&transports,
                                                              numTransports,
                                                              &result,
                                                              NULL));
}

TEST_F (MiddlewareTransportTests, getNumLoadBalanceAttributes)
{
    const char* name = (char*) NOT_NULL;
    int numLoadBalanceAttributes = 0;

    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaTransportGetNumLoadBalanceAttributes(name, 
                                                        &numLoadBalanceAttributes));
}

TEST_F (MiddlewareTransportTests, getNumLoadBalanceAttributesInvalidName)
{
    int numLoadBalanceAttributes = 0;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportGetNumLoadBalanceAttributes(NULL, 
                                                        &numLoadBalanceAttributes));
}

TEST_F (MiddlewareTransportTests, getNumLoadBalanceAttributesInvalidAttributes)
{
    const char* name = (char*) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportGetNumLoadBalanceAttributes(name, NULL));
}

TEST_F (MiddlewareTransportTests, getLoadBalanceSharedObjectName)
{
    transportBridge tport                     = NULL;
    const char*     name                      = "test_tport";
    const char*     parentName                = "test_tport";
    mamaTransport   parentTport               = NULL;
    int             numLoadBalanceAttributes  = NULL;
    const char*     loadName                  = "";

    ASSERT_EQ(MAMA_STATUS_OK,
              mamaTransport_allocate(&parentTport));

    ASSERT_EQ(MAMA_STATUS_OK,
              mamaTransport_create(parentTport, parentName,
                                   mBridge));
    
    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaTransportCreate(&tport,name,
                                                  parentTport));
    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaTransportGetLoadBalanceSharedObjectName(name,
                                                      &loadName));
 
    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaTransportDestroy(tport));
}

TEST_F (MiddlewareTransportTests, getLoadBalanceSharedObjectNameInvalidName)
{
    const char* loadBalanceSharedObjectName = "name";

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportGetLoadBalanceSharedObjectName(NULL,
                                                      &loadBalanceSharedObjectName));
}

TEST_F (MiddlewareTransportTests, getLoadBalanceSharedObjectNameInvalidObjectName)
{
    const char* name = (char*) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportGetLoadBalanceSharedObjectName(name,NULL));
}

TEST_F (MiddlewareTransportTests, getLoadBalanceScheme)
{
    transportBridge tport       = NULL;
    const char*     name        = "test_tport";
    const char*     parentName  = "test_tport";
    mamaTransport   parentTport = NULL;
    tportLbScheme   scheme      = TPORT_LB_SCHEME_STATIC ;

    ASSERT_EQ(MAMA_STATUS_OK,
              mamaTransport_allocate(&parentTport));

    ASSERT_EQ(MAMA_STATUS_OK,
              mamaTransport_create(parentTport, parentName,
                                   mBridge));
    
    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaTransportCreate(&tport,name,
                                                  parentTport));
    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaTransportGetLoadBalanceScheme(name,
                                                      &scheme));
 
    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaTransportDestroy(tport));
}

TEST_F (MiddlewareTransportTests, getLoadBalanceSchemeInvalidName)
{
    tportLbScheme  scheme = TPORT_LB_SCHEME_STATIC;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportGetLoadBalanceScheme(NULL, &scheme));
}

TEST_F (MiddlewareTransportTests, getLoadBalanceSchemeInvalidScheme)
{
    const char*    name = (char*) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportGetLoadBalanceScheme(name, NULL));
}

TEST_F (MiddlewareTransportTests, sendMsgToConnectionInvalidTransportBridge)
{
    mamaConnection  connection = (mamaConnection)  NOT_NULL;
    mamaMsg         msg        = (mamaMsg)         NOT_NULL;
    const char*     topic      = (char*)           NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportSendMsgToConnection(NULL,
                                                               connection,
                                                               msg,
                                                               topic));
}

TEST_F (MiddlewareTransportTests, sendMsgToConnectionInvalidConnection)
{
    transportBridge tport      = (transportBridge) NOT_NULL;
    mamaMsg         msg        = (mamaMsg)         NOT_NULL;
    const char*     topic      = (char*)           NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportSendMsgToConnection(tport,
                                                               NULL,
                                                               msg,
                                                               topic));
}

TEST_F (MiddlewareTransportTests, sendMsgToConnectionInvalidMsg)
{
    transportBridge tport      = (transportBridge) NOT_NULL;
    mamaConnection  connection = (mamaConnection)  NOT_NULL;
    const char*     topic      = (char*)           NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportSendMsgToConnection(tport,
                                                               connection,
                                                               NULL,
                                                               topic));
}

TEST_F (MiddlewareTransportTests, sendMsgToConnectionInvalidTopic)
{
    transportBridge tport      = (transportBridge) NOT_NULL;
    mamaConnection  connection = (mamaConnection)  NOT_NULL;
    mamaMsg         msg        = (mamaMsg)         NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportSendMsgToConnection(tport,
                                                               connection,
                                                               msg,
                                                               NULL));
}

TEST_F (MiddlewareTransportTests, isConnectionInterceptedInvalidConnection)
{
    uint8_t result = 0;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportIsConnectionIntercepted(NULL,&result));
}

TEST_F (MiddlewareTransportTests, isConnectionInterceptedInvalidResult)
{
    mamaConnection connection = (mamaConnection) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportIsConnectionIntercepted(connection,NULL));
}

TEST_F (MiddlewareTransportTests, installConnectConflateMgrInvalidTransportBridge)
{
    mamaConflationManager mgr        = (mamaConflationManager) NOT_NULL;
    mamaConnection        connection = (mamaConnection)        NOT_NULL;
    conflateProcessCb     onProcess  = (conflateProcessCb)     NOT_NULL;
    conflateGetMsgCb      onMsg      = (conflateGetMsgCb)      NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportInstallConnectConflateMgr(NULL,
                                                                     mgr,
                                                                     connection,
                                                                     onProcess,
                                                                     onMsg));
}

TEST_F (MiddlewareTransportTests, installConnectConflateMgrInvalidManager)
{
    transportBridge       handle     = (transportBridge)       NOT_NULL;
    mamaConnection        connection = (mamaConnection)        NOT_NULL;
    conflateProcessCb     onProcess  = (conflateProcessCb)     NOT_NULL;
    conflateGetMsgCb      onMsg      = (conflateGetMsgCb)      NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportInstallConnectConflateMgr(handle,
                                                                     NULL,
                                                                     connection,
                                                                     onProcess,
                                                                     onMsg));
}

TEST_F (MiddlewareTransportTests, installConnectConflateMgrInvalidConnection)
{
    transportBridge       handle     = (transportBridge)       NOT_NULL;
    mamaConflationManager mgr        = (mamaConflationManager) NOT_NULL;
    conflateProcessCb     onProcess  = (conflateProcessCb)     NOT_NULL;
    conflateGetMsgCb      onMsg      = (conflateGetMsgCb)      NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportInstallConnectConflateMgr(handle,
                                                                     mgr,
                                                                     NULL,
                                                                     onProcess,
                                                                     onMsg));
}

TEST_F (MiddlewareTransportTests, installConnectConflateMgrInvalidProcessCB)
{
    transportBridge       handle     = (transportBridge)       NOT_NULL;
    mamaConflationManager mgr        = (mamaConflationManager) NOT_NULL;
    mamaConnection        connection = (mamaConnection)        NOT_NULL;
    conflateGetMsgCb      onMsg      = (conflateGetMsgCb)      NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportInstallConnectConflateMgr(handle,
                                                                     mgr,
                                                                     connection,
                                                                     NULL,
                                                                     onMsg));
}

TEST_F (MiddlewareTransportTests, installConnectConflateMgrInvalidMsgCB)
{
    transportBridge       handle     = (transportBridge)       NOT_NULL;
    mamaConflationManager mgr        = (mamaConflationManager) NOT_NULL;
    mamaConnection        connection = (mamaConnection)        NOT_NULL;
    conflateProcessCb     onProcess  = (conflateProcessCb)     NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportInstallConnectConflateMgr(handle,
                                                                     mgr,
                                                                     connection,
                                                                     onProcess,
                                                                     NULL));
}

TEST_F (MiddlewareTransportTests, uninstallConnectConflateMgrInvalidTransportBridge)
{
    mamaConflationManager mgr        = (mamaConflationManager) NOT_NULL;
    mamaConnection        connection = (mamaConnection)        NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportUninstallConnectConflateMgr(NULL, mgr, 
                                                                       connection));
}

TEST_F (MiddlewareTransportTests, uninstallConnectConflateMgrInvalidMgr)
{
    transportBridge       handle     = (transportBridge)       NOT_NULL;
    mamaConnection        connection = (mamaConnection)        NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportUninstallConnectConflateMgr(handle, NULL, 
                                                                       connection));
}

TEST_F (MiddlewareTransportTests, uninstallConnectConflateMgrInvalidConnection)
{
    transportBridge       handle     = (transportBridge)       NOT_NULL;
    mamaConflationManager mgr        = (mamaConflationManager) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportUninstallConnectConflateMgr(handle, mgr, 
                                                                       NULL));
}

TEST_F (MiddlewareTransportTests, startConnectionConflationInvalidTransportBridge)
{
    mamaConflationManager mgr        = (mamaConflationManager) NOT_NULL;
    mamaConnection        connection = (mamaConnection)        NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportStartConnectionConflation(NULL,mgr,
                                                                     connection));
}

TEST_F (MiddlewareTransportTests, startConnectionConflationInvalidMgr)
{
    transportBridge       tport      = (transportBridge)       NOT_NULL;
    mamaConnection        connection = (mamaConnection)        NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportStartConnectionConflation(tport,NULL,
                                                                     connection));
}

TEST_F (MiddlewareTransportTests, startConnectionConflationInvalidConnection)
{
    transportBridge       tport      = (transportBridge)       NOT_NULL;
    mamaConflationManager mgr        = (mamaConflationManager) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportStartConnectionConflation(tport,mgr,
                                                                     NULL));
}

TEST_F (MiddlewareTransportTests, getNativeTransport)
{
    transportBridge tport       = NULL;
    const char*     name        = "test_tport";
    const char*     parentName  = "test_tport";
    mamaTransport   parentTport = NULL;
    void*           result      = NULL;

    ASSERT_EQ(MAMA_STATUS_OK,
              mamaTransport_allocate(&parentTport));

    ASSERT_EQ(MAMA_STATUS_OK,
              mamaTransport_create(parentTport, parentName,
                                   mBridge));
    
    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaTransportCreate(&tport,name,
                                                  parentTport));
    
    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaTransportGetNativeTransport(tport,&result));

    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaTransportDestroy(tport));
}

TEST_F (MiddlewareTransportTests, getNativeTransportInvalidTransportBridge)
{
    void* result =  NOT_NULL;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportGetNativeTransport(NULL,&result));
}

TEST_F (MiddlewareTransportTests, getNativeTransportInvalidResult)
{
    transportBridge transport = (transportBridge) NOT_NULL;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportGetNativeTransport(transport,NULL));
}

TEST_F (MiddlewareTransportTests, getNativeTransportNamingCtx)
{
    transportBridge tport       = NULL;
    const char*     name        = "test_tport";
    const char*     parentName  = "test_tport";
    mamaTransport   parentTport = NULL;
    void*           result      = NULL;

    ASSERT_EQ(MAMA_STATUS_OK,
              mamaTransport_allocate(&parentTport));

    ASSERT_EQ(MAMA_STATUS_OK,
              mamaTransport_create(parentTport, parentName,
                                   mBridge));
    
    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaTransportCreate(&tport,name,
                                                  parentTport));
    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaTransportGetNativeTransportNamingCtx(tport,&result));

    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaTransportDestroy(tport));
}

TEST_F (MiddlewareTransportTests, getNativeTransportNamingCtxInvalidTransportBridge)
{
    void* result = NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportGetNativeTransportNamingCtx(NULL,&result));
}

TEST_F (MiddlewareTransportTests, getNativeTransportNamingCtxInvalidResult)
{
    transportBridge transport = (transportBridge) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTransportGetNativeTransportNamingCtx(transport,NULL));
}

