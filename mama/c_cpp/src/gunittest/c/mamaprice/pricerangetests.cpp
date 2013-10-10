/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/gunittest/c/mamaprice/pricerangetests.cpp#3 $
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

#include <gtest/gtest.h>
#include "MainUnitTestC.h"
#include "mama/price.h"
#include "priceimpl.h"

#include <vector>
#include <cstdlib>
#include <iostream>


#ifdef WIN32
#define WMPRICE_LARGE_INT32_FORMAT_SPECIFIER "%I32d"
#define WMPRICE_LARGE_INT64_FORMAT_SPECIFIER "%I64d"
#else
#define WMPRICE_LARGE_INT32_FORMAT_SPECIFIER "%ld"
#define WMPRICE_LARGE_INT64_FORMAT_SPECIFIER "%lld"
#endif

class MamaPriceRangeTestC : public ::testing::Test
{
protected:
    typedef std::vector<double>  DoubleColl;
    typedef DoubleColl::iterator DoubleCollItr;

    typedef std::vector<mamaPricePrecision> PrecisionColl;
    typedef PrecisionColl::iterator         PrecisionCollItr;

    typedef std::vector<mamaPriceHints> HintColl;
    typedef HintColl::iterator          HintCollItr;

    typedef std::vector<mamaPrice> PriceColl;
    typedef PriceColl::iterator    PriceCollItr;

    MamaPriceRangeTestC(void);
    virtual ~MamaPriceRangeTestC(void);

    virtual void SetUp(void);
    virtual void TearDown(void);

    void CreateAndDestroyTest(const mamaPrice& price);
    void AddAndSubtractTest(const mamaPrice& price);
    void NegateAndIsZeroTest(const mamaPrice& price);
    void SetAndGetTest(const mamaPrice& price);
    void SetAndGetTestForStrings(const std::string& szExpected,
                                 const mamaPricePrecision& precision,
                                 double val);
    DoubleColl     m_TestDoubles;
    PrecisionColl  m_TestPrecisions;
    PriceColl      m_TestPrices;
    HintColl       m_TestHints;

private:
   void MakeTestData();
   void MakeDoublesTestData();
   void MakePrecisionsTestData();
   void MakePricesTestData();
   void MakeHintsTestData();
   void DestroyTestData();
   double CreateRandomDouble(double min, double max);
};

MamaPriceRangeTestC::MamaPriceRangeTestC(void)
{
   MakeTestData();
}

MamaPriceRangeTestC::~MamaPriceRangeTestC(void)
{
   DestroyTestData();
}


void 
MamaPriceRangeTestC::SetUp(void)
{
}

void 
MamaPriceRangeTestC::TearDown(void)
{
}

void
MamaPriceRangeTestC::DestroyTestData()
{

   PriceColl::iterator itr;

   for (itr = m_TestPrices.begin(); itr != m_TestPrices.end(); ++itr)
   {
      PriceColl::value_type price = (*itr);

      EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(price) );
   }
}

void
MamaPriceRangeTestC::MakeTestData()
{
   MakeDoublesTestData();
   MakePrecisionsTestData();
   MakePricesTestData();
   MakeHintsTestData();
}

double 
MamaPriceRangeTestC::CreateRandomDouble(double min, double max)
{
    double f = (double)rand() / RAND_MAX;
    return min + f * (max - min);
}

// Populate the doubles test data with, min, max, ZERO, a negative value and a positive value
void
MamaPriceRangeTestC::MakeDoublesTestData()
{
   // MIN
   // m_TestDoubles.push_back( std::numeric_limits<double>::min() );

   // MAX
   //  m_TestDoubles.push_back( std::numeric_limits<double>::max() );

   // ZERO
   //m_TestDoubles.push_back( 0.0 );


   // a Few Random Value 
   //m_TestDoubles.push_back( CreateRandomDouble(-1234567, 1234567) );
   //m_TestDoubles.push_back( CreateRandomDouble(-1234567, 1234567) );
   //m_TestDoubles.push_back( CreateRandomDouble(-1234567, 1234567) );
   //m_TestDoubles.push_back( CreateRandomDouble(-1234567, 1234567) );

   for (int i = 0; i < 10; ++i)
      m_TestDoubles.push_back ( CreateRandomDouble(-1234567890, 1234567890) );
}

// Populate the Price Precision collection with all possible enumerated values
void
MamaPriceRangeTestC::MakePrecisionsTestData()
{
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_UNKNOWN);
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_10);  
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_100);
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_1000);
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_10000);
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_100000); 
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_1000000); 
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_10000000);
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_100000000); 
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_1000000000);
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_10000000000); 
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_INT);        
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_DIV_2);     
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_DIV_4);    
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_DIV_8);   
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_DIV_16); 
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_DIV_32);
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_DIV_64);
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_DIV_128);    
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_DIV_256);   
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_DIV_512);  
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_TICK_32);
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_HALF_32); 
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_QUARTER_32);  
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_TICK_64);    
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_HALF_64);   
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_CENTS);    
    m_TestPrecisions.push_back(MAMA_PRICE_PREC_PENNIES); 
}

// Populate the Hints collection with all possible enumerated values
void 
MamaPriceRangeTestC::MakeHintsTestData()
{
   m_TestHints.push_back(MAMA_PRICE_HINTS_NONE);         
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_PREC_MASK);         
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_PREC_UNKNOWN);     
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_PREC_10);         
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_PREC_100);       
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_PREC_1000);     
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_PREC_10000);   
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_PREC_100000); 
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_PREC_1000000);       
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_PREC_10000000);     
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_PREC_100000000);   
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_PREC_1000000000); 
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_PREC_10000000000);
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_PREC_INT);       
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_PREC_2);        
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_PREC_4);       
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_PREC_8);      
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_PREC_16);    
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_PREC_32);   
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_PREC_64);  
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_PREC_128);
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_PREC_256);         
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_PREC_512);         
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_PREC_TICK_32);    
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_PREC_HALF_32);   
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_PREC_QUARTER_32);   
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_PREC_TICK_64);    
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_PREC_HALF_64);   
   m_TestHints.push_back(MAMA_PRICE_IMPL_HINT_VALID_PRICE);   
}

// Populate the Prices Collection with a price object for each possible
// combination of double and precision
void
MamaPriceRangeTestC::MakePricesTestData()
{
   DoubleCollItr dblItr; 

   for (dblItr = m_TestDoubles.begin(); dblItr != m_TestDoubles.end(); ++dblItr)
   {
      DoubleColl::value_type val = (*dblItr);
      PrecisionCollItr prcItr;

      for (prcItr = m_TestPrecisions.begin(); prcItr != m_TestPrecisions.end(); ++prcItr)
      {
         PrecisionColl::value_type precision = (*prcItr); 
         mamaPrice price;

         mamaPrice_create(&price);
         mamaPrice_setValue(price,val);
         mamaPrice_setPrecision(price, precision);         

        m_TestPrices.push_back(price);
      }
   }
}

void 
MamaPriceRangeTestC::CreateAndDestroyTest(const mamaPrice& price)
{

   int eq = 1;
   mamaPrice tmp;

   // Copy price into temp
   // Prices are equal
   // Clear tmp
   // Prices are not equal
   // Destroy temp
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_create(&tmp) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_copy(tmp,price) );
   EXPECT_EQ ( eq, mamaPrice_equal(price,tmp) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_clear(tmp) );
   EXPECT_NE ( eq, mamaPrice_equal(price,tmp) );

   // Destroy the temp price Object
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(tmp) );
}

TEST_F (MamaPriceRangeTestC, testCreateAndDestroy)
{

   PriceColl::iterator itr;

   for (itr = m_TestPrices.begin(); itr != m_TestPrices.end(); ++itr)
   {
      PriceColl::value_type price = (*itr);

      CreateAndDestroyTest(price);
   }
}

void
MamaPriceRangeTestC::AddAndSubtractTest(const mamaPrice& price)
{
   mamaPrice tmp;

   double dbPositive = 1.23, dbNegative = -1.23, oldVal, newVal;
   
   // Create temporary copies of the input Price
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_create(&tmp) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_copy(tmp,price) );

   // Add a positive number to a price AKA increment
   // Get its value
   // Check that the new value eq uals old value + increment 
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getValue(price, &oldVal) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_setValue(tmp, dbPositive) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_add(tmp,price) ); 
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getValue(tmp, &newVal) );
   EXPECT_EQ ( newVal, dbPositive + oldVal );

   // Same scenario as above except add a negative number.
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getValue(price, &oldVal) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_setValue(tmp, dbNegative) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_add(tmp,price) ); 
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getValue(tmp, &newVal) );
   EXPECT_EQ ( newVal, dbNegative + oldVal );

   // SUbtract a positive number from a price AKA increment
   // Get its value
   // Check that the new value eq uals old value - increment 
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getValue(price, &oldVal) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_setValue(tmp, dbPositive) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_subtract(tmp,price) ); 
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getValue(tmp, &newVal) );
   EXPECT_EQ ( newVal, dbPositive - oldVal );

   // Same scenario as above except subtract a negative number.
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getValue(price, &oldVal) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_setValue(tmp, dbNegative) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_subtract(tmp,price) ); 
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getValue(tmp, &newVal) );
   EXPECT_EQ ( newVal, dbNegative - oldVal );

   // Destroy the temp price Object
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(tmp) );
}


TEST_F (MamaPriceRangeTestC, testAddAndSubtract)
{
   PriceColl::iterator itr;

   for (itr = m_TestPrices.begin(); itr != m_TestPrices.end(); ++itr)
   {
      PriceColl::value_type price = (*itr);

      AddAndSubtractTest(price);
   }
}

void
MamaPriceRangeTestC::NegateAndIsZeroTest(const mamaPrice& price)
{
   mamaPrice tmp;

   double origVal, tmpVal; 

   // Create temporary copies of the input Price
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_create(&tmp) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_copy(tmp,price) );

   // Get the original value from the price object
   // Negate the price value
   // Get the negated price value
   // Check that the negated price value = the old value * -1
   // Negate thje price object again
   // Get this new value
   // Check that the lates price value equals the original value
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getValue(tmp, &origVal) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_negate(tmp) ); 
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getValue(tmp, &tmpVal) );
   EXPECT_EQ ( tmpVal, origVal * -1 );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_negate(tmp) ); 
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getValue(tmp, &tmpVal) );
   EXPECT_EQ ( tmpVal, origVal );


   // Test of the price object is ZERO
   mama_bool_t isZero;   

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_isZero(tmp, &isZero) );
   EXPECT_EQ ( origVal == 0, isZero );

   // Set the value to Zero 
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_setValue(tmp, 0.0) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_isZero(tmp, &isZero) );
   EXPECT_EQ ( true, isZero );

   // Destroy the temp price Object
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(tmp) );
}

TEST_F (MamaPriceRangeTestC, testNegateAndIsZero)
{

   PriceColl::iterator itr;

   for (itr = m_TestPrices.begin(); itr != m_TestPrices.end(); ++itr)
   {
      PriceColl::value_type price = (*itr);

      NegateAndIsZeroTest(price);
   }
}

void
MamaPriceRangeTestC::SetAndGetTest(const mamaPrice& price)
{
   mamaPrice tmp;

   double dbConstant = 1.23, origVal, currVal;

   // Create temporary copies of the input Price
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_create(&tmp) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_copy(tmp,price) );

   // Get the original value from the price object
   // Use this to create a new current value
   // Set the price objects value to this new value
   // Retrieve the price objects value
   // Check that the lates price value is as expected
   // Check that the compare method works
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getValue(tmp, &origVal) );
   currVal = origVal * dbConstant;
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_setValue(tmp, currVal) );
   currVal = 0.0;
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getValue(tmp, &currVal) );
   EXPECT_EQ ( currVal, origVal * dbConstant );

   EXPECT_EQ ( 0.0, mamaPrice_compare(price, price) );
   EXPECT_EQ ( origVal - currVal, mamaPrice_compare(price, tmp) );
   EXPECT_EQ ( currVal - origVal, mamaPrice_compare(tmp, price) );

   // Similar test to above except get and set the precision for all price precisions
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_copy(tmp,price) );
   for (PrecisionCollItr itr = m_TestPrecisions.begin();
                         itr != m_TestPrecisions.end(); 
                         ++itr)
   {
      PrecisionColl::value_type p1 = (*itr), p2;

      EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_setPrecision(tmp, p1) );
      EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getPrecision(tmp, &p2) );
      EXPECT_EQ ( p1, p2 );
   }

   // Similar test to above except get and set the hints for all price precisions
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_copy(tmp,price) );
   for (HintCollItr itr = m_TestHints.begin(); 
                    itr != m_TestHints.end(); 
                    ++itr)
   {
      HintColl::value_type h1 = (*itr), h2;

      EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_setHints(tmp, h1) );
      EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getHints(tmp, &h2) );
      EXPECT_EQ ( h1, h2 );
   }

   // Similar test to above except get and set with hints for all price precisions
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_copy(tmp,price) );
   for (HintCollItr itr = m_TestHints.begin(); 
                    itr != m_TestHints.end(); 
                    ++itr)
   {
      HintColl::value_type h1 = (*itr), h2;
      mamaPricePrecision p;
      double v1 = 1.234, v2; 

      EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_setWithHints(tmp, v1, h1) );
      EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getWithHints(tmp, &v2, &p) );
      EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getHints(tmp, &h2) );
      EXPECT_EQ ( v1, v2 );
      EXPECT_EQ ( h1, h2 );
   }

   // Destroy the temp price Object
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(tmp) );
}

TEST_F (MamaPriceRangeTestC, testSetAndGet)
{
   PriceColl::iterator itr;

   for (itr = m_TestPrices.begin(); itr != m_TestPrices.end(); ++itr)
   {
      PriceColl::value_type price = (*itr);

      SetAndGetTest(price);
   }
}

void 
MamaPriceRangeTestC::SetAndGetTestForStrings(const std::string& szExpected,
                                             const mamaPricePrecision& precision,
                                             double val)
{
   mamaPrice tmp;
   mama_size_t size = 50;
   char buff[50];

   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_create(&tmp) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_setValue(tmp, val) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_setPrecision(tmp, precision) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_getAsString(tmp, buff, size) );
   EXPECT_STREQ ( szExpected.c_str(), buff );

   // Destroy the temp price Object
   EXPECT_EQ ( MAMA_STATUS_OK, mamaPrice_destroy(tmp) );
}


TEST_F (MamaPriceRangeTestC, testSetAndGetForString)
{

   // Test Valeu, a number with 20 decimal places
   double val = 1.12345678901234567890;
   std::string szZeroDP      = "1",
               szOneDP       = "1.1",
               szTwoDP       = "1.12",
               szThreeDP     = "1.123",
               szFourDP      = "1.1235",
               szFiveDP      = "1.12346",
               szSixDP       = "1.123457",
               szSevenDP     = "1.1234568",
               szEightDP     = "1.12345679",
               szNineDP      = "1.123456789",
               szTenDP       = "1.1234567890",
               szUnknownDP   = "1.12345678901234567891";

   SetAndGetTestForStrings(szZeroDP,    MAMA_PRICE_PREC_INT, val); 
   SetAndGetTestForStrings(szOneDP,     MAMA_PRICE_PREC_10, val); 
   SetAndGetTestForStrings(szTwoDP,     MAMA_PRICE_PREC_100, val); 
   SetAndGetTestForStrings(szThreeDP,   MAMA_PRICE_PREC_1000, val); 
   SetAndGetTestForStrings(szFourDP,    MAMA_PRICE_PREC_10000, val); 
   SetAndGetTestForStrings(szFiveDP,    MAMA_PRICE_PREC_100000, val); 
   SetAndGetTestForStrings(szSixDP,     MAMA_PRICE_PREC_1000000, val); 
   SetAndGetTestForStrings(szSevenDP,   MAMA_PRICE_PREC_10000000, val); 
   SetAndGetTestForStrings(szEightDP,   MAMA_PRICE_PREC_100000000, val); 
   SetAndGetTestForStrings(szNineDP,    MAMA_PRICE_PREC_1000000000, val); 
   SetAndGetTestForStrings(szTenDP,     MAMA_PRICE_PREC_10000000000, val); 
   SetAndGetTestForStrings(szUnknownDP, MAMA_PRICE_PREC_UNKNOWN, val); 
}
