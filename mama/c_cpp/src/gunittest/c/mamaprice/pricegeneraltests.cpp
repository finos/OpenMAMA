/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/gunittest/c/mamaprice/pricegeneraltests.cpp#1 $
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Technologies, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; eithersStr
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

/*  Description: These tests will check a mamaPrice to ensure that 
 *               setting it's precision will not cause it's value to be
 *               truncated. 
 */

#include "gtest/gtest.h"
#include "MainUnitTestC.h"
#include "mama/price.h"
#include "priceimpl.h"


#define WMPRICE_LARGE_INT64_FORMAT_SPECIFIER "%lld"


// Create a price
// THIS IS A HELPER FUNCTION FOR UNIT TESTING PURPOSES ONLY
// NEVER Create a price this way in an application

mamaPrice CreateTestPrice(const double value, const mama_price_hints_t hints)
{
   mamaPrice price = NULL;
   mama_status status = mamaPrice_create (&price);

   mama_price_t* p1 = (mama_price_t*)price;

   p1->mValue = value;
   p1->mHints = hints;

   return price;
}

// Compare 2 prices for equality
// THIS IS A HELPER FUNCTION FOR UNIT TESTING PURPOSES ONLY
// NEVER compare prices this way in an application

bool ArePricesEqual(const mamaPrice& p1, const mamaPrice& p2)
{
   mama_price_t* t1 = (mama_price_t*) p1;
   mama_price_t* t2 = (mama_price_t*) p2;

   return t1->mValue == t2->mValue &&
          t1->mHints == t2->mHints;
}

class MamaPriceTestC : public ::testing::Test
{
protected:

    mamaPrice m_price;

protected:

    MamaPriceTestC(void);
    virtual ~MamaPriceTestC(void);

    virtual void SetUp(void);
    virtual void TearDown(void);

};


MamaPriceTestC::MamaPriceTestC(void)
{
	m_price = NULL;
}

MamaPriceTestC::~MamaPriceTestC(void)
{
}


void MamaPriceTestC::SetUp(void)
{
	/* Create a new mama price */
    ASSERT_EQ (mamaPrice_create (&m_price), MAMA_STATUS_OK);

    /* Set the value of the price */	
    ASSERT_EQ (mamaPrice_setValue (m_price, 4000000000), MAMA_STATUS_OK);
}

void MamaPriceTestC::TearDown(void)
{
	/* Delete the price */
	if(m_price != NULL)
	{
        ASSERT_EQ (mamaPrice_destroy (m_price), MAMA_STATUS_OK);
	}
}

/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */

/*  Description:     Create a mamaPrice (m_price) of precision int, get the
 *                   value of m_price as a double, format the double as a string
 *                   with an int flag (doubleString). Compare this to the string
 *                   representation of m_price(stringValue).
 *
 *  Expected Result: stringValue = doublestring = 4000000000
 */
TEST_F (MamaPriceTestC, SetPrecisionInt)
{
    double doubleValue      = 0;
    char   doubleString[20] = "";
    char   stringValue[20]  = "";

    /* Set the precision */
    ASSERT_EQ (MAMA_STATUS_OK, 
               mamaPrice_setPrecision (m_price, MAMA_PRICE_PREC_INT));
    
    /* Get the value as a double */
    ASSERT_EQ (MAMA_STATUS_OK, 
               mamaPrice_getValue (m_price, &doubleValue));

    /* Format the double value as a string using an integer flag */
    sprintf (doubleString, WMPRICE_LARGE_INT64_FORMAT_SPECIFIER, (long long int)doubleValue);

    /* Get the value as a string */
    ASSERT_EQ (MAMA_STATUS_OK, 
               mamaPrice_getAsString (m_price, stringValue, 19));

    /* Compare the strings */
    ASSERT_STREQ (stringValue, doubleString);    
}

TEST_F (MamaPriceTestC, SetPrecisionDiv2)
{
    double doubleValue      = 0;
    char   doubleString[20] = "";
    char   stringValue[20]  = "";

    /* Set the precision */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPrice_setPrecision (m_price, MAMA_PRICE_PREC_DIV_2));
    
    /* Get the value as a double */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPrice_getValue (m_price, &doubleValue));

    /* Format the double value as a string using an integer flag */
    sprintf (doubleString, WMPRICE_LARGE_INT64_FORMAT_SPECIFIER, (long long int)doubleValue);

    /* Get the value as a string */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPrice_getAsString (m_price, stringValue, 19));

    /* Compare the strings */
    ASSERT_STREQ (stringValue, doubleString);    
}

/*  Description:     Create a mamaPrice (m_price) of precision div4, get the
 *                   value of m_price as a double, format the double as a string
 *                   with an int flag (doubleString). Compare this to the string
 *                   representation of m_price(stringValue).
 *
 *  Expected Result: stringValue = doublestring = 4000000000
 */
TEST_F (MamaPriceTestC, SetPrecisionDiv4)
{
    double doubleValue      = 0;
    char   doubleString[20] = "";
    char   stringValue[20]  = "";

    /* Set the precision */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPrice_setPrecision (m_price, MAMA_PRICE_PREC_DIV_4));
    
    /* Get the value as a double */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPrice_getValue (m_price, &doubleValue));

    /* Format the double value as a string using an integer flag */
    sprintf (doubleString, WMPRICE_LARGE_INT64_FORMAT_SPECIFIER, (long long int)doubleValue);

    /* Get the value as a string */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPrice_getAsString (m_price, stringValue, 19));

    /* Compare the strings */
    ASSERT_STREQ (stringValue, doubleString);    
}

/*  Description:     Create a mamaPrice (m_price) of precision div8, get the
 *                   value of m_price as a double, format the double as a string
 *                   with an int flag (doubleString). Compare this to the string
 *                   representation of m_price(stringValue).
 *
 *  Expected Result: stringValue = doublestring = 4000000000
 */
TEST_F (MamaPriceTestC, SetPrecisionDiv8)
{
    double doubleValue      = 0;
    char   doubleString[20] = "";
    char   stringValue[20]  = "";

    /* Set the precision */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPrice_setPrecision (m_price, MAMA_PRICE_PREC_DIV_8));
    
    /* Get the value as a double */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPrice_getValue (m_price, &doubleValue));

    /* Format the double value as a string using an integer flag */
    sprintf (doubleString, WMPRICE_LARGE_INT64_FORMAT_SPECIFIER, (long long int)doubleValue);

    /* Get the value as a string */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPrice_getAsString (m_price, stringValue, 19));

    /* Compare the strings */
    ASSERT_STREQ (stringValue, doubleString);    
}

/*  Description:     Create a mamaPrice (m_price) of precision div16, get the
 *                   value of m_price as a double, format the double as a string
 *                   with an int flag (doubleString). Compare this to the string
 *                   representation of m_price(stringValue).
 *
 *  Expected Result: stringValue = doublestring = 4000000000
 */
TEST_F (MamaPriceTestC, SetPrecisionDiv16)
{
    double doubleValue      = 0;
    char   doubleString[20] = "";
    char   stringValue[20]  = "";

    /* Set the precision */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPrice_setPrecision (m_price, MAMA_PRICE_PREC_DIV_16));
    
    /* Get the value as a double */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPrice_getValue (m_price, &doubleValue));

    /* Format the double value as a string using an integer flag */
    sprintf (doubleString, WMPRICE_LARGE_INT64_FORMAT_SPECIFIER, (long long int)doubleValue);

    /* Get the value as a string */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPrice_getAsString (m_price, stringValue, 19));

    /* Compare the strings */
    ASSERT_STREQ (stringValue, doubleString);    
}

/*  Description:     Create a mamaPrice (m_price) of precision div32, get the
 *                   value of m_price as a double, format the double as a string
 *                   with an int flag (doubleString). Compare this to the string
 *                   representation of m_price(stringValue).
 *
 *  Expected Result: stringValue = doublestring = 4000000000
 */
TEST_F (MamaPriceTestC, SetPrecisionDiv32)
{
    double doubleValue      = 0;
    char   doubleString[20] = "";
    char   stringValue[20]  = "";

    /* Set the precision */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPrice_setPrecision (m_price, MAMA_PRICE_PREC_DIV_32));
    
    /* Get the value as a double */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPrice_getValue (m_price, &doubleValue));

    /* Format the double value as a string using an integer flag */
    sprintf (doubleString, WMPRICE_LARGE_INT64_FORMAT_SPECIFIER, (long long int)doubleValue);

    /* Get the value as a string */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPrice_getAsString (m_price, stringValue, 19));

    /* Compare the strings */
    ASSERT_STREQ (stringValue, doubleString);    
}

/*  Description:     Create a mamaPrice (m_price) of precision div64, get the
 *                   value of m_price as a double, format the double as a string
 *                   with an int flag (doubleString). Compare this to the string
 *                   representation of m_price(stringValue).
 *
 *  Expected Result: stringValue = doublestring = 4000000000
 */
TEST_F (MamaPriceTestC, SetPrecisionDiv64)
{
    double doubleValue      = 0;
    char   doubleString[20] = "";
    char   stringValue[20]  = "";

    /* Set the precision */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPrice_setPrecision (m_price, MAMA_PRICE_PREC_DIV_64));
    
    /* Get the value as a double */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPrice_getValue (m_price, &doubleValue));

    /* Format the double value as a string using an integer flag */
    sprintf (doubleString, WMPRICE_LARGE_INT64_FORMAT_SPECIFIER, (long long int)doubleValue);

    /* Get the value as a string */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPrice_getAsString (m_price, stringValue, 19));

    /* Compare the strings */
    ASSERT_STREQ (stringValue, doubleString);    
}

/*  Description:     Create a mamaPrice (m_price) of precision div128, get the
 *                   value of m_price as a double, format the double as a string
 *                   with an int flag (doubleString). Compare this to the string
 *                   representation of m_price(stringValue).
 *
 *  Expected Result: stringValue = doublestring = 4000000000
 */
TEST_F (MamaPriceTestC, SetPrecisionDiv128)
{
    double doubleValue      = 0;
    char   doubleString[20] = "";
    char   stringValue[20]  = "";
    /* Set the precision */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPrice_setPrecision (m_price, MAMA_PRICE_PREC_DIV_128));
    
    /* Get the value as a double */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPrice_getValue (m_price, &doubleValue));

    /* Format the double value as a string using an integer flag */
    sprintf (doubleString, WMPRICE_LARGE_INT64_FORMAT_SPECIFIER, (long long int)doubleValue);

    /* Get the value as a string */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPrice_getAsString (m_price, stringValue, 19));

    /* Compare the strings */
    ASSERT_STREQ (stringValue, doubleString);    
}

/*  Description:     Create a mamaPrice (m_price) of precision div256, get the
 *                   value of m_price as a double, format the double as a string
 *                   with an int flag (doubleString). Compare this to the string
 *                   representation of m_price(stringValue).
 *
 *  Expected Result: stringValue = doublestring = 4000000000
 */
TEST_F (MamaPriceTestC, SetPrecisionDiv256)
{
    double doubleValue      = 0;
    char   doubleString[20] = "";
    char   stringValue[20]  = "";

    /* Set the precision */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPrice_setPrecision (m_price, MAMA_PRICE_PREC_DIV_256));
    
    /* Get the value as a double */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPrice_getValue (m_price, &doubleValue));

    /* Format the double value as a string using an integer flag */
    sprintf (doubleString, WMPRICE_LARGE_INT64_FORMAT_SPECIFIER, (long long int)doubleValue);

    /* Get the value as a string */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPrice_getAsString (m_price, stringValue, 19));

    /* Compare the strings */
    ASSERT_STREQ (stringValue, doubleString);    
}

/*  Description:     Create a mamaPrice (m_price) of precision div512, get the
 *                   value of m_price as a double, format the double as a string
 *                   with an int flag (doubleString). Compare this to the string
 *                   representation of m_price(stringValue).
 *
 *  Expected Result: stringValue = doublestring = 4000000000
 */
TEST_F (MamaPriceTestC, SetPrecisionDiv512)
{
    double doubleValue      = 0;
    char   doubleString[20] = "";
    char   stringValue[20]  = "";

    /* Set the precision */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPrice_setPrecision (m_price, MAMA_PRICE_PREC_DIV_512));
    
    /* Get the value as a double */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPrice_getValue (m_price, &doubleValue));

    /* Format the double value as a string using an integer flag */
    sprintf (doubleString, WMPRICE_LARGE_INT64_FORMAT_SPECIFIER, (long long int)doubleValue);

    /* Get the value as a string */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPrice_getAsString (m_price, stringValue, 19));

    /* Compare the strings */
    ASSERT_STREQ (stringValue, doubleString);    
}

/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */

/*  Description:     Use the price API to create and destroy a simple price object.
 *                   Use the price API to destroy a NULL price Object
 *
 *  Expected Result: create a valid price - MAMA STATUS OK
 *  Expected Result: destroy a valid price - MAMA STATUS OK
 *  Expected Result: destroy a NULL price - MAMA STATUS INVALID ARGUMENT
 */
TEST_F (MamaPriceTestC, testCreateAndDestroyMamaPrice)
{
   mamaPrice price,
             nullPrice = NULL;

   // Test Create and Destroy with a good price object
   ASSERT_EQ ( MAMA_STATUS_OK, mamaPrice_create(&price) );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price) );

   // Test Destroy with NULL price objects
   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_destroy(nullPrice) );
}

/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */

/*  Description:     Create a price
 *                   Clear the price
 *                   Destroy the price.
 *
 *  Expected Result: valid price.- MAMA STATUS OK
 *  Expected Result: NULL price -  MAMA STATUS INVALID ARGUMENT
 */
TEST_F (MamaPriceTestC, testClearMamaPrice)
{
   double x = 100;

   mama_price_hints_t hint = MAMA_PRICE_IMPL_HINT_PREC_10;

   mamaPrice price = CreateTestPrice(x, hint),
             nullPrice = NULL;

   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_clear(nullPrice) );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_clear(price) );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price) );
}

/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */

/*  Description:     Use the price API to copy a mama price object to a second mama price object.
 *                   Both objects must have different values and precision hints before the copy
 *                   Both objects must have identical values and precision hints after the copy
 *                   Test for all combinations of valid and invalid input parameters
 *
 *  Expected Failures : NULL Price to NULL Price - MAMA STATUS INVALID ARGUMENT
 *                    : NULL Price to Valid Price - MAMA STATUS INVALID ARGUMENT
 *                    : Valid Price to NULL Price - MAMA STATUS INVALID ARGUMENT
 *
 *  Expected Passes   : Valid Price to Valid Price - MAMA STATUS OK && value & hints are equal
 *                                            
 */
TEST_F (MamaPriceTestC, testCopyMamaPrice)
{
   double x = 100, 
          y = 200;

   mama_price_hints_t hint1 = MAMA_PRICE_IMPL_HINT_PREC_10, 
                      hint2 = MAMA_PRICE_IMPL_HINT_PREC_2;

   mamaPrice price1     = CreateTestPrice(x, hint1),
             price2     = CreateTestPrice(y, hint2),
             nullPrice  = NULL;

   EXPECT_FALSE ( ArePricesEqual(price1,price2) );
   EXPECT_FALSE ( ArePricesEqual(price2,price1) );

   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_copy(nullPrice,nullPrice) );
   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_copy(nullPrice,price2) );
   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_copy(price1,nullPrice) );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_copy(price2,price1) );
   EXPECT_TRUE ( ArePricesEqual(price1,price2) );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price1) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price2) );
}


/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */

/*  Description:     Use the price API to add a mama price object to a second mama price object
 *                   Both objects must have different values before the addition
 *                   Both objects must have identical precision hints before the addition
 *                   Test for all combinations of valid and invalid input parameters
 *
 *  Expected Failures : NULL Price to NULL Price - MAMA STATUS INVALID ARGUMENT
 *                    : NULL Price to Valid Price - MAMA STATUS INVALID ARGUMENT
 *                    : Valid Price to NULL Price - MAMA STATUS INVALID ARGUMENT
 *
 *  Expected Passes   : Valid Price to Valid Price - MAMA STATUS OK && 
 *                                                   value & hints are equal to the expected result
 *                                            
 */
TEST_F (MamaPriceTestC, testAddMamaPrice)
{
   double x = 100, 
          y = 200, 
          z = x + y;

   mama_price_hints_t hint = MAMA_PRICE_IMPL_HINT_PREC_10; 

   mamaPrice price1        = CreateTestPrice(x,hint),
             price2        = CreateTestPrice(y,hint),
             expectedPrice = CreateTestPrice(z,hint),
             nullPrice     = NULL;

   EXPECT_FALSE ( ArePricesEqual(price1,price2) );
   EXPECT_FALSE ( ArePricesEqual(price1,expectedPrice) );
   EXPECT_FALSE ( ArePricesEqual(price2,expectedPrice) );

   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_add(nullPrice,nullPrice) );
   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_add(price1,nullPrice) );
   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_add(nullPrice,price2) );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_add(price1,price2) );
   EXPECT_TRUE ( ArePricesEqual(price1,expectedPrice) );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price1) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price2) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(expectedPrice) );
}

/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */

/*  Description:     Use the price API to add a subtract a price object from a second mama price object
 *                   Both objects must have different values before the subtraction 
 *                   Both objects must have identical precision hints before the subtraction 
 *                   Test for all combinations of valid and invalid input parameters
 *
 *  Expected Failures : NULL Price to NULL Price - MAMA STATUS INVALID ARGUMENT
 *                    : NULL Price to Valid Price - MAMA STATUS INVALID ARGUMENT
 *                    : Valid Price to NULL Price - MAMA STATUS INVALID ARGUMENT
 *
 *  Expected Passes   : Valid Price to Valid Price - MAMA STATUS OK && 
 *                                                   value & hints are equal to the expected result
 *                                            
 */
TEST_F (MamaPriceTestC, testSubtractMamaPrice)
{
   double x = 100, 
          y = 200, 
          z = x - y;

   mama_price_hints_t hint = MAMA_PRICE_IMPL_HINT_PREC_10; 

   mamaPrice price1        = CreateTestPrice(x,hint),
             price2        = CreateTestPrice(y,hint),
             expectedPrice = CreateTestPrice(z,hint),
             nullPrice     = NULL;

   EXPECT_FALSE ( ArePricesEqual(price1,price2) );
   EXPECT_FALSE ( ArePricesEqual(price1,expectedPrice) );
   EXPECT_FALSE ( ArePricesEqual(price2,expectedPrice) );

   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_subtract(nullPrice,nullPrice) );
   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_subtract(price1,nullPrice) );
   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_subtract(nullPrice,price2) );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_subtract(price1,price2)  );
   EXPECT_TRUE ( ArePricesEqual(price1,expectedPrice) );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price1) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price2) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(expectedPrice) );
}

/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */

/*  Description:     Use the price API to test for equality between 2 different price objects
 *                   Test for all combinations of valid and invalid input parameters
 *                   Note that comparing 2 NULL prices is a pass
 *
 *  Expected Failures : 
 *                    : NULL Price to Valid Price - MAMA STATUS INVALID ARGUMENT
 *                    : Valid Price to NULL Price - MAMA STATUS INVALID ARGUMENT
 *
 *  Expected Passes   : true  - value & hints are identical
 *                    : false - value & hints are not identical  
 *                                            
 */
TEST_F (MamaPriceTestC, testEqualMamaPrice)
{

   double x = 100, 
          y = 100, 
          z = 300;

   double eq = 1, 
          neq = 0;

   mama_price_hints_t hints = MAMA_PRICE_IMPL_HINT_PREC_10; 

   mamaPrice price1    = CreateTestPrice(x,hints),
             price2    = CreateTestPrice(y,hints),
             price3    = CreateTestPrice(z,hints),
             nullPrice = NULL;

   EXPECT_EQ ( neq, mamaPrice_equal(price1,nullPrice) );
   EXPECT_EQ ( neq, mamaPrice_equal(nullPrice,price1) );

   EXPECT_EQ ( eq, mamaPrice_equal(nullPrice,nullPrice) );
   EXPECT_EQ ( eq,  mamaPrice_equal(price1,price1) );
   EXPECT_EQ ( eq,  mamaPrice_equal(price1,price2) );
   EXPECT_EQ ( neq, mamaPrice_equal(price1,price3) );
   EXPECT_EQ ( neq, mamaPrice_equal(price3,price1) );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price1) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price2) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price3) );
}

/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */

/*  Description:     Use the price API to compare 2 different price objects
 *                   Upon successful completion, the fuction will return the difference between 
 *                   the values of both objects.
 *                   The test will include scenarios wehre the difference is positive and negative.
 *                   Both objects must have identical values before the comparison 
 *
 *  Known Issues      : NULL PRice to NULL PRice - Core
 *                    : NULL Price to Valid Price - Core
 *                    : Valid Price to NULL Price - Core
 *
 *  Expected Passes   : return the difference between both
 *                    : false - value & hints are not identical  
 *                                            
 */
TEST_F (MamaPriceTestC, testCompareMamaPrice)
{

   double x = 100, 
          y = 200;

   double r0 = 0, 
          r1 = x - y, 
          r2 = y - x;
   
   mama_price_hints_t hints = MAMA_PRICE_IMPL_HINT_PREC_10; 

   mamaPrice price1    = CreateTestPrice(x,hints),
             price2    = CreateTestPrice(y,hints),
             nullPrice = NULL;

   // TOTO -= Bug in compare method. App crashes when NULL Prices are passed in
   // EXPECT_EQ ( r0, mamaPrice_compare(nullPrice, nullPrice) );
   // EXPECT_EQ ( r1, mamaPrice_compare(nullPrice, price2) );
   // EXPECT_EQ ( r2, mamaPrice_compare(price2, nullPrice) );

   EXPECT_EQ ( r0, mamaPrice_compare(price1, price1) );
   EXPECT_EQ ( r1, mamaPrice_compare(price1, price2) );
   EXPECT_EQ ( r2, mamaPrice_compare(price2, price1) );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price1) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price2) );
}

/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */

/*  Description:     Use the price API to set the value of a price object
 *                   Upon successful completion, the fuction will set the value attribute of the price
 *                   object to the input value.
 *
 *  Expected Failures : NULL Price - MAMA STATUS INVALID ARG
 *
 *  Expected Passes   : With a valid price and value, the function retuns MAMA STATUS OK and updates
 *                    : the value attribute of the price to the new value
 *                                            
 */
TEST_F (MamaPriceTestC, testSetValueMamaPrice)
{

   double x = 100, 
          y = 200;

   mama_price_hints_t hints = MAMA_PRICE_IMPL_HINT_PREC_10; 

   mamaPrice price     = CreateTestPrice(x,hints), 
             nullPrice = NULL;

   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_setValue(nullPrice, x) );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_setValue(price, y) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getValue(price, &x) );
   EXPECT_EQ ( x, y );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price) );
}

/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */

/*  Description:     Use the price API to set the precision of a price object
 *                   Upon successful completion, the fuction will set the hintw attribute of the price
 *                   object to the input value.
 *
 *  Expected Failures : NULL Price - MAMA STATUS INVALID ARG
 *
 *  Expected Passes   : With a valid price and precision, the function retuns MAMA STATUS OK and updates
 *                    : the value attribute of the price to the new value
 *                                            
 */
TEST_F (MamaPriceTestC, testSetPrecisionMamaPrice)
{

   double x = 100;

   mama_price_hints_t hints = MAMA_PRICE_IMPL_HINT_PREC_10; 

   mamaPricePrecision precision = MAMA_PRICE_PREC_DIV_8,
                      expectedPrecision;

   mamaPrice price     = CreateTestPrice(x,hints),
             nullPrice = NULL;

   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_setPrecision(nullPrice, precision) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_setPrecision(price, precision) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getPrecision(price, &expectedPrecision) );
   EXPECT_EQ ( precision, expectedPrecision );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price) );
}

TEST_F (MamaPriceTestC, testSetIsValidPriceMamaPrice)
{

   double x = 100.1;

   mama_price_hints_t hints = MAMA_PRICE_IMPL_HINT_PREC_10; 

   mama_bool_t t = true,
               f = false,
               expected;

   mamaPrice price = CreateTestPrice(x,hints),
             nullPrice = NULL;

   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_setIsValidPrice(nullPrice, t) );
   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_setIsValidPrice(nullPrice, f) );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_setIsValidPrice(price, t) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getIsValidPrice(price, &expected) );
   EXPECT_NE ( f, expected );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_setIsValidPrice(price, f) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getIsValidPrice(price, &expected) );
   EXPECT_EQ ( f, expected );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price) );
}

TEST_F (MamaPriceTestC, testSetHintsMamaPrice)
{

   double x = 100;

   mama_price_hints_t hint1 = MAMA_PRICE_IMPL_HINT_PREC_10, 
                      hint2 = MAMA_PRICE_IMPL_HINT_PREC_2;

   mamaPrice price = CreateTestPrice(x,hint1),
             nullPrice = NULL;;

   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_setHints(nullPrice, hint2) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_setHints(price, hint2) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getHints(price, &hint1) );
   EXPECT_EQ ( hint1, hint2 );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price) );
}

TEST_F (MamaPriceTestC, testSetWithHintsMamaPrice)
{

   double x = 100;

   mama_price_hints_t hint1 = MAMA_PRICE_IMPL_HINT_PREC_10, 
                      hint2 = MAMA_PRICE_IMPL_HINT_PREC_2;

   mamaPrice price     = CreateTestPrice(x,hint1),
             nullPrice = NULL;

   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_setWithHints(nullPrice, x, hint2) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_setWithHints(price, x, hint2) );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price) );
}

TEST_F (MamaPriceTestC, testSetFromStringMamaPrice)
{

   mamaPrice nullPrice = NULL;

   EXPECT_EQ ( MAMA_STATUS_NOT_IMPLEMENTED, mamaPrice_setFromString(nullPrice, "SOME STRING") );
}

TEST_F (MamaPriceTestC, testGetValueMamaPrice)
{

   double x = 100,
          val;

   mama_price_hints_t hint = MAMA_PRICE_IMPL_HINT_PREC_10; 

   mamaPrice price     = CreateTestPrice(x,hint),
             nullPrice = NULL;

   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_getValue(nullPrice, &val) );
   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_getValue(price, NULL) );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getValue(price, &val) );
   EXPECT_EQ ( val, x);

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price) );
}

TEST_F (MamaPriceTestC, testGetRoundedValueMamaPrice)
{

   EXPECT_EQ ( 1, 1 );
}

TEST_F (MamaPriceTestC, testGetPrecisionMamaPrice)
{

   double x = 100;

   mama_price_hints_t hints = MAMA_PRICE_IMPL_HINT_PREC_10; 

   mamaPricePrecision precision1 = MAMA_PRICE_PREC_DIV_8,
                      precision2;

   mamaPrice price     = CreateTestPrice(x,hints),
             nullPrice = NULL;

   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_getPrecision(nullPrice, NULL) );
   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_getPrecision(nullPrice, &precision2) );
   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_getPrecision(price, NULL) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getPrecision(price, &precision2) );
   EXPECT_EQ ( MAMA_PRICE_PREC_10, precision2 );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price) );
}


TEST_F (MamaPriceTestC, testGetIsValidPriceMamaPrice)
{

   double x = 100,
          y = 12.3;

   mama_price_hints_t hints = MAMA_PRICE_IMPL_HINT_PREC_10; 

   mama_bool_t b;

   mamaPrice price     = CreateTestPrice(x,hints),
             nullPrice = NULL;

   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_getIsValidPrice(nullPrice, NULL) );
   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_getIsValidPrice(nullPrice, &b) );
   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_getIsValidPrice(price, NULL) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getIsValidPrice(price, &b) );
   EXPECT_FALSE ( b );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price) );
}

TEST_F (MamaPriceTestC, testGetHintsMamaPrice)
{

   double x = 100;

   mama_price_hints_t hint1 = MAMA_PRICE_IMPL_HINT_PREC_10,
                      hint2; 

   mamaPrice price     = CreateTestPrice(x,hint1),
             nullPrice = NULL;

   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_getHints(nullPrice, NULL) );
   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_getHints(nullPrice, &hint2) );
   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_getHints(price, NULL) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getHints(price, &hint2) );
   EXPECT_EQ ( hint1, hint2 );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price) );
}

TEST_F (MamaPriceTestC, testGetWithHintsMamaPrice)
{

   double x = 100;

   mamaPricePrecision precision = MAMA_PRICE_PREC_DIV_8;

   mama_price_hints_t hint = MAMA_PRICE_IMPL_HINT_PREC_10;

   mamaPrice price     = CreateTestPrice(x,hint),
             nullPrice = NULL;

   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_getWithHints(nullPrice, NULL, NULL) );
   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_getWithHints(nullPrice, NULL, &precision) );
   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_getWithHints(nullPrice, &x, NULL) );
   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_getWithHints(nullPrice, &x, &precision) );
   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_getWithHints(price, NULL, NULL) );
   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_getWithHints(price, NULL, &precision) );
   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_getWithHints(price, &x, NULL) );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getWithHints(price, &x, &precision) );
   EXPECT_EQ ( MAMA_PRICE_PREC_10, precision) ;

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price) );
}

TEST_F (MamaPriceTestC, testGetAsStringMamaPrice)
{

   double x = 1.1234567890;
   mama_size_t size = 20;
   char buff[20];

   mama_price_hints_t hint = MAMA_PRICE_IMPL_HINT_PREC_10000000000;

   mamaPrice price     = CreateTestPrice(x,hint),
             nullPrice = NULL;

   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_getAsString(nullPrice, NULL, size) );
   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_getAsString(nullPrice, buff, size) );
   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_getAsString(price, NULL, size) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getAsString(price, buff, size) );
   EXPECT_STREQ ( "1.1234567890", buff );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price) );
}

TEST_F (MamaPriceTestC, testNegateMamaPrice)
{

   double x = 100;

   mama_price_hints_t hint = MAMA_PRICE_IMPL_HINT_PREC_10;

   mamaPrice price1 = CreateTestPrice(x,hint),
             price2 = CreateTestPrice(x * -1,hint),
             nullPrice = NULL;

   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_negate(nullPrice) );
   EXPECT_FALSE ( ArePricesEqual(price2,price1) );
   EXPECT_FALSE ( ArePricesEqual(price1,price2) );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_negate(price1) );
   EXPECT_TRUE ( ArePricesEqual(price1,price2) );
   EXPECT_TRUE ( ArePricesEqual(price2,price1) );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price1) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price2) );
}

TEST_F (MamaPriceTestC, testIsZeroMamaPrice)
{

   double x = 100,
          y = 0;

   mama_price_hints_t hint = MAMA_PRICE_IMPL_HINT_PREC_10;

   mama_bool_t b;

   mamaPrice price1    = CreateTestPrice(x,hint),
             price2    = CreateTestPrice(y,hint),
             nullPrice = NULL;

   EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaPrice_isZero(nullPrice, &b) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_isZero(price1, &b) );
   EXPECT_FALSE (b);
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_isZero(price2, &b) );
   EXPECT_TRUE (b);

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price1) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price2) );
}

TEST_F (MamaPriceTestC, testDecimals2PrecisionMamaPrice)
{

   EXPECT_EQ ( MAMA_PRICE_PREC_INT, mamaPrice_decimals2Precision(0) );
   EXPECT_EQ ( MAMA_PRICE_PREC_UNKNOWN, mamaPrice_decimals2Precision(-11) );
   EXPECT_EQ ( MAMA_PRICE_PREC_UNKNOWN, mamaPrice_decimals2Precision(11) );
   EXPECT_EQ ( MAMA_PRICE_PREC_10, mamaPrice_decimals2Precision(1) );
   EXPECT_EQ ( MAMA_PRICE_PREC_100, mamaPrice_decimals2Precision(2) );
   EXPECT_EQ ( MAMA_PRICE_PREC_1000, mamaPrice_decimals2Precision(3) );
   EXPECT_EQ ( MAMA_PRICE_PREC_10000, mamaPrice_decimals2Precision(4) );
   EXPECT_EQ ( MAMA_PRICE_PREC_100000, mamaPrice_decimals2Precision(5) );
   EXPECT_EQ ( MAMA_PRICE_PREC_1000000, mamaPrice_decimals2Precision(6) );
   EXPECT_EQ ( MAMA_PRICE_PREC_10000000, mamaPrice_decimals2Precision(7) );
   EXPECT_EQ ( MAMA_PRICE_PREC_100000000, mamaPrice_decimals2Precision(8) );
   EXPECT_EQ ( MAMA_PRICE_PREC_1000000000, mamaPrice_decimals2Precision(9) );
   EXPECT_EQ ( MAMA_PRICE_PREC_10000000000, mamaPrice_decimals2Precision(10) );
}

TEST_F (MamaPriceTestC, testDenom2PrecisionMamaPrice)
{

   EXPECT_EQ ( MAMA_PRICE_PREC_UNKNOWN, mamaPrice_denom2Precision(0) );
   EXPECT_EQ ( MAMA_PRICE_PREC_UNKNOWN, mamaPrice_denom2Precision(-10) );
   EXPECT_EQ ( MAMA_PRICE_PREC_INT, mamaPrice_denom2Precision(1) );
   EXPECT_EQ ( MAMA_PRICE_PREC_DIV_2, mamaPrice_denom2Precision(2) );
   EXPECT_EQ ( MAMA_PRICE_PREC_DIV_4, mamaPrice_denom2Precision(4) );
   EXPECT_EQ ( MAMA_PRICE_PREC_DIV_8, mamaPrice_denom2Precision(8) );
   EXPECT_EQ ( MAMA_PRICE_PREC_DIV_16, mamaPrice_denom2Precision(16) );
   EXPECT_EQ ( MAMA_PRICE_PREC_DIV_32, mamaPrice_denom2Precision(32) );
   EXPECT_EQ ( MAMA_PRICE_PREC_DIV_64, mamaPrice_denom2Precision(64) );
   EXPECT_EQ ( MAMA_PRICE_PREC_DIV_128, mamaPrice_denom2Precision(128) );
   EXPECT_EQ ( MAMA_PRICE_PREC_DIV_256, mamaPrice_denom2Precision(256) );
   EXPECT_EQ ( MAMA_PRICE_PREC_DIV_512, mamaPrice_denom2Precision(512) );
}


TEST_F (MamaPriceTestC, testPrecision2DecimalsMamaPrice)
{

   EXPECT_EQ ( mama_i32_t(0), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_UNKNOWN) );
   EXPECT_EQ ( mama_i32_t(1), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_10) );
   EXPECT_EQ ( mama_i32_t(2), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_100) );
   EXPECT_EQ ( mama_i32_t(3), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_1000) );
   EXPECT_EQ ( mama_i32_t(4), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_10000) );
   EXPECT_EQ ( mama_i32_t(5), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_100000) );
   EXPECT_EQ ( mama_i32_t(6), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_1000000) );
   EXPECT_EQ ( mama_i32_t(7), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_10000000) );
   EXPECT_EQ ( mama_i32_t(8), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_100000000) );
   EXPECT_EQ ( mama_i32_t(9), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_1000000000) );
   EXPECT_EQ ( mama_i32_t(10), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_10000000000) );
   EXPECT_EQ ( mama_i32_t(10), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_INT) );
   EXPECT_EQ ( mama_i32_t(10), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_DIV_2) );
   EXPECT_EQ ( mama_i32_t(10), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_DIV_4) );
   EXPECT_EQ ( mama_i32_t(10), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_DIV_8) );
   EXPECT_EQ ( mama_i32_t(10), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_DIV_16) );
   EXPECT_EQ ( mama_i32_t(10), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_DIV_32) );
   EXPECT_EQ ( mama_i32_t(10), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_DIV_64) );
   EXPECT_EQ ( mama_i32_t(10), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_DIV_128) );
   EXPECT_EQ ( mama_i32_t(10), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_DIV_256) );
   EXPECT_EQ ( mama_i32_t(10), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_DIV_512) );
   EXPECT_EQ ( mama_i32_t(10), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_TICK_32) );
   EXPECT_EQ ( mama_i32_t(10), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_HALF_32) );
   EXPECT_EQ ( mama_i32_t(10), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_QUARTER_32) );
   EXPECT_EQ ( mama_i32_t(10), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_TICK_64) );
   EXPECT_EQ ( mama_i32_t(10), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_HALF_64) );
   EXPECT_EQ ( mama_i32_t(MAMA_PRICE_PREC_100), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_CENTS) );
   EXPECT_EQ ( mama_i32_t(MAMA_PRICE_PREC_100), mamaPrice_precision2Decimals(MAMA_PRICE_PREC_PENNIES) );

}

TEST_F (MamaPriceTestC, testPrecision2DenomMamaPrice)
{

   EXPECT_EQ ( mama_i32_t(0), mamaPrice_precision2Denom(MAMA_PRICE_PREC_UNKNOWN) );
   EXPECT_EQ ( mama_i32_t(0), mamaPrice_precision2Denom(MAMA_PRICE_PREC_10) );
   EXPECT_EQ ( mama_i32_t(0), mamaPrice_precision2Denom(MAMA_PRICE_PREC_100) );
   EXPECT_EQ ( mama_i32_t(0), mamaPrice_precision2Denom(MAMA_PRICE_PREC_1000) );
   EXPECT_EQ ( mama_i32_t(0), mamaPrice_precision2Denom(MAMA_PRICE_PREC_10000) );
   EXPECT_EQ ( mama_i32_t(0), mamaPrice_precision2Denom(MAMA_PRICE_PREC_100000) );
   EXPECT_EQ ( mama_i32_t(0), mamaPrice_precision2Denom(MAMA_PRICE_PREC_1000000) );
   EXPECT_EQ ( mama_i32_t(0), mamaPrice_precision2Denom(MAMA_PRICE_PREC_10000000) );
   EXPECT_EQ ( mama_i32_t(0), mamaPrice_precision2Denom(MAMA_PRICE_PREC_100000000) );
   EXPECT_EQ ( mama_i32_t(0), mamaPrice_precision2Denom(MAMA_PRICE_PREC_1000000000) );
   EXPECT_EQ ( mama_i32_t(0), mamaPrice_precision2Denom(MAMA_PRICE_PREC_10000000000) );
   EXPECT_EQ ( mama_i32_t(1), mamaPrice_precision2Denom(MAMA_PRICE_PREC_INT) );
   EXPECT_EQ ( mama_i32_t(2), mamaPrice_precision2Denom(MAMA_PRICE_PREC_DIV_2) );
   EXPECT_EQ ( mama_i32_t(4), mamaPrice_precision2Denom(MAMA_PRICE_PREC_DIV_4) );
   EXPECT_EQ ( mama_i32_t(8), mamaPrice_precision2Denom(MAMA_PRICE_PREC_DIV_8) );
   EXPECT_EQ ( mama_i32_t(16), mamaPrice_precision2Denom(MAMA_PRICE_PREC_DIV_16) );
   EXPECT_EQ ( mama_i32_t(32), mamaPrice_precision2Denom(MAMA_PRICE_PREC_DIV_32) );
   EXPECT_EQ ( mama_i32_t(64), mamaPrice_precision2Denom(MAMA_PRICE_PREC_DIV_64) );
   EXPECT_EQ ( mama_i32_t(128), mamaPrice_precision2Denom(MAMA_PRICE_PREC_DIV_128) );
   EXPECT_EQ ( mama_i32_t(256), mamaPrice_precision2Denom(MAMA_PRICE_PREC_DIV_256) );
   EXPECT_EQ ( mama_i32_t(512), mamaPrice_precision2Denom(MAMA_PRICE_PREC_DIV_512) );
   EXPECT_EQ ( mama_i32_t(0), mamaPrice_precision2Denom(MAMA_PRICE_PREC_TICK_32) );
   EXPECT_EQ ( mama_i32_t(0), mamaPrice_precision2Denom(MAMA_PRICE_PREC_HALF_32) );
   EXPECT_EQ ( mama_i32_t(0), mamaPrice_precision2Denom(MAMA_PRICE_PREC_QUARTER_32) );
   EXPECT_EQ ( mama_i32_t(0), mamaPrice_precision2Denom(MAMA_PRICE_PREC_TICK_64) );
   EXPECT_EQ ( mama_i32_t(0), mamaPrice_precision2Denom(MAMA_PRICE_PREC_HALF_64) );
   EXPECT_EQ ( mama_i32_t(0), mamaPrice_precision2Denom(MAMA_PRICE_PREC_CENTS) );
   EXPECT_EQ ( mama_i32_t(0), mamaPrice_precision2Denom(MAMA_PRICE_PREC_PENNIES) );

}
