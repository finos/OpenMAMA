/* $Id$
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

/*  Description: These tests will check the basic functionality behind
 *               opening and closing middleware bridges.
 */

#include <gtest/gtest.h>
#include <mama/mama.h>
#include <mama/status.h>

#include <mamainternal.h>
#include <msgimpl.h>

#include "MainUnitTestC.h"

class MamaInternalTestC : public ::testing::Test
{
protected:

    MamaInternalTestC (void);
    virtual ~MamaInternalTestC (void);

    virtual void SetUp    (void);
    virtual void TearDown (void);

    mamaBridge          mBridge;
    mamaPayloadBridge   mPayload;
};

MamaInternalTestC::MamaInternalTestC (void)
{

}

MamaInternalTestC::~MamaInternalTestC (void)
{

}

void MamaInternalTestC::SetUp (void)
{
    mBridge  = NULL;
    mPayload = NULL;
}

void MamaInternalTestC::TearDown (void)
{
    mBridge  = NULL;
    mPayload = NULL;
}

TEST_F (MamaInternalTestC, findBridge)
{
    mamaBridge returnedBridge = NULL;

    ASSERT_EQ (MAMA_STATUS_OK, mama_loadBridge (&mBridge, getMiddleware()));
    ASSERT_EQ (MAMA_STATUS_OK, mama_open ());

    returnedBridge = mamaInternal_findBridge ();
    EXPECT_EQ (mBridge, returnedBridge);

    /* In order to cleanup properly, we call open, the close */
    ASSERT_EQ (MAMA_STATUS_OK, mama_close ());
}

TEST_F (MamaInternalTestC, findBridgeNoneLoaded)
{
    mamaBridge returnedBridge = NULL;

    returnedBridge = mamaInternal_findBridge ();

    EXPECT_EQ (NULL, returnedBridge);
}

TEST_F (MamaInternalTestC, findPayload)
{
    mamaPayloadBridge returnedPayload = NULL;

    ASSERT_EQ (MAMA_STATUS_OK, mama_loadPayloadBridge (&mPayload, getPayload ()));

    /* In order to cleanup correctly, we call loadBridge, open, then close */
    ASSERT_EQ (MAMA_STATUS_OK, mama_loadBridge (&mBridge, getMiddleware ()));
    ASSERT_EQ (MAMA_STATUS_OK, mama_open ());

    returnedPayload = mamaInternal_findPayload (getPayloadId ());

    EXPECT_EQ (mPayload, returnedPayload);

    ASSERT_EQ (MAMA_STATUS_OK, mama_close ());
}

TEST_F (MamaInternalTestC, findPayloadNoneLoaded)
{
    mamaPayloadBridge returnedPayload = NULL;

    returnedPayload = mamaInternal_findPayload (getPayloadId ());

    EXPECT_EQ (NULL, returnedPayload);
}

TEST_F (MamaInternalTestC, getDefaultPayload)
{
    mamaPayloadBridge returnedPayload = NULL;

    ASSERT_EQ (MAMA_STATUS_OK, mama_loadPayloadBridge (&mPayload, getPayload ()));
    ASSERT_EQ (MAMA_STATUS_OK, mama_loadBridge (&mBridge, getMiddleware ()));
    ASSERT_EQ (MAMA_STATUS_OK, mama_open ());

    returnedPayload = mamaInternal_getDefaultPayload ();

    EXPECT_EQ (mPayload, returnedPayload);

    ASSERT_EQ (MAMA_STATUS_OK, mama_close ());
}

TEST_F (MamaInternalTestC, getDefaultPayloadNoneLoaded)
{
    mamaPayloadBridge returnedPayload = NULL;

    returnedPayload = mamaInternal_getDefaultPayload ();

    EXPECT_EQ (NULL, returnedPayload);
}

TEST_F (MamaInternalTestC, mamaInit)
{
    EXPECT_EQ (MAMA_STATUS_OK, mamaInternal_init ());
}

TEST_F (MamaInternalTestC, multipleInit)
{
    EXPECT_EQ (MAMA_STATUS_OK, mamaInternal_init ());
    EXPECT_EQ (MAMA_STATUS_OK, mamaInternal_init ());
    EXPECT_EQ (MAMA_STATUS_OK, mamaInternal_init ());
}

/* The mamaInternal_registerBridge function is now deprecated, and thus should
 * simply not doing anything. Tests are included to ensure backwards
 * compatibility
 */
TEST_F (MamaInternalTestC, registerBridge)
{
    ASSERT_EQ (MAMA_STATUS_OK, mama_loadBridge (&mBridge, getMiddleware ()));
    ASSERT_EQ (MAMA_STATUS_OK, mama_open ());

    mamaInternal_registerBridge (mBridge, getMiddleware ());

    ASSERT_EQ (MAMA_STATUS_OK, mama_close ());
}

TEST_F (MamaInternalTestC, registerBridgeNullBridge)
{
    mamaInternal_registerBridge (NULL, getMiddleware ());
}

TEST_F (MamaInternalTestC, registerBridgeNullMiddleware)
{
    ASSERT_EQ (MAMA_STATUS_OK, mama_loadBridge (&mBridge, getMiddleware ()));
    ASSERT_EQ (MAMA_STATUS_OK, mama_open ());

    mamaInternal_registerBridge (mBridge, NULL);

    ASSERT_EQ (MAMA_STATUS_OK, mama_close ());
}

TEST_F (MamaInternalTestC, wrapperGetVersion)
{
    const char * version = NULL;

    ASSERT_EQ (MAMA_STATUS_OK, mama_loadBridge (&mBridge, getMiddleware ()));
    ASSERT_EQ (MAMA_STATUS_OK, mama_open ());

    version = mama_wrapperGetVersion (mBridge);

    EXPECT_TRUE (NULL != version);

    ASSERT_EQ (MAMA_STATUS_OK, mama_close ());
}

TEST_F (MamaInternalTestC, wrapperGetVersionNullBridge)
{
    const char * version = NULL;

    version = mama_wrapperGetVersion (NULL);

    EXPECT_EQ (NULL, version);
}

TEST_F (MamaInternalTestC, getProperties)
{
    wproperty_t properties = NULL;

    ASSERT_EQ (MAMA_STATUS_OK, mama_loadBridge (&mBridge, getMiddleware ()));
    ASSERT_EQ (MAMA_STATUS_OK, mama_open ());

    properties = mamaInternal_getProperties ();

    EXPECT_TRUE (NULL != properties);

    ASSERT_EQ (MAMA_STATUS_OK, mama_close ());
}

TEST_F (MamaInternalTestC, getPropertiesUnloaded)
{
    wproperty_t properties = NULL;

    properties = mamaInternal_getProperties ();

    EXPECT_EQ (NULL, properties);
}

TEST_F (MamaInternalTestC, setProperty)
{
    const char * name  = "mama.property.test";
    const char * value = "test_value";
    const char * result = NULL;

    EXPECT_EQ (MAMA_STATUS_OK, mama_setProperty (name, value));

    result = mama_getProperty (name);

    EXPECT_STREQ (value, result);
}

TEST_F (MamaInternalTestC, setPropertyNullName)
{
    const char * value = "test_value";

    EXPECT_EQ (MAMA_STATUS_NULL_ARG, mama_setProperty (NULL, value));
}

TEST_F (MamaInternalTestC, setPropertyNullValue)
{
    const char * name = "mama.property.test";

    EXPECT_EQ (MAMA_STATUS_NULL_ARG, mama_setProperty (name, NULL));
}

TEST_F (MamaInternalTestC, getPropertyNullName)
{
    const char * name = "mama.property.test";
    const char * value = "test_value";
    const char * result = NULL;

    ASSERT_EQ (MAMA_STATUS_OK, mama_setProperty (name, value));

    result = mama_getProperty (NULL);

    EXPECT_EQ (NULL, result);
}

TEST_F (MamaInternalTestC, getPayloadChar)
{
    char payloadChar = '\0';
    mamaPayloadBridge returnedPayload = NULL;

    ASSERT_EQ (MAMA_STATUS_OK, mama_loadPayloadBridge (&mPayload, getPayload ()));
    ASSERT_EQ (MAMA_STATUS_OK, mama_loadBridge (&mBridge, getMiddleware ()));
    ASSERT_EQ (MAMA_STATUS_OK, mama_open ());

    EXPECT_EQ (MAMA_STATUS_OK, mamaInternal_getPayloadId (getPayload (),
                                                          mPayload,
                                                          &payloadChar));

    EXPECT_EQ (getPayloadId (), payloadChar);

    returnedPayload = mamaInternal_findPayload (payloadChar);

    EXPECT_EQ (mPayload, returnedPayload);

    ASSERT_EQ (MAMA_STATUS_OK, mama_close ());
}

TEST_F (MamaInternalTestC, getPayloadCharFromMessage)
{
    char payloadChar = '\0';
    mamaMsg msg = NULL;

    ASSERT_EQ (MAMA_STATUS_OK, mama_loadPayloadBridge (&mPayload, getPayload ()));
    ASSERT_EQ (MAMA_STATUS_OK, mama_loadBridge (&mBridge, getMiddleware ()));
    ASSERT_EQ (MAMA_STATUS_OK, mama_open ());
    ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_createForPayloadBridge(&msg, mamaInternal_findPayload(getPayloadId())));

    EXPECT_EQ (MAMA_STATUS_OK, mamaMsgImpl_getPayloadId (msg, &payloadChar));

    EXPECT_EQ (getPayloadId (), payloadChar);

    mamaMsg_destroy(msg);

    ASSERT_EQ (MAMA_STATUS_OK, mama_close ());
}

TEST_F (MamaInternalTestC, getPayloadCharNullPayloadName)
{
    char payloadChar = '\0';

    ASSERT_EQ (MAMA_STATUS_OK, mama_loadPayloadBridge (&mPayload, getPayload ()));
    ASSERT_EQ (MAMA_STATUS_OK, mama_loadBridge (&mBridge, getMiddleware ()));
    ASSERT_EQ (MAMA_STATUS_OK, mama_open ());

    EXPECT_EQ (MAMA_STATUS_NULL_ARG, mamaInternal_getPayloadId (NULL,
                                                                mPayload,
                                                                &payloadChar));

    ASSERT_EQ (MAMA_STATUS_OK, mama_close ());
}

TEST_F (MamaInternalTestC, getPayloadCharNullPayloadBridge)
{
    char payloadChar = '\0';

    EXPECT_EQ (MAMA_STATUS_NULL_ARG, mamaInternal_getPayloadId (getPayload (),
                                                                NULL,
                                                                &payloadChar));
}

TEST_F (MamaInternalTestC, getPayloadCharNullPayloadChar)
{
    ASSERT_EQ (MAMA_STATUS_OK, mama_loadPayloadBridge (&mPayload, getPayload ()));
    ASSERT_EQ (MAMA_STATUS_OK, mama_loadBridge (&mBridge, getMiddleware ()));
    ASSERT_EQ (MAMA_STATUS_OK, mama_open ());

    EXPECT_EQ (MAMA_STATUS_NULL_ARG, mamaInternal_getPayloadId (getPayload (),
                                                                mPayload,
                                                                NULL));

    ASSERT_EQ (MAMA_STATUS_OK, mama_close ());
}

TEST_F (MamaInternalTestC, getPayloadCharFromProperty)
{
    char propertyNameBuffer[256];
    char payloadChar = '\0';

    ASSERT_EQ (MAMA_STATUS_OK, mama_loadPayloadBridge (&mPayload, getPayload ()));
    ASSERT_EQ (MAMA_STATUS_OK, mama_loadBridge (&mBridge, getMiddleware ()));
    ASSERT_EQ (MAMA_STATUS_OK, mama_open ());

    /* retrievePayloadChar expects to find a property in the form:
     * mama.payload.<name>.payloadId
     */
    snprintf (propertyNameBuffer, 256, "mama.payload.%s.payloadId", getPayload ());
    ASSERT_EQ (MAMA_STATUS_OK, mama_setProperty (propertyNameBuffer, "Z"));

    EXPECT_EQ (MAMA_STATUS_OK, mamaInternal_getPayloadId (getPayload (),
                                                          mPayload,
                                                          &payloadChar));

    EXPECT_EQ ('Z', payloadChar);

    ASSERT_EQ (MAMA_STATUS_OK, mama_close ());
}

TEST_F (MamaInternalTestC, getPayloadCharInvalidProperty)
{
    char propertyNameBuffer[256];
    char payloadChar = '\0';

    ASSERT_EQ (MAMA_STATUS_OK, mama_loadPayloadBridge (&mPayload, getPayload ()));
    ASSERT_EQ (MAMA_STATUS_OK, mama_loadBridge (&mBridge, getMiddleware ()));
    ASSERT_EQ (MAMA_STATUS_OK, mama_open ());

    snprintf (propertyNameBuffer, 256, "mama.payload.test.payloadId");
    ASSERT_EQ (MAMA_STATUS_OK, mama_setProperty (propertyNameBuffer, "Z"));

    EXPECT_EQ (MAMA_STATUS_OK, mamaInternal_getPayloadId (getPayload (),
                                                          mPayload,
                                                          &payloadChar));

    EXPECT_NE ('Z', payloadChar);
    EXPECT_EQ (getPayloadId (), payloadChar);

    ASSERT_EQ (MAMA_STATUS_OK, mama_close ());
}

TEST_F (MamaInternalTestC, loadBridge)
{
    EXPECT_EQ (MAMA_STATUS_OK, mama_loadBridge (&mBridge, getMiddleware ()));

    ASSERT_EQ (MAMA_STATUS_OK, mama_open ());
    ASSERT_EQ (MAMA_STATUS_OK, mama_close ());
}

TEST_F (MamaInternalTestC, loadSameBridge)
{
    mamaBridge reloadedBridge = NULL;
    ASSERT_EQ (MAMA_STATUS_OK, mama_loadBridge (&mBridge, getMiddleware ()));
    EXPECT_EQ (MAMA_STATUS_OK, mama_loadBridge (&reloadedBridge, getMiddleware ()));

    EXPECT_EQ (mBridge, reloadedBridge);

    ASSERT_EQ (MAMA_STATUS_OK, mama_close ());
}

TEST_F (MamaInternalTestC, loadBridgeInvalidName)
{
    EXPECT_EQ (MAMA_STATUS_NO_BRIDGE_IMPL, mama_loadBridge (&mBridge, "TEST"));
}

TEST_F (MamaInternalTestC, loadBridgeNullBridge)
{
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, mama_loadBridge (NULL, getMiddleware ()));
}

TEST_F (MamaInternalTestC, loadBridgeNullName)
{
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, mama_loadBridge (&mBridge, NULL));
}

TEST_F (MamaInternalTestC, loadPayloadBridge)
{
    EXPECT_EQ (MAMA_STATUS_OK, mama_loadPayloadBridge (&mPayload, getPayload ()));

    ASSERT_EQ (MAMA_STATUS_OK, mama_loadBridge (&mBridge, getMiddleware ()));
    ASSERT_EQ (MAMA_STATUS_OK, mama_open ());
    ASSERT_EQ (MAMA_STATUS_OK, mama_close ());
}

TEST_F (MamaInternalTestC, loadSamePayloadBridge)
{
    mamaPayloadBridge reloadedPayload;
    ASSERT_EQ (MAMA_STATUS_OK, mama_loadPayloadBridge (&mPayload, getPayload ()));

    EXPECT_EQ (MAMA_STATUS_OK, mama_loadPayloadBridge (&reloadedPayload, getPayload ()));
    EXPECT_EQ (mPayload, reloadedPayload);

    ASSERT_EQ (MAMA_STATUS_OK, mama_loadBridge (&mBridge, getMiddleware ()));
    ASSERT_EQ (MAMA_STATUS_OK, mama_open ());
    ASSERT_EQ (MAMA_STATUS_OK, mama_close ());
}

TEST_F (MamaInternalTestC, loadPayloadBridgeInvalidName)
{
    EXPECT_EQ (MAMA_STATUS_NO_BRIDGE_IMPL,
               mama_loadPayloadBridge (&mPayload, "TEST"));
}

TEST_F (MamaInternalTestC, loadPayloadBridgeNullBridge)
{
    EXPECT_EQ (MAMA_STATUS_NULL_ARG,
               mama_loadPayloadBridge (NULL, getPayload ()));
}

TEST_F (MamaInternalTestC, loadPayloadBridgeNullName)
{
    EXPECT_EQ (MAMA_STATUS_NULL_ARG,
               mama_loadPayloadBridge (&mPayload, NULL));
}

