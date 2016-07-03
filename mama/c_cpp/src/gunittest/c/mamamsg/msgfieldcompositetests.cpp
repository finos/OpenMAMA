/*
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
#include "mama/msg.h"
#include "MainUnitTestC.h"
#include <iostream>

using std::cout;
using std::endl;

class MsgFieldCompositeTestsC : public ::testing::Test
{
protected:
    MsgFieldCompositeTestsC(void) 
        : mMsg           (NULL)
        , mPayloadBridge (NULL)
        , mDict          (NULL)
    {
    	mamaDictionary_create (&mDict);
    }

    virtual ~MsgFieldCompositeTestsC(void)
    {
    	mamaDictionary_destroy (mDict);
    };

    virtual void SetUp(void) 
    {
        mama_loadPayloadBridge (&mPayloadBridge, getPayload());
        mamaMsg_create (&mMsg);
    };

    virtual void TearDown(void) 
    {
        mamaMsg_destroy(mMsg);
    };
    
    mamaMsg           mMsg;
    mamaPayloadBridge mPayloadBridge;
    mamaDictionary    mDict;
};

static void msgOnField (const mamaMsg	   msg,
                        const mamaMsgField field,
                        void*              closure)
{
	mamaFieldDescriptor result = NULL;
    ASSERT_EQ (mamaMsgField_getDescriptor(field, &result), MAMA_STATUS_OK);
}

/*************************************************************
 * General Tests
*************************************************************/

TEST_F (MsgFieldCompositeTestsC, msgFieldGetAsStringValid)
{
    mamaMsgField      mMsgField  = NULL;
    size_t            len        = 5;
    char              buffer[5];

    //Create & add fields to msg
    mamaMsg_addString( mMsg, "name", 1, "test");
    mamaMsg_getField (mMsg, "name", 1, &mMsgField);

    ASSERT_EQ (mamaMsgField_getAsString(mMsgField, buffer, len), MAMA_STATUS_OK);
}

TEST_F (MsgFieldCompositeTestsC, msgFieldGetAsStringInValidMsgField)
{
    mamaMsgField      mMsgField  = NULL;
    char*             buffer     = NULL;
    size_t            len        = 0;

    //Create & add fields to msg
    mamaMsg_addString( mMsg, "name", 1, "test");
    mamaMsg_getField (mMsg, "name", 1, &mMsgField);

    ASSERT_EQ (mamaMsgField_getAsString(NULL, buffer, len), MAMA_STATUS_INVALID_ARG);
}

TEST_F (MsgFieldCompositeTestsC, msgFieldGetAsStringInValidBuffer)
{
    mamaMsgField      mMsgField  = NULL;
    size_t            len        = 0;

    //Create & add fields to msg
    mamaMsg_addString( mMsg, "name", 1, "test");
    mamaMsg_getField (mMsg, "name", 1, &mMsgField);

    ASSERT_EQ (mamaMsgField_getAsString(mMsgField, NULL, len), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgFieldCompositeTestsC, msgFieldGetAsStringInValidLen)
{
    mamaMsgField      mMsgField  = NULL;
    char*             buffer     = NULL;

    //Create & add fields to msg
    mamaMsg_addString( mMsg, "name", 1, "test");
    mamaMsg_getField (mMsg, "name", 1, &mMsgField);

    ASSERT_EQ (mamaMsgField_getAsString(mMsgField, buffer, 0), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgFieldCompositeTestsC, msgFieldGetDescriptorValid)
{
    mamaFieldDescriptor   original   = NULL;
    mama_fid_t            fid        = 101;
    const char*           name       = "fieldName";

    ASSERT_EQ (MAMA_STATUS_OK, mamaDictionary_createFieldDescriptor (
                    mDict,
                    fid,
                    name,
                    MAMA_FIELD_TYPE_STRING,
                    &original));

    //Create & add fields to msg
    mamaMsg_addString( mMsg, name, fid, "test");

    /* iterate through the msg's fields to populate dictionary */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaMsg_iterateFields (mMsg, msgOnField, mDict, NULL));
}

TEST_F (MsgFieldCompositeTestsC, msgFieldGetDescriptorInValidMsgField)
{
    mamaMsgField          mMsgField  = NULL;
    mamaFieldDescriptor*  result     = NULL;

    //Create & add fields to msg
    mamaMsg_addString( mMsg, "name", 1, "test");
    mamaMsg_getField (mMsg, "name", 1, &mMsgField);

    ASSERT_EQ (mamaMsgField_getDescriptor(NULL, result), MAMA_STATUS_INVALID_ARG);
}

TEST_F (MsgFieldCompositeTestsC, msgFieldGetDescriptorInValidResult)
{
    mamaMsgField          mMsgField  = NULL;

    //Create & add fields to msg
    mamaMsg_addString( mMsg, "name", 1, "test");
    mamaMsg_getField (mMsg, "name", 1, &mMsgField);

    ASSERT_EQ (mamaMsgField_getDescriptor(mMsgField, NULL), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgFieldCompositeTestsC, msgFieldGetFidValid)
{
    mamaMsgField      mMsgField  = NULL;
    uint16_t         result     = 0;

    //Create & add fields to msg
    mamaMsg_addString( mMsg, "name", 1, "test");
    mamaMsg_getField (mMsg, "name", 1, &mMsgField);

    ASSERT_EQ (mamaMsgField_getFid(mMsgField, &result), MAMA_STATUS_OK);
}

TEST_F (MsgFieldCompositeTestsC, msgFieldGetFidInValidMsgField)
{
    mamaMsgField      mMsgField  = NULL;
    uint16_t*         result     = 0;

    //Create & add fields to msg
    mamaMsg_addString( mMsg, "name", 1, "test");
    mamaMsg_getField (mMsg, "name", 1, &mMsgField);

    ASSERT_EQ (mamaMsgField_getFid(NULL, result), MAMA_STATUS_INVALID_ARG);
}

TEST_F (MsgFieldCompositeTestsC, msgFieldGetFidInValidResult)
{
    mamaMsgField      mMsgField  = NULL;
    uint16_t*         result     = 0;

    //Create & add fields to msg
    mamaMsg_addString( mMsg, "name", 1, "test");
    mamaMsg_getField (mMsg, "name", 1, &mMsgField);

    ASSERT_EQ (mamaMsgField_getFid(mMsgField, result), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgFieldCompositeTestsC, msgFieldGetNameValid)
{
    mamaMsgField      mMsgField  = NULL;
    const char*       result     = "";

    //Create & add fields to msg
    mamaMsg_addString( mMsg, "name", 1, "test");
    mamaMsg_getField (mMsg, "name", 1, &mMsgField);

    ASSERT_EQ (mamaMsgField_getName(mMsgField, &result), MAMA_STATUS_OK);
}

TEST_F (MsgFieldCompositeTestsC, msgFieldGetNameInValidMsgField)
{
    mamaMsgField      mMsgField  = NULL;
    const char*       result     = "";

    //Create & add fields to msg
    mamaMsg_addString( mMsg, "name", 1, "test");
    mamaMsg_getField (mMsg, "name", 1, &mMsgField);

    ASSERT_EQ (mamaMsgField_getName(NULL, &result), MAMA_STATUS_INVALID_ARG);
}

TEST_F (MsgFieldCompositeTestsC, msgFieldGetNameInValidResult)
{
    mamaMsgField      mMsgField  = NULL;

    //Create & add fields to msg
    mamaMsg_addString( mMsg, "name", 1, "test");
    mamaMsg_getField (mMsg, "name", 1, &mMsgField);

    ASSERT_EQ (mamaMsgField_getName(mMsgField, NULL), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgFieldCompositeTestsC, msgFieldGetTypeValid)
{
    mamaMsgField      mMsgField  = NULL;
    mamaFieldType     result;

    //Create & add fields to msg
    mamaMsg_addString( mMsg, "name", 1, "test");
    mamaMsg_getField (mMsg, "name", 1, &mMsgField);

    ASSERT_EQ (mamaMsgField_getType(mMsgField, &result), MAMA_STATUS_OK);
}

TEST_F (MsgFieldCompositeTestsC, msgFieldGetTypeInValidMsgField)
{
    mamaMsgField      mMsgField  = NULL;
    mamaFieldType*    result     = NULL;

    //Create & add fields to msg
    mamaMsg_addString( mMsg, "name", 1, "test");
    mamaMsg_getField (mMsg, "name", 1, &mMsgField);

    ASSERT_EQ (mamaMsgField_getType(NULL, result), MAMA_STATUS_INVALID_ARG);
}

TEST_F (MsgFieldCompositeTestsC, msgFieldGetTypeInValidResult)
{
    mamaMsgField      mMsgField  = NULL;

    //Create & add fields to msg
    mamaMsg_addString( mMsg, "name", 1, "test");
    mamaMsg_getField (mMsg, "name", 1, &mMsgField);

    ASSERT_EQ (mamaMsgField_getType(mMsgField, NULL), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgFieldCompositeTestsC, msgFieldGetTypeNameValid)
{
    mamaMsgField      mMsgField  = NULL;
    const char*       result     = "";

    //Create & add fields to msg
    mamaMsg_addString( mMsg, "name", 1, "test");
    mamaMsg_getField (mMsg, "name", 1, &mMsgField);

    ASSERT_EQ (mamaMsgField_getTypeName(mMsgField, &result), MAMA_STATUS_OK);
}

TEST_F (MsgFieldCompositeTestsC, msgFieldGetTypeNameInValidMsgField)
{
    mamaMsgField      mMsgField  = NULL;
    const char*       result     = "";

    //Create & add fields to msg
    mamaMsg_addString( mMsg, "name", 1, "test");
    mamaMsg_getField (mMsg, "name", 1, &mMsgField);

    ASSERT_EQ (mamaMsgField_getTypeName(NULL, &result), MAMA_STATUS_INVALID_ARG);
}

TEST_F (MsgFieldCompositeTestsC, DISABLED_msgFieldGetTypeNameInValidResult)
{
    mamaMsgField      mMsgField  = NULL;

    //Create & add fields to msg
    mamaMsg_addString( mMsg, "name", 1, "test");
    mamaMsg_getField (mMsg, "name", 1, &mMsgField);

    ASSERT_EQ (mamaMsgField_getTypeName(mMsgField, NULL), MAMA_STATUS_NULL_ARG);
}

/**********************************************************************
 * String Tests
 **********************************************************************/
class FieldStringTestsC : public MsgFieldCompositeTestsC
{
protected:

    //const char* mIn, mUpdate, mOut;
    char*           mIn;
    char*           mUpdate;
    const char*     mOut;
    mamaMsgField    mField;

    FieldStringTestsC()
         : mOut   (NULL)
         , mField (NULL)
    {
         mIn = strdup("Hello World");
         mUpdate = strdup("Unit Testing Is Awesome");
    }

    ~FieldStringTestsC()
    {
        free (mIn);
        free (mUpdate);
    }

};

// Update
TEST_F (FieldStringTestsC, DISABLED_updateStringValid)
{
    // NOTE: there is no mamaMsgField_updateString
    ASSERT_EQ (mamaMsg_addString(mMsg, "Gary", 10101, mIn), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsg_getField(mMsg, "Gary", 10101, &mField), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsg_updateString(mMsg, "Gary", 10101, mUpdate), MAMA_STATUS_OK);
}

TEST_F (FieldStringTestsC, updateStringInValidMsg)
{
    ASSERT_EQ (mamaMsg_updateString(NULL, "Gary", 10101, mUpdate), MAMA_STATUS_NULL_ARG);
}

TEST_F (FieldStringTestsC, updateStringInValidValue)
{
    ASSERT_EQ (mamaMsg_addString(mMsg, "Gary", 10101, mIn), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsg_updateString(mMsg, "Gary", 10101, NULL), MAMA_STATUS_NULL_ARG);
}

// Get
TEST_F (FieldStringTestsC, getStringValid)
{
    ASSERT_EQ (mamaMsg_addString(mMsg, "Gary", 10101, mIn), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsg_getField(mMsg, "Gary", 10101, &mField), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsgField_getString(mField, &mOut), MAMA_STATUS_OK);
}

TEST_F (FieldStringTestsC, getStringInValidField)
{
    ASSERT_EQ (mamaMsgField_getString(NULL, &mOut), MAMA_STATUS_NULL_ARG);
}

TEST_F (FieldStringTestsC, getStringInValidValue)
{
    ASSERT_EQ (mamaMsg_addString(mMsg, "Gary", 10101, mIn), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsg_getField(mMsg, "Gary", 10101, &mField), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsgField_getString(mField, NULL), MAMA_STATUS_NULL_ARG);
}


/**********************************************************************
 * DateTime Tests
 **********************************************************************/
class FieldDateTimeTestsC : public MsgFieldCompositeTestsC
{
protected:

    mamaDateTime    mIn, mUpdate, mOut;
    mamaMsgField    mField;
    char            mStr[64];

    FieldDateTimeTestsC()
         : mOut   (NULL)
         , mField (NULL)
    {
         mamaDateTime_create(&mIn);
         mamaDateTime_setToNow(mIn);
         mamaDateTime_create(&mUpdate);
         mamaDateTime_setToNow(mUpdate);

         memset (mStr, 0, 64);
         mamaDateTime_getAsString(mIn, mStr, 64);
    }

    ~FieldDateTimeTestsC()
    {
        mamaDateTime_destroy (mIn);
        mamaDateTime_destroy (mUpdate);
    }

};

// Update
TEST_F (FieldDateTimeTestsC, updateDateTimeValid)
{
    ASSERT_EQ (mamaMsg_addDateTime(mMsg, "Gary", 10101, mIn), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsg_getField(mMsg, "Gary", 10101, &mField), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsgField_updateDateTime(mField, mUpdate), MAMA_STATUS_OK);
}

TEST_F (FieldDateTimeTestsC, updateDateTimeInValidField)
{
    ASSERT_EQ (mamaMsgField_updateDateTime(NULL, mUpdate), MAMA_STATUS_NULL_ARG);
}

TEST_F (FieldDateTimeTestsC, updateDateTimeInValidValue)
{
    ASSERT_EQ (mamaMsg_addDateTime(mMsg, "Gary", 10101, mIn), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsg_getField(mMsg, "Gary", 10101, &mField), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsgField_updateDateTime(mField, NULL), MAMA_STATUS_NULL_ARG);
}


// Get
TEST_F (FieldDateTimeTestsC, getDateTimeValid)
{
    mamaDateTime_create(&mOut);
    ASSERT_EQ (mamaMsg_addDateTime(mMsg, "Gary", 10101, mIn), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsg_getField(mMsg, "Gary", 10101, &mField), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsgField_getDateTime(mField, mOut), MAMA_STATUS_OK);
    mamaDateTime_destroy(mOut);
}

TEST_F (FieldDateTimeTestsC, getDateTimeInValidField)
{
    ASSERT_EQ (mamaMsgField_getDateTime(NULL, mOut), MAMA_STATUS_INVALID_ARG);
}

TEST_F (FieldDateTimeTestsC, getDateTimeInValidValue)
{
    ASSERT_EQ (mamaMsg_addDateTime(mMsg, "Gary", 10101, mIn), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsg_getField(mMsg, "Gary", 10101, &mField), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsgField_getDateTime(mField, NULL), MAMA_STATUS_NULL_ARG);
}


/**********************************************************************
 * Opaque Tests
 **********************************************************************/
class FieldOpaqueTestsC : public MsgFieldCompositeTestsC
{
protected:

    const void*     mIn;
    const void*     mUpdate;
    const void*     mOut;
    mama_size_t     mInSize, mOutSize;
    mamaMsgField    mField;

    FieldOpaqueTestsC()
         : mOut   (NULL)
         , mField (NULL)
    {
        mInSize = 6;
        mOutSize = 0;
        mIn = calloc (1, mInSize);
        mUpdate = calloc (1, mInSize);
    }

    ~FieldOpaqueTestsC()
    {
        free ((void*)mIn);
        free ((void*)mUpdate);
    }

};

// Update
TEST_F (FieldOpaqueTestsC, DISABLED_updateOpaqueValid)
{
    // NOTE: there is no mamaMsgField_updateOpaque
    ASSERT_EQ (mamaMsg_addOpaque(mMsg, "Gary", 10101, mIn, mInSize), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsg_getField(mMsg, "Gary", 10101, &mField), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsg_updateOpaque(mMsg, "Gary", 10101, mUpdate, mInSize), MAMA_STATUS_OK);
}

TEST_F (FieldOpaqueTestsC, updateOpaqueInValidField)
{
    ASSERT_EQ (mamaMsg_updateOpaque(NULL, "Gary", 10101, mUpdate, mInSize), MAMA_STATUS_NULL_ARG);
}


// Get
TEST_F (FieldOpaqueTestsC, getOpaqueValid)
{
    ASSERT_EQ (mamaMsg_addOpaque(mMsg, "Gary", 10101, mIn, mInSize), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsg_getField(mMsg, "Gary", 10101, &mField), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsgField_getOpaque(mField, &mOut, &mOutSize), MAMA_STATUS_OK);
}

TEST_F (FieldOpaqueTestsC, getOpaqueInValidField)
{
    ASSERT_EQ (mamaMsgField_getOpaque(NULL, &mOut, &mOutSize), MAMA_STATUS_INVALID_ARG);
}


/**********************************************************************
 * Price Tests
 **********************************************************************/
class FieldPriceTestsC : public MsgFieldCompositeTestsC
{
protected:

    mamaPrice       mIn, mUpdate, mOut;
    mamaMsgField    mField;

    FieldPriceTestsC()
         : mOut    (NULL)
         , mField  (NULL)
    {
        mIn = NULL;
        mamaPrice_create(&mIn);
        mamaPrice_setValue(mIn, 12.09);
        mUpdate = NULL;
        mamaPrice_create(&mUpdate);
        mamaPrice_setValue(mUpdate, 13.45);
    }

    ~FieldPriceTestsC()
    {
        mamaPrice_destroy(mIn);
        mamaPrice_destroy(mUpdate);
    }

};

// Update
TEST_F (FieldPriceTestsC, updatePriceValid)
{
    ASSERT_EQ (mamaMsg_addPrice(mMsg, "Gary", 10101, mIn), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsg_getField(mMsg, "Gary", 10101, &mField), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsgField_updatePrice(mField, mUpdate), MAMA_STATUS_OK);
}

TEST_F (FieldPriceTestsC, updatePriceInValidField)
{
    ASSERT_EQ (mamaMsgField_updatePrice(NULL, mUpdate), MAMA_STATUS_NULL_ARG);
}


// Get
TEST_F (FieldPriceTestsC, getPriceValid)
{
    ASSERT_EQ (mamaMsg_addPrice(mMsg, "Gary", 10101, mIn), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsg_getField(mMsg, "Gary", 10101, &mField), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsgField_getPrice(mField, &mOut), MAMA_STATUS_OK);
}

TEST_F (FieldPriceTestsC, getPriceInValidField)
{
    ASSERT_EQ (mamaMsgField_getPrice(NULL, &mOut), MAMA_STATUS_INVALID_ARG);
}


/**********************************************************************
 * Msg Tests
 **********************************************************************/
class FieldMsgTestsC : public MsgFieldCompositeTestsC
{
protected:

    mamaMsg         mIn, mUpdate, mOut;
    mamaMsgField    mField;

    FieldMsgTestsC()
         : mOut   (NULL)
         , mField (NULL)  
    {
        mamaMsg_create (&mIn);
        mamaMsg_create (&mUpdate);
    }

    ~FieldMsgTestsC()
    {
        mamaMsg_destroy(mIn);
        mamaMsg_destroy(mUpdate);
    }

};

// Update
TEST_F (FieldMsgTestsC, DISABLED_updateSubMsgValid)
{
    // NOTE: there is no mamaMsgField_updateMsg or mamaMsgField_updateSubMsg
    ASSERT_EQ (mamaMsg_addString(mIn, "Gary", 10101, "Unit Testing updateSubMsg"), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsg_addString(mUpdate, "Gary", 10101, "Updating Unit Testing updateSubMsg"), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsg_addMsg(mMsg, "Gary", 10101, mIn), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsg_updateSubMsg(mMsg, "Gary", 10101, mUpdate), MAMA_STATUS_OK);
}

TEST_F (FieldMsgTestsC, updateSubMsgInValidMsg)
{
    ASSERT_EQ (mamaMsg_updateSubMsg(NULL, "Gary", 10101, mUpdate), MAMA_STATUS_NULL_ARG);
}


// Get
TEST_F (FieldMsgTestsC, getMsgValid)
{
    ASSERT_EQ (mamaMsg_addString(mIn, "Gary", 10101, "Unit Testing getMsg"), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsg_addMsg(mMsg, "Gary", 10101, mIn), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsg_getField(mMsg, "Gary", 10101, &mField), MAMA_STATUS_OK);
    ASSERT_EQ (mamaMsgField_getMsg(mField, &mOut), MAMA_STATUS_OK);
}

TEST_F (FieldMsgTestsC, getMsgInValidField)
{
    ASSERT_EQ (mamaMsgField_getMsg(NULL, &mOut), MAMA_STATUS_INVALID_ARG);
}
