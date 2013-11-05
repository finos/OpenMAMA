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
#include "MainUnitTestC.h"
#include <iostream>
#include "payloadbridge.h"

using std::cout;
using std::endl;

class PayloadAtomicTestsC : public ::testing::Test
{
protected:

    mamaPayloadBridge m_payloadBridge;
    msgPayload        m_payload;
    
    virtual ~PayloadAtomicTestsC(){};

    virtual void SetUp(void);
    virtual void TearDown(void);

};

void PayloadAtomicTestsC::SetUp(void)
{
	mama_loadPayloadBridge(&m_payloadBridge, getPayload());
    m_payloadBridge->msgPayloadCreate(&m_payload);
}

void PayloadAtomicTestsC::TearDown()
{
    m_payloadBridge->msgPayloadDestroy(m_payload);
}


/* **********************************************/
/* Tests which add to the payload               */
/* **********************************************/

/* msgPayload_addBool */

TEST_F(PayloadAtomicTestsC, addBoolValid)
{
    mama_bool_t     initialValue = (mama_bool_t)1;
    mama_bool_t     returnedValue = (mama_bool_t)0;
    mama_status     res;


    /* Adding the boolean to payload. */
    res = m_payloadBridge->msgPayloadAddBool(m_payload, NULL, 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddBool(m_payload, NULL, 2, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    /* Recover the boolean from the payload. */
    res = m_payloadBridge->msgPayloadGetBool(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match. */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(returnedValue, initialValue);
}

TEST_F(PayloadAtomicTestsC, addBoolInvalidFID)
{
    mama_bool_t initialValue = (mama_bool_t)1;
    mama_status res;

    /* Adding the boolean to the payload */
    res = m_payloadBridge->msgPayloadAddBool(m_payload, NULL, 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddBool(m_payload, NULL, 0, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F (PayloadAtomicTestsC, addBoolInValidName)
{
    mama_bool_t initialValue = (mama_bool_t)1;
    mama_status res;

    /* Adding the boolean to payload */
    res = m_payloadBridge->msgPayloadAddBool(m_payload, "testValue", 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddBool(m_payload, NULL, 0, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, addBoolNullPayload)
{
    mama_bool_t initialValue = (mama_bool_t)1;
    mama_status res;

    /* Adding the boolean to payload. */
    res = m_payloadBridge->msgPayloadAddBool(NULL, NULL, 1, initialValue);
    EXPECT_EQ(MAMA_STATUS_NULL_ARG, res);
}

TEST_F(PayloadAtomicTestsC, addCharValid)
{
    char        initialValue = 'A';
    char        returnedValue = 'B';
    mama_status res;
    
    /* Adding the char to payload. */
    res = m_payloadBridge->msgPayloadAddChar(m_payload, NULL, 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddChar(m_payload, NULL, 2, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    
    /* Recover the char from the payload. */
    res = m_payloadBridge->msgPayloadGetChar(m_payload, NULL, 1, &returnedValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    /* Test initial and returned values match. */
    EXPECT_EQ(returnedValue, initialValue);
}

TEST_F(PayloadAtomicTestsC, addCharInValidFID)
{
    char        initialValue =  'A';
    mama_status res;
    /* Adding the char to payload. */
    res = m_payloadBridge->msgPayloadAddChar(m_payload, NULL, 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddChar(m_payload, NULL, 0, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, addCharInValidName)
{
    char        initialValue =  'A';
    mama_status res;

    /* Adding the char to the payload */
    res = m_payloadBridge->msgPayloadAddChar(m_payload, "testValue", 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddChar(m_payload, NULL, 0, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, addCharNullPayload)
{
    char initialValue = 'A';
    mama_status res;

    /* Adding the char to payload. */
    res = m_payloadBridge->msgPayloadAddChar(NULL, NULL, 1, initialValue);
    EXPECT_EQ(MAMA_STATUS_NULL_ARG, res);
}

TEST_F(PayloadAtomicTestsC, addI8Valid)
{
    mama_i8_t   initialValue = (mama_i8_t)2;
    mama_i8_t   returnedValue = (mama_i8_t)3;
    mama_status res;

    /* Adding the i8 to the payload */
    res = m_payloadBridge->msgPayloadAddI8(m_payload, NULL, 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddI8(m_payload, NULL, 2, initialValue);
    /* Recover the i8 from the payload */
    res = m_payloadBridge->msgPayloadGetI8(m_payload, NULL, 1, &returnedValue);
   
    /* Test initial and returned values match */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(returnedValue, initialValue);
}

TEST_F(PayloadAtomicTestsC, addI8InValidFID)
{
    mama_i8_t        initialValue = (mama_i8_t)2;
    mama_status res;
    
    /* Adding the I8 to payload. */
    res = m_payloadBridge->msgPayloadAddI8(m_payload, NULL, 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddI8(m_payload, NULL, 0, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, addI8InValidName)
{
    mama_i8_t        initialValue =  (mama_i8_t)2;
    mama_status res;

    /* Adding the I8 to the payload */
    res = m_payloadBridge->msgPayloadAddI8(m_payload, "testValue", 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddI8(m_payload, NULL, 0, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, addI8NullPayload)
{
    mama_i8_t initialValue = (mama_i8_t)2;
    mama_status res;

    /* Adding the I8 to payload. */
    res = m_payloadBridge->msgPayloadAddI8(NULL, NULL, 1, initialValue);
    EXPECT_EQ(MAMA_STATUS_NULL_ARG, res);
}

TEST_F(PayloadAtomicTestsC, addU8Valid)
{
    mama_u8_t   initialValue = (mama_u8_t)2;
    mama_u8_t   returnedValue = (mama_u8_t)3;
    mama_status res;
    
    /* Adding the u8 to the payload */
    res = m_payloadBridge->msgPayloadAddU8(m_payload, NULL, 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddU8(m_payload, NULL, 2, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    /* Recover the u8 from the payload */
    res  = m_payloadBridge->msgPayloadGetU8(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(returnedValue, initialValue);
}

TEST_F(PayloadAtomicTestsC, addU8InValidFID)
{
    mama_u8_t        initialValue = (mama_u8_t)2;
    mama_u8_t        returnedValue = (mama_u8_t)3;
    mama_status res;
    
    /* Adding the U8 to payload. */
    res = m_payloadBridge->msgPayloadAddU8(m_payload, NULL, 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddU8(m_payload, NULL, 0, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, addU8InValidName)
{
    mama_u8_t        initialValue =  (mama_u8_t)2;
    mama_u8_t        returnedValue = (mama_u8_t)3;
    mama_status res;

    /* Adding the U8 to the payload */
    res = m_payloadBridge->msgPayloadAddU8(m_payload, "testValue", 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddU8(m_payload, NULL, 0, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, addU8NullPayload)
{
    mama_u8_t initialValue = (mama_u8_t)2;
    mama_status res;

    /* Adding the U8 to payload. */
    res = m_payloadBridge->msgPayloadAddU8(NULL, NULL, 1, initialValue);
    EXPECT_EQ(MAMA_STATUS_NULL_ARG, res);
}

TEST_F(PayloadAtomicTestsC, addI16Valid)
{
    mama_i16_t  initialValue = (mama_i16_t)2;
    mama_i16_t  returnedValue = (mama_i16_t)3;
    mama_status res;
    
    /* Adding the i16 to the payload */
    res = m_payloadBridge->msgPayloadAddI16(m_payload, NULL, 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddI16(m_payload, NULL, 2, initialValue); 
    /* Recover the i16 from the payload */
    res = m_payloadBridge->msgPayloadGetI16(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(returnedValue, initialValue);
}

TEST_F(PayloadAtomicTestsC, addI16InValidFID)
{
    mama_i16_t        initialValue = (mama_i16_t)2;
    mama_status res;
    
    /* Adding the I16 to payload. */
    res = m_payloadBridge->msgPayloadAddI16(m_payload, NULL, 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddI16(m_payload, NULL, 0, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, addI16InValidName)
{
    mama_i16_t        initialValue =  (mama_i16_t)2;
    mama_status res;

    /* Adding the I16 to the payload */
    res = m_payloadBridge->msgPayloadAddI16(m_payload, "testValue", 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddI16(m_payload, NULL, 0, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, addI16NullPayload)
{
    mama_i16_t initialValue = (mama_i16_t)2;
    mama_status res;

    /* Adding the I16 to payload. */
    res = m_payloadBridge->msgPayloadAddI16(NULL, NULL, 1, initialValue);
    EXPECT_EQ(MAMA_STATUS_NULL_ARG, res);
}

TEST_F(PayloadAtomicTestsC, addU16Valid)
{
    mama_u16_t  initialValue = (mama_u16_t)2;
    mama_u16_t  returnedValue = (mama_u16_t)3;
    mama_status res;
    
    /* Adding the u16 to the payload */
    res = m_payloadBridge->msgPayloadAddU16(m_payload, NULL, 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddU16(m_payload, NULL, 2, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    /* Recover the u16 from the payload */
    res = m_payloadBridge->msgPayloadGetU16(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(returnedValue, initialValue);
}

TEST_F(PayloadAtomicTestsC, addU16InValidFID)
{
    mama_u16_t        initialValue = (mama_u16_t)2;
    mama_status res;
    
    /* Adding the U16 to payload. */
    res = m_payloadBridge->msgPayloadAddU16(m_payload, NULL, 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddU16(m_payload, NULL, 0, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, addU16InValidName)
{
    mama_u16_t        initialValue =  (mama_u16_t)2;
    mama_status res;

    /* Adding the U16 to the payload */
    res = m_payloadBridge->msgPayloadAddU16(m_payload, "testValue", 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddU16(m_payload, NULL, 0, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, addU16NullPayload)
{
    mama_u16_t initialValue = (mama_u16_t)2;
    mama_status res;

    /* Adding the U16 to payload. */
    res = m_payloadBridge->msgPayloadAddU16(NULL, NULL, 1, initialValue);
    EXPECT_EQ(MAMA_STATUS_NULL_ARG, res);
}

TEST_F(PayloadAtomicTestsC, addI32Valid)
{
    mama_i32_t  initialValue = (mama_i32_t)2;
    mama_i32_t  returnedValue = (mama_i32_t)3;
    mama_status res;
    
    /* Adding the i32 to the payload */
    res = m_payloadBridge->msgPayloadAddI32(m_payload, NULL, 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddI32(m_payload, NULL, 2, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    /* Recover the i32 from the payload */
    res = m_payloadBridge->msgPayloadGetI32(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(returnedValue, initialValue);
}

TEST_F(PayloadAtomicTestsC, addI32InValidFID)
{
    mama_i32_t        initialValue = (mama_i32_t)2;
    mama_status res;
    
    /* Adding the I32 to payload. */
    res = m_payloadBridge->msgPayloadAddI32(m_payload, NULL, 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddI32(m_payload, NULL, 0, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, addI32InValidName)
{
    mama_i32_t        initialValue =  (mama_i32_t)2;
    mama_status res;

    /* Adding the I32 to the payload */
    res = m_payloadBridge->msgPayloadAddI32(m_payload, "testValue", 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddI32(m_payload, NULL, 0, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, addI32NullPayload)
{
    mama_i32_t initialValue = (mama_i32_t)2;
    mama_status res;

    /* Adding the I32 to payload. */
    res = m_payloadBridge->msgPayloadAddI32(NULL, NULL, 1, initialValue);
    EXPECT_EQ(MAMA_STATUS_NULL_ARG, res);
}

TEST_F(PayloadAtomicTestsC, addU32Valid)
{
    mama_u32_t  initialValue = (mama_u32_t)2;
    mama_u32_t  returnedValue = (mama_u32_t)3;
    mama_status res;
    
    /* Adding the u32 to the payload */
    res = m_payloadBridge->msgPayloadAddU32(m_payload, NULL, 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddU32(m_payload, NULL, 2, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);

    /* Recover the u32 from the payload */
    res = m_payloadBridge->msgPayloadGetU32(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(returnedValue, initialValue);
}

TEST_F(PayloadAtomicTestsC, addU32InValidFID)
{
    mama_u32_t        initialValue = (mama_u32_t)2;
    mama_status res;
    
    /* Adding the U32 to payload. */
    res = m_payloadBridge->msgPayloadAddU32(m_payload, NULL, 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddU32(m_payload, NULL, 0, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, addU32InValidName)
{
    mama_u32_t        initialValue =  (mama_u32_t)2;
    mama_status res;

    /* Adding the U32 to the payload */
    res = m_payloadBridge->msgPayloadAddU32(m_payload, "testValue", 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddU32(m_payload, NULL, 0, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, addU32NullPayload)
{
    mama_u32_t initialValue = (mama_u32_t)2;
    mama_status res;

    /* Adding the U32 to payload. */
    res = m_payloadBridge->msgPayloadAddU32(NULL, NULL, 1, initialValue);
    EXPECT_EQ(MAMA_STATUS_NULL_ARG, res);
}

TEST_F(PayloadAtomicTestsC, addI64Valid)
{
    mama_i64_t  initialValue = (mama_i64_t)2;
    mama_i64_t  returnedValue = (mama_i64_t)3;
    mama_status res;
    
    /* Adding the i64 to the payload */
    res = m_payloadBridge->msgPayloadAddI64(m_payload, NULL, 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddI64(m_payload, NULL, 2, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
     
    /* Recover the i64 from the payload */
    res = m_payloadBridge->msgPayloadGetI64(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(returnedValue, initialValue);
}

TEST_F(PayloadAtomicTestsC, addI64InValidFID)
{
    mama_i64_t        initialValue = (mama_i64_t)2;
    mama_status res;
    
    /* Adding the I64 to payload. */
    res = m_payloadBridge->msgPayloadAddI64(m_payload, NULL, 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddI64(m_payload, NULL, 0, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, addI64InValidName)
{
    mama_i64_t        initialValue =  (mama_i64_t)2;
    mama_status res;

    /* Adding the I64 to the payload */
    res = m_payloadBridge->msgPayloadAddI64(m_payload, "testValue", 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddI64(m_payload, NULL, 0, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, addI64NullPayload)
{
    mama_i64_t initialValue = (mama_i64_t)2;
    mama_status res;

    /* Adding the I64 to payload. */
    res = m_payloadBridge->msgPayloadAddI64(NULL, NULL, 1, initialValue);
    EXPECT_EQ(MAMA_STATUS_NULL_ARG, res);
}

TEST_F(PayloadAtomicTestsC, addU64Valid)
{
    mama_u64_t  initialValue = (mama_u64_t)2;
    mama_u64_t  returnedValue = (mama_u64_t)3;
    mama_status res;
    
    /* Adding the u64 to the payload */
    res = m_payloadBridge->msgPayloadAddU64(m_payload, NULL, 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddU64(m_payload, NULL, 2, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    /* Recover the u64 from the payload */
    res = m_payloadBridge->msgPayloadGetU64(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(returnedValue, initialValue);
}

TEST_F(PayloadAtomicTestsC, addU64InValidFID)
{
    mama_u64_t        initialValue = (mama_u64_t)2;
    mama_status res;
    
    /* Adding the U64 to payload. */
    res = m_payloadBridge->msgPayloadAddU64(m_payload, NULL, 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddU64(m_payload, NULL, 0, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, addU64InValidName)
{
    mama_u64_t        initialValue =  (mama_u64_t)2;
    mama_status res;

    /* Adding the U64 to the payload */
    res = m_payloadBridge->msgPayloadAddU64(m_payload, "testValue", 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddU64(m_payload, NULL, 0, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, addU64NullPayload)
{
    mama_u64_t initialValue = (mama_u64_t)2;
    mama_status res;

    /* Adding the U64 to payload. */
    res = m_payloadBridge->msgPayloadAddU64(NULL, NULL, 1, initialValue);
    EXPECT_EQ(MAMA_STATUS_NULL_ARG, res);
}

TEST_F(PayloadAtomicTestsC, addF32Valid)
{
    mama_f32_t  initialValue = (mama_f32_t)2;
    mama_f32_t  returnedValue = (mama_f32_t)3;
    mama_status res;

    /* Adding the f32 to the payload */
    res = m_payloadBridge->msgPayloadAddF32(m_payload, NULL, 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddF32(m_payload, NULL, 2, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);

    /* Recover the f32 from the payload */
    res = m_payloadBridge->msgPayloadGetF32(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(returnedValue, initialValue);
}

TEST_F(PayloadAtomicTestsC, addF32InValidFID)
{
    mama_f32_t        initialValue = (mama_f32_t)2;
    mama_status res;
    
    /* Adding the F32 to payload. */
    res = m_payloadBridge->msgPayloadAddF32(m_payload, NULL, 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddF32(m_payload, NULL, 0, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, addF32InValidName)
{
    mama_f32_t        initialValue =  (mama_f32_t)2;
    mama_status res;

    /* Adding the F32 to the payload */
    res = m_payloadBridge->msgPayloadAddF32(m_payload, "testValue", 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddF32(m_payload, NULL, 0, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, addF32NullPayload)
{
    mama_f32_t initialValue = (mama_f32_t)2.0;
    mama_status res;

    /* Adding the F32 to payload. */
    res = m_payloadBridge->msgPayloadAddF32(NULL, NULL, 1, initialValue);
    EXPECT_EQ(MAMA_STATUS_NULL_ARG, res);
}

TEST_F(PayloadAtomicTestsC, addF64Valid)
{
    mama_f64_t  initialValue = (mama_f64_t)2;
    mama_f64_t  returnedValue = (mama_f64_t)3;
    mama_status res;

    /* Adding the f64 to the payload */
    res = m_payloadBridge->msgPayloadAddF64(m_payload, NULL, 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddF64(m_payload, NULL, 2, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    /* Recover the f64 from the payload */
    res = m_payloadBridge->msgPayloadGetF64(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(returnedValue, initialValue);
}

TEST_F(PayloadAtomicTestsC, addF64InValidFID)
{
    mama_f64_t        initialValue = (mama_f64_t)2;
    mama_status res;
    
    /* Adding the F64 to payload. */
    res = m_payloadBridge->msgPayloadAddF64(m_payload, NULL, 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddF64(m_payload, NULL, 0, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, addF64InValidName)
{
    mama_f64_t        initialValue =  (mama_f64_t)2;
    mama_status res;

    /* Adding the F64 to the payload */
    res = m_payloadBridge->msgPayloadAddF64(m_payload, "testValue", 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    res = m_payloadBridge->msgPayloadAddF64(m_payload, NULL, 0, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, addF64NullPayload)
{
    mama_f64_t initialValue = (mama_f64_t)2.0;
    mama_status res;

    /* Adding the F64 to payload. */
    res = m_payloadBridge->msgPayloadAddF64(NULL, NULL, 1, initialValue);
    EXPECT_EQ(MAMA_STATUS_NULL_ARG, res);
}

/************************************************
 * Tests which update the payload               *
 ************************************************/

TEST_F(PayloadAtomicTestsC, updateBoolValid)
{
    mama_bool_t     initialValue = (mama_bool_t)1;
    mama_bool_t     updatedValue = (mama_bool_t)0;
    mama_bool_t     returnedValue = (mama_bool_t)1;
    mama_status     res;

    /* Adding the boolean to payload. */
    res = m_payloadBridge->msgPayloadAddBool(m_payload, NULL, 1, initialValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    /* Check the stored value. */
    res = m_payloadBridge->msgPayloadGetBool(m_payload, NULL, 1, &returnedValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(initialValue, returnedValue);

    /* Update the payload with the value from another bool.*/
    res = m_payloadBridge->msgPayloadUpdateBool(m_payload, NULL, 1, updatedValue);
    EXPECT_EQ(res, MAMA_STATUS_OK);
    /* Recover the boolean from the payload. */
    res = m_payloadBridge->msgPayloadGetBool(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match. */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(updatedValue, returnedValue);
}

TEST_F(PayloadAtomicTestsC, updateBoolNullPayload)
{
    mama_bool_t     updatedValue = (mama_bool_t) 0;
    mama_status     res;
     
    res = m_payloadBridge->msgPayloadUpdateBool(NULL, NULL, 1, updatedValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, updateCharValid)
{
    char     initialValue  = 'B';
    char     updatedValue  = 'A';
    char     returnedValue = 'B';
    mama_status     res;

    /* Adding the char to payload. */
    res = m_payloadBridge->msgPayloadAddChar(m_payload, NULL, 1, initialValue);
    
    /* Update the payload with the value from another char.*/
    res = m_payloadBridge->msgPayloadUpdateChar(m_payload, NULL, 1, updatedValue);
    
    /* Recover the char from the payload. */
    res = m_payloadBridge->msgPayloadGetChar(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match. */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(updatedValue, returnedValue);
}

TEST_F(PayloadAtomicTestsC, updateCharNullPayload)
{
    char     updatedValue = 'A';
    mama_status     res;
     
    res = m_payloadBridge->msgPayloadUpdateChar(NULL, NULL, 1, updatedValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, updateU8Valid)
{
    mama_u8_t     initialValue  = (mama_u8_t)1;
    mama_u8_t     updatedValue  = (mama_u8_t)0;
    mama_u8_t     returnedValue = (mama_u8_t)1;
    mama_status     res;

    /* Adding the u8 to payload. */
    res = m_payloadBridge->msgPayloadAddU8(m_payload, NULL, 1, initialValue);
    
    /* Update the payload with the value from another u8.*/
    res = m_payloadBridge->msgPayloadUpdateU8(m_payload, NULL, 1, updatedValue);
    
    /* Recover the u8 from the payload. */
    res = m_payloadBridge->msgPayloadGetU8(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match. */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(updatedValue, returnedValue);
}

TEST_F(PayloadAtomicTestsC, updateU8NullPayload)
{
    mama_u8_t     updatedValue = (mama_u8_t) 0;
    mama_status     res;
     
    res = m_payloadBridge->msgPayloadUpdateU8(NULL, NULL, 1, updatedValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, updateI8Valid)
{
    mama_i8_t     initialValue  = (mama_i8_t)1;
    mama_i8_t     updatedValue  = (mama_i8_t)0;
    mama_i8_t     returnedValue = (mama_i8_t)1;
    mama_status   res;

    /* Adding the i8 to payload. */
    res = m_payloadBridge->msgPayloadAddI8(m_payload, NULL, 1, initialValue);
    
    /* Update the payload with the value from another i8.*/
    res = m_payloadBridge->msgPayloadUpdateI8(m_payload, NULL, 1, updatedValue);
    
    /* Recover the i8 from the payload. */
    res = m_payloadBridge->msgPayloadGetI8(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match. */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(updatedValue, returnedValue);
}

TEST_F(PayloadAtomicTestsC, updateI8NullPayload)
{
    mama_i8_t     updatedValue = (mama_i8_t) 0;
    mama_status     res;
     
    res = m_payloadBridge->msgPayloadUpdateI8(NULL, NULL, 1, updatedValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, updateI16Valid)
{
    mama_i16_t     initialValue  = (mama_i16_t)1;
    mama_i16_t     updatedValue  = (mama_i16_t)0;
    mama_i16_t     returnedValue = (mama_i16_t)1;
    mama_status    res;

    /* Adding the i16 to payload. */
    res = m_payloadBridge->msgPayloadAddI16(m_payload, NULL, 1, initialValue);
    
    /* Update the payload with the value from another i16.*/
    res = m_payloadBridge->msgPayloadUpdateI16(m_payload, NULL, 1, updatedValue);
    
    /* Recover the i16 from the payload. */
    res = m_payloadBridge->msgPayloadGetI16(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match. */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(updatedValue, returnedValue);
}

TEST_F(PayloadAtomicTestsC, updateI16NullPayload)
{
    mama_i16_t     updatedValue = (mama_i16_t) 0;
    mama_status     res;
     
    res = m_payloadBridge->msgPayloadUpdateI16(NULL, NULL, 1, updatedValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, updateU16Valid)
{
    mama_u16_t     initialValue  = (mama_u16_t)1;
    mama_u16_t     updatedValue  = (mama_u16_t)0;
    mama_u16_t     returnedValue = (mama_u16_t)1;
    mama_status    res;

    /* Adding the u16 to payload. */
    res = m_payloadBridge->msgPayloadAddU16(m_payload, NULL, 1, initialValue);
    
    /* Update the payload with the value from another u16.*/
    res = m_payloadBridge->msgPayloadUpdateU16(m_payload, NULL, 1, updatedValue);
    
    /* Recover the u16 from the payload. */
    res = m_payloadBridge->msgPayloadGetU16(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match. */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(updatedValue, returnedValue);
}

TEST_F(PayloadAtomicTestsC, updateU16NullPayload)
{
    mama_u16_t     updatedValue = (mama_u16_t) 0;
    mama_status     res;
     
    res = m_payloadBridge->msgPayloadUpdateU16(NULL, NULL, 1, updatedValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}


TEST_F(PayloadAtomicTestsC, updateI32Valid)
{
    mama_i32_t     initialValue  = (mama_i32_t)1;
    mama_i32_t     updatedValue  = (mama_i32_t)0;
    mama_i32_t     returnedValue = (mama_i32_t)1;
    mama_status    res;

    /* Adding the i32 to payload. */
    res = m_payloadBridge->msgPayloadAddI32(m_payload, NULL, 1, initialValue);
    
    /* Update the payload with the value from another i32.*/
    res = m_payloadBridge->msgPayloadUpdateI32(m_payload, NULL, 1, updatedValue);
    
    /* Recover the i16 from the payload. */
    res = m_payloadBridge->msgPayloadGetI32(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match. */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(updatedValue, returnedValue);
}

TEST_F(PayloadAtomicTestsC, updateI32NullPayload)
{
    mama_i32_t     updatedValue = (mama_i32_t) 0;
    mama_status     res;
     
    res = m_payloadBridge->msgPayloadUpdateI32(NULL, NULL, 1, updatedValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, updateU32Valid)
{
    mama_u32_t     initialValue  = (mama_u32_t)1;
    mama_u32_t     updatedValue  = (mama_u32_t)0;
    mama_u32_t     returnedValue = (mama_u32_t)1;
    mama_status    res;

    /* Adding the u32 to payload. */
    res = m_payloadBridge->msgPayloadAddU32(m_payload, NULL, 1, initialValue);
    
    /* Update the payload with the value from another u32.*/
    res = m_payloadBridge->msgPayloadUpdateU32(m_payload, NULL, 1, updatedValue);
    
    /* Recover the u32 from the payload. */
    res = m_payloadBridge->msgPayloadGetU32(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match. */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(updatedValue, returnedValue);
}

TEST_F(PayloadAtomicTestsC, updateU32NullPayload)
{
    mama_u32_t     updatedValue = (mama_u32_t) 0;
    mama_status     res;
     
    res = m_payloadBridge->msgPayloadUpdateU32(NULL, NULL, 1, updatedValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, updateF32Valid)
{
    mama_f32_t     initialValue  = (mama_f32_t)1;
    mama_f32_t     updatedValue  = (mama_f32_t)0;
    mama_f32_t     returnedValue = (mama_f32_t)1;
    mama_status    res;

    /* Adding the f32 to payload. */
    res = m_payloadBridge->msgPayloadAddF32(m_payload, NULL, 1, initialValue);
    
    /* Update the payload with the value from another f32.*/
    res = m_payloadBridge->msgPayloadUpdateF32(m_payload, NULL, 1, updatedValue);
    
    /* Recover the f32 from the payload. */
    res = m_payloadBridge->msgPayloadGetF32(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match. */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(updatedValue, returnedValue);
}

TEST_F(PayloadAtomicTestsC, updateF32NullPayload)
{
    mama_f32_t     updatedValue = (mama_f32_t) 0;
    mama_status     res;
     
    res = m_payloadBridge->msgPayloadUpdateF32(NULL, NULL, 1, updatedValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadAtomicTestsC, updateF64Valid)
{
    mama_f64_t     initialValue  = (mama_f64_t)1;
    mama_f64_t     updatedValue  = (mama_f64_t)0;
    mama_f64_t     returnedValue = (mama_f64_t)1;
    mama_status    res;

    /* Adding the f64 to payload. */
    res = m_payloadBridge->msgPayloadAddF64(m_payload, NULL, 1, initialValue);
    
    /* Update the payload with the value from another f64.*/
    res = m_payloadBridge->msgPayloadUpdateF64(m_payload, NULL, 1, updatedValue);
    
    /* Recover the f64 from the payload. */
    res = m_payloadBridge->msgPayloadGetF64(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match. */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(updatedValue, returnedValue);
}

TEST_F(PayloadAtomicTestsC, updateF64NullPayload)
{
    mama_f64_t     updatedValue = (mama_f64_t) 0;
    mama_status     res;
     
    res = m_payloadBridge->msgPayloadUpdateF64(NULL, NULL, 1, updatedValue);
    EXPECT_EQ(res, MAMA_STATUS_NULL_ARG);
}
/************************************************
 * Tests which get the payload                  *
 ************************************************/
   
TEST_F(PayloadAtomicTestsC, getBoolValid)
{
    mama_bool_t     initialValue = (mama_bool_t)1;
    mama_bool_t     returnedValue = (mama_bool_t)0;
    mama_status     res;

    /* Adding the boolean to payload. */
    res = m_payloadBridge->msgPayloadAddBool(m_payload, NULL, 1, initialValue);
    
    /* Recover the boolean from the payload. */
    res = m_payloadBridge->msgPayloadGetBool(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match. */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(returnedValue, initialValue);
}

TEST_F(PayloadAtomicTestsC, getBoolInValid)
{
    mama_status     res;
    mama_bool_t     returnedValue = (mama_bool_t)0;

    res = m_payloadBridge->msgPayloadGetBool(m_payload, NULL, 1, &returnedValue);
    EXPECT_EQ(res, MAMA_STATUS_NOT_FOUND);
}

TEST_F(PayloadAtomicTestsC, getCharValid)
{
    char     initialValue = 'A';
    char     returnedValue = 'B';
    char     res;

    /* Adding the char to payload. */
    res = m_payloadBridge->msgPayloadAddChar(m_payload, NULL, 1, initialValue);
    
    /* Recover the char from the payload. */
    res = m_payloadBridge->msgPayloadGetChar(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match. */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(returnedValue, initialValue);
}

TEST_F(PayloadAtomicTestsC, getCharInValid)
{
    mama_status     res;
    char            returnedValue = 'A';

    res = m_payloadBridge->msgPayloadGetChar(m_payload, NULL, 1, &returnedValue);
    EXPECT_EQ(res, MAMA_STATUS_NOT_FOUND);
}

TEST_F(PayloadAtomicTestsC, getI8Valid)
{
    mama_i8_t     initialValue  = (mama_i8_t)1;
    mama_i8_t     returnedValue = (mama_i8_t)1;
    mama_status   res;

    /* Adding the i8 to payload. */
    res = m_payloadBridge->msgPayloadAddI8(m_payload, NULL, 1, initialValue);
    
    /* Recover the i8 from the payload. */
    res = m_payloadBridge->msgPayloadGetI8(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match. */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(returnedValue, returnedValue);
}

TEST_F(PayloadAtomicTestsC, getI8InValid)
{
    mama_status     res;
    mama_i8_t     returnedValue = (mama_i8_t)0;

    res = m_payloadBridge->msgPayloadGetI8(m_payload, NULL, 1, &returnedValue);
    EXPECT_EQ(res, MAMA_STATUS_NOT_FOUND);
}

TEST_F(PayloadAtomicTestsC, getU8Valid)
{
    mama_u8_t     initialValue  = (mama_u8_t)1;
    mama_u8_t     returnedValue = (mama_u8_t)1;
    mama_status   res;

    /* Adding the u16 to payload. */
    res = m_payloadBridge->msgPayloadAddU8(m_payload, NULL, 1, initialValue);
    
    /* Recover the u16 from the payload. */
    res = m_payloadBridge->msgPayloadGetU8(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match. */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(returnedValue, returnedValue);
}

TEST_F(PayloadAtomicTestsC, getU8InValid)
{
    mama_status     res;
    mama_u8_t      returnedValue = (mama_u8_t)0;

    res = m_payloadBridge->msgPayloadGetU8(m_payload, NULL, 1, &returnedValue);
    EXPECT_EQ(res, MAMA_STATUS_NOT_FOUND);
}   

TEST_F(PayloadAtomicTestsC, getI16Valid)
{
    mama_i16_t     initialValue  = (mama_i16_t)1;
    mama_i16_t     returnedValue = (mama_i16_t)1;
    mama_status   res;

    /* Adding the i16 to payload. */
    res = m_payloadBridge->msgPayloadAddI16(m_payload, NULL, 1, initialValue);
    
    /* Recover the i16 from the payload. */
    res = m_payloadBridge->msgPayloadGetI16(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match. */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(returnedValue, returnedValue);
}   

TEST_F(PayloadAtomicTestsC, getI16InValid)
{
    mama_status     res;
    mama_i16_t      returnedValue = (mama_i16_t)0;

    res = m_payloadBridge->msgPayloadGetI16(m_payload, NULL, 1, &returnedValue);
    EXPECT_EQ(res, MAMA_STATUS_NOT_FOUND);
}

TEST_F(PayloadAtomicTestsC, getU16Valid)
{
    mama_u16_t     initialValue  = (mama_u16_t)1;
    mama_u16_t     returnedValue = (mama_u16_t)1;
    mama_status   res;

    /* Adding the u16 to payload. */
    res = m_payloadBridge->msgPayloadAddU16(m_payload, NULL, 1, initialValue);
    
    /* Recover the u16 from the payload. */
    res = m_payloadBridge->msgPayloadGetU16(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match. */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(returnedValue, returnedValue);
}

TEST_F(PayloadAtomicTestsC, getU16InValid)
{
    mama_status     res;
    mama_u16_t      returnedValue = (mama_u16_t)0;

    res = m_payloadBridge->msgPayloadGetU16(m_payload, NULL, 1, &returnedValue);
    EXPECT_EQ(res, MAMA_STATUS_NOT_FOUND);
}   

TEST_F(PayloadAtomicTestsC, getI32Valid)
{
    mama_i32_t     initialValue  = (mama_i32_t)1;
    mama_i32_t     returnedValue = (mama_i32_t)1;
    mama_status   res;

    /* Adding the i32 to payload. */
    res = m_payloadBridge->msgPayloadAddI32(m_payload, NULL, 1, initialValue);
    
    /* Recover the i32 from the payload. */
    res = m_payloadBridge->msgPayloadGetI32(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match. */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(returnedValue, returnedValue);
}

TEST_F(PayloadAtomicTestsC, getI32InValid)
{
    mama_status     res;
    mama_i32_t     returnedValue = (mama_i32_t)0;

    res = m_payloadBridge->msgPayloadGetI32(m_payload, NULL, 1, &returnedValue);
    EXPECT_EQ(res, MAMA_STATUS_NOT_FOUND);
}

TEST_F(PayloadAtomicTestsC, getU32Valid)
{
    mama_u32_t     initialValue  = (mama_u32_t)1;
    mama_u32_t     returnedValue = (mama_u32_t)1;
    mama_status   res;

    /* Adding the u32 to payload. */
    res = m_payloadBridge->msgPayloadAddU32(m_payload, NULL, 1, initialValue);
    
    /* Recover the u32 from the payload. */
    res = m_payloadBridge->msgPayloadGetU32(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match. */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(returnedValue, returnedValue);
}

TEST_F(PayloadAtomicTestsC, getU32InValid)
{
    mama_status     res;
    mama_u32_t      returnedValue = (mama_u32_t)0;

    res = m_payloadBridge->msgPayloadGetU32(m_payload, NULL, 1, &returnedValue);
    EXPECT_EQ(res, MAMA_STATUS_NOT_FOUND);
}

TEST_F(PayloadAtomicTestsC, getI64Valid)
{
    mama_i64_t     initialValue  = (mama_i64_t)1;
    mama_i64_t     returnedValue = (mama_i64_t)1;
    mama_status   res;

    /* Adding the i64 to payload. */
    res = m_payloadBridge->msgPayloadAddI64(m_payload, NULL, 1, initialValue);
    
    /* Recover the i64 from the payload. */
    res = m_payloadBridge->msgPayloadGetI64(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match. */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(returnedValue, returnedValue);
}

TEST_F(PayloadAtomicTestsC, getI64InValid)
{
    mama_status     res;
    mama_i64_t      returnedValue = (mama_i64_t)0;

    res = m_payloadBridge->msgPayloadGetI64(m_payload, NULL, 1, &returnedValue);
    EXPECT_EQ(res, MAMA_STATUS_NOT_FOUND);
}

TEST_F(PayloadAtomicTestsC, getU64Valid)
{
    mama_u64_t     initialValue  = (mama_u64_t)1;
    mama_u64_t     returnedValue = (mama_u64_t)1;
    mama_status   res;

    /* Adding the u64 to payload. */
    res = m_payloadBridge->msgPayloadAddU64(m_payload, NULL, 1, initialValue);
    
    /* Recover the u64 from the payload. */
    res = m_payloadBridge->msgPayloadGetU64(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match. */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(returnedValue, returnedValue);
}

TEST_F(PayloadAtomicTestsC, getU64InValid)
{
    mama_status     res;
    mama_u64_t      returnedValue = (mama_u64_t)0;

    res = m_payloadBridge->msgPayloadGetU64(m_payload, NULL, 1, &returnedValue);
    EXPECT_EQ(res, MAMA_STATUS_NOT_FOUND);
}

TEST_F(PayloadAtomicTestsC, getF32Valid)
{
    mama_f32_t     initialValue  = (mama_f32_t)1;
    mama_f32_t     returnedValue = (mama_f32_t)1;
    mama_status   res;

    /* Adding the f32 to payload. */
    res = m_payloadBridge->msgPayloadAddF32(m_payload, NULL, 1, initialValue);
    
    /* Recover the f32 from the payload. */
    res = m_payloadBridge->msgPayloadGetF32(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match. */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(returnedValue, returnedValue);
}

TEST_F(PayloadAtomicTestsC, getF32InValid)
{
    mama_status     res;
    mama_f32_t      returnedValue = (mama_f32_t)0;

    res = m_payloadBridge->msgPayloadGetF32(m_payload, NULL, 1, &returnedValue);
    EXPECT_EQ(res, MAMA_STATUS_NOT_FOUND);
}

TEST_F(PayloadAtomicTestsC, getF64Valid)
{
    mama_f64_t     initialValue  = (mama_f64_t)1;
    mama_f64_t     returnedValue = (mama_f64_t)1;
    mama_status   res;

    /* Adding the f64 to payload. */
    res = m_payloadBridge->msgPayloadAddF64(m_payload, NULL, 1, initialValue);
    
    /* Recover the f64 from the payload. */
    res = m_payloadBridge->msgPayloadGetF64(m_payload, NULL, 1, &returnedValue);

    /* Test initial and returned values match. */
    EXPECT_EQ(res, MAMA_STATUS_OK);
    EXPECT_EQ(returnedValue, returnedValue);
}

TEST_F(PayloadAtomicTestsC, getF64InValid)
{
    mama_status     res;
    mama_f64_t      returnedValue = (mama_f64_t)0;

    res = m_payloadBridge->msgPayloadGetF64(m_payload, NULL, 1, &returnedValue);
    EXPECT_EQ(res, MAMA_STATUS_NOT_FOUND);
}

