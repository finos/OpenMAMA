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
    mama_status      status        =				   MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportForceClientDisconnect(NULL,
            numTransports,
            ipAddress,
            port);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, forceClientDisconnect)
{
    int             numTransports = 1;
    const char*     ip            = "127.0.0.1";
    uint16_t        port          = 80;
    mama_status     status	      = MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportForceClientDisconnect(&tport,
            numTransports,
            ip,
            port);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_OK, 
               status);
}


TEST_F (MiddlewareTransportTests, forceClientDisconnectInvalidNumTransports)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    const char*      ipAddress     = (char*)           NOT_NULL;
    uint16_t         port          =                   80;
    mama_status      status        =			       MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportForceClientDisconnect(&transports,
                                                                 0,
                                                                 ipAddress,
                                                                 port);
    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               status);
}

TEST_F (MiddlewareTransportTests, forceClientDisconnectInvalidIPAddress)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    int              numTransports =                   0;
    uint16_t         port          =                   80;
    mama_status      status        = 				   MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportForceClientDisconnect(&transports,
                                                                 numTransports,
                                                                 NULL,
                                                                 port);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, forceClientDisconnectInvalidPort)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    int              numTransports =                   0;
    const char*      ipAddress     = (char*)           NOT_NULL;
    mama_status      status        =                   MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportForceClientDisconnect(&transports,
                                                                 numTransports,
                                                                 ipAddress,
                                                                 0);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, findConnection)
{
    int             numTransports = 1;
    const char*     ip            = "127.0.0.1";
    uint16_t        port          = 80;
    mamaConnection  result        = NULL;
    mama_status     status        = MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportFindConnection(&tport,
                                                          numTransports,
                                                          &result,
                                                          ip,
                                                          port);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_OK,
               status);
}

TEST_F (MiddlewareTransportTests, findConnectionInvalidTransportBridge)
{
    int              numTransports =                   0;
    mamaConnection   result        = (mamaConnection)  NOT_NULL;
    const char*      ipAddress     = (char*)           NOT_NULL;
    uint16_t         port          =                   80;
    mama_status      status        =				   MAMA_STATUS_OK;
    
    status = mBridge->bridgeMamaTransportFindConnection(NULL,
                                                          numTransports,
                                                          &result,
                                                          ipAddress,
                                                          port);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               status);
}

TEST_F (MiddlewareTransportTests, findConnectionInvalidNumTransports)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    mamaConnection   result        = (mamaConnection)  NOT_NULL;
    const char*      ipAddress     = (char*)           NOT_NULL;
    uint16_t         port          =                   80;
    mama_status      status        =                   MAMA_STATUS_OK;
    
    status = mBridge->bridgeMamaTransportFindConnection(&transports,
                                                          0,
                                                          &result,
                                                          ipAddress,
                                                          port);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               status);
}

TEST_F (MiddlewareTransportTests, findConnectionInvalidResult)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    int              numTransports =                   0;
    const char*      ipAddress     = (char*)           NOT_NULL;
    uint16_t         port          =                   80;
    mama_status      status        =                   MAMA_STATUS_OK;
    
    status = mBridge->bridgeMamaTransportFindConnection(&transports,
                                                          numTransports,
                                                          NULL,
                                                          ipAddress,
                                                          port);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               status);
}

TEST_F (MiddlewareTransportTests, findConnectionInvalidIPAddress)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    int              numTransports =                   0;
    mamaConnection   result        = (mamaConnection)  NOT_NULL;
    uint16_t         port          =                   80;
    mama_status      status        =                   MAMA_STATUS_OK;
    
    status = mBridge->bridgeMamaTransportFindConnection(&transports,
                                                          numTransports,
                                                          &result,
                                                          NULL,
                                                          port);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               status);
}

TEST_F (MiddlewareTransportTests, findConnectionInvalidPort)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    int              numTransports =                   0;
    mamaConnection   result        = (mamaConnection)  NOT_NULL;
    const char*      ipAddress     = (char*)           NOT_NULL;
    mama_status      status        =                   MAMA_STATUS_OK;
    
    status = mBridge->bridgeMamaTransportFindConnection(&transports,
                                                          numTransports,
                                                          &result,
                                                          ipAddress,
                                                          0);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               status);
}

TEST_F (MiddlewareTransportTests, getAllConnections)
{
    int             numTransports = 1;
    mamaConnection* result        = NULL;
    uint32_t        len           = 1;
    mama_status     status        = MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportGetAllConnections(&tport,
                                                             numTransports,
                                                             &result,
                                                             &len);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_OK,
               status);
}

TEST_F (MiddlewareTransportTests, getAllConnectionsInvalidTransportBridge)
{   
    int              numTransports =                   0;
    mamaConnection*  result        = (mamaConnection*) NOT_NULL;
    uint32_t         len           =                   0;
    mama_status      status        =                   MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportGetAllConnections(NULL,
                                                             numTransports,
                                                             &result,
                                                             &len);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, getAllConnectionsInvalidNumTransports)
{   
    transportBridge  transports    = (transportBridge) NOT_NULL;
    mamaConnection*  result        = (mamaConnection*) NOT_NULL;
    uint32_t         len           =                   0;
    mama_status      status        =                   MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportGetAllConnections(&transports,
                                                             0,
                                                             &result,
                                                             &len);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, getAllConnectionsInvalidResult)
{   
    transportBridge  transports    = (transportBridge) NOT_NULL;
    int              numTransports =                   0;
    uint32_t         len           =                   0;
    mama_status      status        =                   MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportGetAllConnections(&transports,
                                                             numTransports,
                                                             NULL,
                                                             &len);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, getAllConnectionsInvalidLen)
{   
    transportBridge  transports    = (transportBridge) NOT_NULL;
    int              numTransports =                   0;
    mamaConnection*  result        = (mamaConnection*) NOT_NULL;
    mama_status      status        = 				   MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportGetAllConnections(&transports,
                                                             numTransports,
                                                             &result,
                                                             NULL);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, getAllConnectionsForTopicInvalidTransportBridge)
{
    int              numTransports =                   0;
    const char*      topic         = (char*)           NOT_NULL;
    mamaConnection*  result        = (mamaConnection*) NOT_NULL;
    uint32_t         len           =                   0;
    mama_status      status        =                   MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportGetAllConnectionsForTopic(NULL,
                                                                     numTransports,
                                                                     topic,
                                                                     &result,
                                                                     &len);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               status);
}

TEST_F (MiddlewareTransportTests, getAllConnectionsForTopicInvalidNumTransports)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    const char*      topic         = (char*)           NOT_NULL;
    mamaConnection*  result        = (mamaConnection*) NOT_NULL;
    uint32_t         len           =                   0;
    mama_status      status        =                   MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportGetAllConnectionsForTopic(&transports,
                                                                     0,
                                                                     topic,
                                                                     &result,
                                                                     &len);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               status);
}



TEST_F (MiddlewareTransportTests, getAllConnectionsForTopic)
{
    int             numTransports = 1;
    mamaConnection* result        = NULL;
    uint32_t        len           = 1;
    const char*     topic         = "topic";
    mama_status     status        = MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportGetAllConnectionsForTopic(&tport,
                                                             numTransports,
                                                             topic,
                                                             &result,
                                                             &len);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_OK,
               status);
}

TEST_F (MiddlewareTransportTests, getAllConnectionsForTopicInvalidTopic)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    int              numTransports =                   0;
    mamaConnection*  result        = (mamaConnection*) NOT_NULL;
    uint32_t         len           =                   0;
    mama_status      status        =                   MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportGetAllConnectionsForTopic(&transports,
                                                                     numTransports,
                                                                     NULL,
                                                                     &result,
                                                                     &len);
    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               status);
}

TEST_F (MiddlewareTransportTests, getAllConnectionsForTopicInvalidResult)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    int              numTransports =                   0;
    const char*      topic         = (char*)           NOT_NULL;
    uint32_t         len           =                   0;
    mama_status      status        =                   MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportGetAllConnectionsForTopic(&transports,
                                                                     numTransports,
                                                                     topic,
                                                                     NULL,
                                                                     &len);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               status);
}

TEST_F (MiddlewareTransportTests, getAllConnectionsForTopicInvalidLen)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    int              numTransports =                   0;
    const char*      topic         = (char*)           NOT_NULL;
    mamaConnection*  result        = (mamaConnection*) NOT_NULL;
    mama_status      status        =                   MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportGetAllConnectionsForTopic(&transports,
                                                                     numTransports,
                                                                     topic,
                                                                     &result,
                                                                     NULL);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               status);
}

TEST_F (MiddlewareTransportTests, requestConflation)
{
    int             numTransports = 1;
    mama_status     status        = MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportRequestConflation(&tport,
                                                             numTransports);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_OK, 
               status);
}

TEST_F (MiddlewareTransportTests, requestConflationInvalidTransportBridge)
{
    int             numTransports = 1;
    mama_status     status        = MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportRequestConflation(NULL, numTransports);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, requestConflationInvalidNumTransports)
{
    transportBridge transports = (transportBridge) NOT_NULL;
    mama_status     status     =                   MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportRequestConflation(&transports, 0);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}


TEST_F (MiddlewareTransportTests, requestEndConflation)
{
    int             numTransports = 1;
    mama_status     status        = MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportRequestEndConflation(&tport, numTransports);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_OK, 
               status);
}

TEST_F (MiddlewareTransportTests, requestEndConflationInvalidTransportBridge)
{
    int             numTransports = 1;
    mama_status     status        = MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportRequestEndConflation(NULL,numTransports);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, requestEndConflationInvalidNumTransports)
{
    transportBridge transports = (transportBridge) NOT_NULL;
    mama_status     status     =                   MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportRequestEndConflation(&transports,0);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, getAllServerConnectionsInvalidTransportBridge)
{
    int                    numTransports =                         0;
    mamaServerConnection*  result        = (mamaServerConnection*) NOT_NULL;
    uint32_t               len           =                         0;
    mama_status            status        =                         MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportGetAllServerConnections(NULL,
                                                                   numTransports,
                                                                   &result,
                                                                   &len);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, getAllServerConnectionsInvalidNumTransports)
{
    transportBridge        transports    = (transportBridge)       NOT_NULL;
    mamaServerConnection*  result        = (mamaServerConnection*) NOT_NULL;
    uint32_t               len           =                         0;
    mama_status            status        =                         MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportGetAllServerConnections(&transports,
                                                                   0,
                                                                   &result,
                                                                   &len);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, getAllServerConnectionsInvalidResult)
{
    transportBridge        transports    = (transportBridge)       NOT_NULL;
    int                    numTransports =                         0;
    uint32_t               len           =                         0;
    mama_status            status        =                         MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportGetAllServerConnections(&transports,
                                                                   numTransports,
                                                                   NULL,
                                                                   &len);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, getAllServerConnectionsInvalidLen)
{
    transportBridge        transports    = (transportBridge)       NOT_NULL;
    int                    numTransports =                         0;
    mamaServerConnection*  result        = (mamaServerConnection*) NOT_NULL;
    mama_status            status        =                         MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportGetAllServerConnections(&transports,
                                                                   numTransports,
                                                                   &result,
                                                                   NULL);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, freeAllServerConnectionsInvalidTransportBridge)
{
    int                   numTransports =                        0;
    mamaServerConnection  result        = (mamaServerConnection) NOT_NULL;
    uint32_t              len           =                        0;
    mama_status           status        =                        MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportFreeAllServerConnections(NULL,
                                                                    numTransports,
                                                                    &result,
                                                                    len);

	CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, freeAllServerConnectionsInvalidNumTransports)
{
    transportBridge       transports    = (transportBridge)      NOT_NULL;
    mamaServerConnection  result        = (mamaServerConnection) NOT_NULL;
    uint32_t              len           =                        0;
    mama_status           status        =                        MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportFreeAllServerConnections(&transports,
                                                                    0,
                                                                    &result,
                                                                    len);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, freeAllServerConnectionsInvalidServerConnection)
{
    transportBridge       transports    = (transportBridge)      NOT_NULL;
    int                   numTransports =                        0;
    uint32_t              len           =                        0;
    mama_status           status        =                        MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportFreeAllServerConnections(&transports,
                                                                    numTransports,
                                                                    NULL,
                                                                    len);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, freeAllServerConnectionsInvalidLen)
{
    transportBridge       transports    = (transportBridge)      NOT_NULL;
    int                   numTransports =                        0;
    mamaServerConnection  result        = (mamaServerConnection) NOT_NULL;
    mama_status           status        =                        MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportFreeAllServerConnections(&transports,
                                                                    numTransports,
                                                                    &result,
                                                                    0);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, freeAllConnectionsInvalidTransportBridge)
{   
    int              numTransports =                   0;
    mamaConnection   result        = (mamaConnection)  NOT_NULL;
    uint32_t         len           =                   0;
    mama_status      status		   = 				   MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportFreeAllConnections(NULL,
                                                              numTransports,
                                                              &result,
                                                              len);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, freeAllConnectionsInvalidNumTransports)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    mamaConnection   result        = (mamaConnection)  NOT_NULL;
    uint32_t         len           =                   0;
    mama_status      status        =				   MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportFreeAllConnections(&transports,
                                                              0,
                                                              &result,
                                                              len);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, freeAllConnectionsInvalidConnection)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    int              numTransports =                   0;
    uint32_t         len           =                   0;
    mama_status      status        =				   MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportFreeAllConnections(&transports,
                                                              numTransports,
                                                              NULL,
                                                              len);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, freeAllConnectionsInvalidLen)
{
    transportBridge  transports    = (transportBridge) NOT_NULL;
    int              numTransports =                   0;
    mamaConnection   result        = (mamaConnection)  NOT_NULL;
    mama_status      status        =                   MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportFreeAllConnections(&transports,
                                                              numTransports,
                                                              &result,
                                                              0);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, getNumLoadBalanceAttributes)
{
    const char* name = (char*) NOT_NULL;
    int numLoadBalanceAttributes = 0;
    mama_status status = mBridge->bridgeMamaTransportGetNumLoadBalanceAttributes(name,
                                                        &numLoadBalanceAttributes);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_OK, 
               status);
}

TEST_F (MiddlewareTransportTests, getNumLoadBalanceAttributesInvalidName)
{
    int numLoadBalanceAttributes = 0;
    mama_status status = mBridge->bridgeMamaTransportGetNumLoadBalanceAttributes(NULL,
                                                        &numLoadBalanceAttributes);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, getNumLoadBalanceAttributesInvalidAttributes)
{
    const char* name = (char*) NOT_NULL;
    mama_status status = mBridge->bridgeMamaTransportGetNumLoadBalanceAttributes(name, NULL);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, getLoadBalanceSharedObjectName)
{
    const char*     name                      = "test_tport";
    const char*     loadName                  = "";
    mama_status     status                    = MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportGetLoadBalanceSharedObjectName(name,
                                                      &loadName);

	CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_OK, 
               status);
 

}

TEST_F (MiddlewareTransportTests, getLoadBalanceSharedObjectNameInvalidName)
{
    const char* loadBalanceSharedObjectName = "name";
    mama_status status = mBridge->bridgeMamaTransportGetLoadBalanceSharedObjectName(NULL,
                                                      &loadBalanceSharedObjectName);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, getLoadBalanceSharedObjectNameInvalidObjectName)
{
    const char* name = (char*) NOT_NULL;
    mama_status status = mBridge->bridgeMamaTransportGetLoadBalanceSharedObjectName(name,NULL);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, getLoadBalanceScheme)
{
    const char*     name        = "test_tport";
    tportLbScheme   scheme      = TPORT_LB_SCHEME_STATIC ;

    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaTransportGetLoadBalanceScheme(name,
                                                      &scheme));
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
    mama_status     status     =                   MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportSendMsgToConnection(NULL,
                                                               connection,
                                                               msg,
                                                               topic);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, sendMsgToConnectionInvalidConnection)
{
    transportBridge tport      = (transportBridge) NOT_NULL;
    mamaMsg         msg        = (mamaMsg)         NOT_NULL;
    const char*     topic      = (char*)           NOT_NULL;
    mama_status     status     =                   MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportSendMsgToConnection(tport,
                                                               NULL,
                                                               msg,
                                                               topic);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, sendMsgToConnectionInvalidMsg)
{
    transportBridge tport      = (transportBridge) NOT_NULL;
    mamaConnection  connection = (mamaConnection)  NOT_NULL;
    const char*     topic      = (char*)           NOT_NULL;
    mama_status     status     =                   MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportSendMsgToConnection(tport,
                                                               connection,
                                                               NULL,
                                                               topic);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, sendMsgToConnectionInvalidTopic)
{
    transportBridge tport      = (transportBridge) NOT_NULL;
    mamaConnection  connection = (mamaConnection)  NOT_NULL;
    mamaMsg         msg        = (mamaMsg)         NOT_NULL;
    mama_status     status     =                   MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportSendMsgToConnection(tport,
                                                               connection,
                                                               msg,
                                                               NULL);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, isConnectionInterceptedInvalidConnection)
{
    uint8_t result = 0;
    mama_status status = mBridge->bridgeMamaTransportIsConnectionIntercepted(NULL,&result);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
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
    mama_status           status     =                         MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportInstallConnectConflateMgr(NULL,
                                                                     mgr,
                                                                     connection,
                                                                     onProcess,
                                                                     onMsg);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               status);
}

TEST_F (MiddlewareTransportTests, installConnectConflateMgrInvalidManager)
{
    transportBridge       handle     = (transportBridge)       NOT_NULL;
    mamaConnection        connection = (mamaConnection)        NOT_NULL;
    conflateProcessCb     onProcess  = (conflateProcessCb)     NOT_NULL;
    conflateGetMsgCb      onMsg      = (conflateGetMsgCb)      NOT_NULL;
    mama_status           status     =                         MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportInstallConnectConflateMgr(handle,
                                                                     NULL,
                                                                     connection,
                                                                     onProcess,
                                                                     onMsg);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               status);
}

TEST_F (MiddlewareTransportTests, installConnectConflateMgrInvalidConnection)
{
    transportBridge       handle     = (transportBridge)       NOT_NULL;
    mamaConflationManager mgr        = (mamaConflationManager) NOT_NULL;
    conflateProcessCb     onProcess  = (conflateProcessCb)     NOT_NULL;
    conflateGetMsgCb      onMsg      = (conflateGetMsgCb)      NOT_NULL;
    mama_status           status     =                         MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportInstallConnectConflateMgr(handle,
                                                                     mgr,
                                                                     NULL,
                                                                     onProcess,
                                                                     onMsg);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               status);
}

TEST_F (MiddlewareTransportTests, installConnectConflateMgrInvalidProcessCB)
{
    transportBridge       handle     = (transportBridge)       NOT_NULL;
    mamaConflationManager mgr        = (mamaConflationManager) NOT_NULL;
    mamaConnection        connection = (mamaConnection)        NOT_NULL;
    conflateGetMsgCb      onMsg      = (conflateGetMsgCb)      NOT_NULL;
    mama_status           status     =                         MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportInstallConnectConflateMgr(handle,
                                                                     mgr,
                                                                     connection,
                                                                     NULL,
                                                                     onMsg);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               status);
}

TEST_F (MiddlewareTransportTests, installConnectConflateMgrInvalidMsgCB)
{
    transportBridge       handle     = (transportBridge)       NOT_NULL;
    mamaConflationManager mgr        = (mamaConflationManager) NOT_NULL;
    mamaConnection        connection = (mamaConnection)        NOT_NULL;
    conflateProcessCb     onProcess  = (conflateProcessCb)     NOT_NULL;
    mama_status           status     =                         MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportInstallConnectConflateMgr(handle,
                                                                     mgr,
                                                                     connection,
                                                                     onProcess,
                                                                     NULL);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               status);
}

TEST_F (MiddlewareTransportTests, uninstallConnectConflateMgrInvalidTransportBridge)
{
    mamaConflationManager mgr        = (mamaConflationManager) NOT_NULL;
    mamaConnection        connection = (mamaConnection)        NOT_NULL;
    mama_status           status     =                         MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportUninstallConnectConflateMgr(NULL, mgr,
                                                                       connection);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, uninstallConnectConflateMgrInvalidMgr)
{
    transportBridge       handle     = (transportBridge)       NOT_NULL;
    mamaConnection        connection = (mamaConnection)        NOT_NULL;
    mama_status           status     =                         MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportUninstallConnectConflateMgr(handle, NULL,
                                                                       connection);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, uninstallConnectConflateMgrInvalidConnection)
{
    transportBridge       handle     = (transportBridge)       NOT_NULL;
    mamaConflationManager mgr        = (mamaConflationManager) NOT_NULL;
    mama_status           status     =                         MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportUninstallConnectConflateMgr(handle, mgr,
                                                                       NULL);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, startConnectionConflationInvalidTransportBridge)
{
    mamaConflationManager mgr        = (mamaConflationManager) NOT_NULL;
    mamaConnection        connection = (mamaConnection)        NOT_NULL;
    mama_status           status     =                         MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportStartConnectionConflation(NULL,mgr,
                                                                     connection);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, startConnectionConflationInvalidMgr)
{
    transportBridge       tport      = (transportBridge)       NOT_NULL;
    mamaConnection        connection = (mamaConnection)        NOT_NULL;
    mama_status           status     =                         MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportStartConnectionConflation(tport,NULL,
                                                                     connection);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, startConnectionConflationInvalidConnection)
{
    transportBridge       tport      = (transportBridge)       NOT_NULL;
    mamaConflationManager mgr        = (mamaConflationManager) NOT_NULL;
    mama_status           status     =                         MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportStartConnectionConflation(tport,mgr,
                                                                     NULL);
    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareTransportTests, getNativeTransport)
{
    void*           result      = NULL;

    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaTransportGetNativeTransport(tport,&result));
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
    void*           result      = NULL;
    mama_status     status      = MAMA_STATUS_OK;

    status = mBridge->bridgeMamaTransportGetNativeTransportNamingCtx(tport,&result);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_OK, 
               status);


}

TEST_F (MiddlewareTransportTests, getNativeTransportNamingCtxInvalidTransportBridge)
{
    void* result = NOT_NULL;
    mama_status status = mBridge->bridgeMamaTransportGetNativeTransportNamingCtx(NULL,&result);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               status);
}

TEST_F (MiddlewareTransportTests, getNativeTransportNamingCtxInvalidResult)
{
    transportBridge transport = (transportBridge) NOT_NULL;
    mama_status status = mBridge->bridgeMamaTransportGetNativeTransportNamingCtx(transport,NULL);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
              status);
}

