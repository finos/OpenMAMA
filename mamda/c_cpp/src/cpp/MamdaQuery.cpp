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

#include <mamda/MamdaQuery.h>
#include <wombat/port.h>
#include <mama/mamacpp.h>
#include <vector>
#include <string.h>

using namespace std;

namespace Wombat
{

    enum condType
    {
        COND_TYPE_UNKNOWN   = 0,
        COND_TYPE_BOOL      = 1,
        COND_TYPE_INT       = 2,
        COND_TYPE_DOUBLE    = 3,
        COND_TYPE_CHAR      = 4,
        COND_TYPE_DATE      = 5
    };

    class MamdaQuery::QueryImpl
    {
    public:
        QueryImpl  (); 
        ~QueryImpl () {};
        
        void addCondition (MamdaQuery* query1);
        bool getXML       (char *restr);
        int  getDepth     ();

        vector <MamdaQuery*> mConditions;
    };
     
    class MamdaQuery::QueryInfoImpl
    {
    public:
        QueryInfoImpl  () {mSubsc = NULL;}; 
        ~QueryInfoImpl () {};
        
        void               setSubscriptionInfo (MamdaSubscription* subscInfo);
        MamdaSubscription* getSubscriptionInfo ();
        MamdaSubscription* mSubsc;
    };
     
     
    class MamdaQuery::CondImpl
    {
    public:
        CondImpl  (); 
        ~CondImpl () {};
        
        void setBool    (bool        val);
        void setInt     (int         val);
        void setDouble  (double      val);
        void setString  (const char* val);
        void setField   (const char* name);
        void setDate    (MamaDateTime& start, MamaDateTime& end);
        
        bool addInt     (int         val);
        bool addDouble  (double      val);
        bool addString  (const char* val);
        
        bool getXML     (char* restr);
        int  getDepth   ();

        condType        mType;
        bool            bVal;
        vector <char*>  mStrings;
        vector <double> mDoubles;
        vector <int>    mInts;
        double          startDate;
        double          endDate;
        char*           fieldName;
        
        void                setSubscriptionInfo (MamdaSubscription* subscInfo);
        MamdaSubscription*  getSubscriptionInfo ();
        MamdaSubscription*  mSubsc;
    };
     
     
     MamdaQuery::MamdaQuery ()
        : mInfoImpl (*new QueryInfoImpl())
    { 
    }

    bool MamdaQuery::getQuery (char *&restr)
    {
        char* mXML = new char [2048];
           
        snprintf (mXML, 256, "<QUERY><VERSION>1.0</VERSION>");
        
        getXML   (mXML);
            
        strcat   (mXML, "</QUERY>");
        
	    restr = mXML;

        return true;
    }
     
    void MamdaQuery::setSubscriptionInfo (MamdaSubscription* subscInfo)
    {
        mInfoImpl.setSubscriptionInfo (subscInfo);
    }

    MamdaSubscription* MamdaQuery::getSubscriptionInfo ()
    {
        return mInfoImpl.getSubscriptionInfo ();
    }
      
      
    void MamdaQuery::QueryInfoImpl::setSubscriptionInfo (MamdaSubscription* subscInfo)
    {
        mSubsc = subscInfo;
    }

    MamdaSubscription* MamdaQuery::QueryInfoImpl::getSubscriptionInfo ()
    {
        return mSubsc;
    }
        
    MamdaOrQuery::MamdaOrQuery (MamdaQuery* query1, MamdaQuery* query2)
        : mImpl(*new QueryImpl())
    { 
        mImpl.addCondition (query1);
        mImpl.addCondition (query2);
    }

    void MamdaOrQuery::addQuery (MamdaQuery* query1)
    {
        mImpl.addCondition (query1);
    }

    bool MamdaOrQuery::getXML (char *restr)
    {
        strcat (restr, "<OR>");
        mImpl.getXML (restr);
        strcat (restr, "</OR>");
        
        return true;
    }

    int MamdaOrQuery::getDepth ()
    {
        return mImpl.getDepth();
    }

    MamdaAndQuery::MamdaAndQuery (MamdaQuery* query1, MamdaQuery* query2)
        : mImpl(*new QueryImpl())
    {
        mImpl.addCondition (query1);
        mImpl.addCondition (query2);
    }

    void MamdaAndQuery::addQuery (MamdaQuery* query1)
    {
        mImpl.addCondition (query1);
    }

    bool MamdaAndQuery::getXML (char *restr)
    {
        strcat (restr, "<AND>");
        
        mImpl.getXML (restr);
            
        strcat (restr, "</AND>");
        
        return true;
    }

    int MamdaAndQuery::getDepth ()
    {
        return mImpl.getDepth();
    }

    MamdaEqualsQuery::MamdaEqualsQuery (const char* field, const char* val)
        : mImpl(*new CondImpl())
    {
        mImpl.setString (val);
        mImpl.setField  (field);
    }

    MamdaEqualsQuery::MamdaEqualsQuery (const char* field, double val)
        : mImpl(*new CondImpl())
    {
        mImpl.setDouble (val);
        mImpl.setField  (field);
    }

    MamdaEqualsQuery::MamdaEqualsQuery (const char* field, int val)
        : mImpl(*new CondImpl())
    {
        mImpl.setInt   (val);
        mImpl.setField (field);
    }

    MamdaEqualsQuery::MamdaEqualsQuery (const char* field, bool val)
        : mImpl(*new CondImpl())
    {
        mImpl.setBool  (val);
        mImpl.setField (field);
    }
        
    bool MamdaEqualsQuery::getXML (char *restr)
    {
        strcat (restr, "<EQUALS>");
        mImpl.getXML(restr);
        strcat (restr, "</EQUALS>");
        return true;
    }

    int MamdaEqualsQuery::getDepth ()
    {
        return mImpl.getDepth();
    }

    bool MamdaEqualsQuery::addItem (const char* item)
    {
        return mImpl.addString (item);
    }

    bool MamdaEqualsQuery::addItem (int item)
    {
        return mImpl.addInt (item);
    }

    bool MamdaEqualsQuery::addItem (double item)
    {
        return mImpl.addDouble (item);
    }

    MamdaDateQuery::MamdaDateQuery (MamaDateTime& start, MamaDateTime& end)
        : mImpl(*new CondImpl())
    {
        mImpl.setDate (start, end);
    }

    bool MamdaDateQuery::getXML (char *restr)
    {
	    strcat (restr, "<DATE>");
        mImpl.getXML(restr);
        strcat (restr, "</DATE>");
        return true;
    }

    int MamdaDateQuery::getDepth ()
    {
        return mImpl.getDepth();
    }

    MamdaContainsAllQuery::MamdaContainsAllQuery (const char* field, const char * item)
        : mImpl(*new CondImpl())
    {
        mImpl.setField  (field);
        mImpl.setString (item);
    }

    MamdaContainsAllQuery::MamdaContainsAllQuery (const char* field, double item)
        : mImpl(*new CondImpl())
    {
        mImpl.setField  (field);
        mImpl.setDouble (item);
    }

    MamdaContainsAllQuery::MamdaContainsAllQuery (const char* field, int item)
        : mImpl(*new CondImpl())
    {
        mImpl.setField (field);
        mImpl.setInt   (item);
    }

    bool MamdaContainsAllQuery::addItem (const char *item)
    {
        return mImpl.addString (item);
    }


    bool MamdaContainsAllQuery::addItem (int item)
    {
        return mImpl.addInt (item);
    }

    bool MamdaContainsAllQuery::addItem (double item)
    {
        return mImpl.addDouble (item);
    }

    bool MamdaContainsAllQuery::getXML (char *restr)
    {
        strcat (restr, "<CONTAINSALL>");
        mImpl.getXML(restr);
        strcat (restr, "</CONTAINSALL>");
        return true;
    }

    int MamdaContainsAllQuery::getDepth ()
    {
        return mImpl.getDepth();
    }

    MamdaContainsQuery::MamdaContainsQuery (const char *field, const char * item)
        : mImpl(*new CondImpl())
    {
        mImpl.setField (field);
        mImpl.setString (item);
    }

    MamdaContainsQuery::MamdaContainsQuery (const char *field, double item)
        : mImpl(*new CondImpl())
    {
        mImpl.setField (field);
        mImpl.setDouble (item);
    }

    MamdaContainsQuery::MamdaContainsQuery (const char *field, int item)
        : mImpl(*new CondImpl())
    {
        mImpl.setField (field);
        mImpl.setInt (item);
    }

    bool MamdaContainsQuery::addItem (const char *item)
    {
        return mImpl.addString (item);
    }

    bool MamdaContainsQuery::addItem (int item)
    {
        return mImpl.addInt (item);
    }

    bool MamdaContainsQuery::addItem (double item)
    {
        return mImpl.addDouble (item);
    }

    bool MamdaContainsQuery::getXML (char *restr)
    {
        strcat (restr, "<CONTAINS>");
        mImpl.getXML(restr);
        strcat (restr, "</CONTAINS>");
        return true;
    }

    int MamdaContainsQuery::getDepth ()
    {
        return mImpl.getDepth();
    }

    MamdaQuery::QueryImpl::QueryImpl ()
    {
    }

    MamdaQuery::CondImpl::CondImpl ()
    {
    }

    void MamdaQuery::QueryImpl::addCondition (MamdaQuery* query1)
    {
        mConditions.push_back (query1);
    }

    bool MamdaQuery::QueryImpl::getXML (char *restr)
    {
         for (size_t i=0; i<mConditions.size(); i++)
            (mConditions[i])->getXML(restr);
            
         return true;
    }

    int MamdaQuery::QueryImpl::getDepth ()
    {
        int level=0, newlevel=0;
        
        for (size_t i=0; i<mConditions.size(); i++)
        {
            if ((newlevel = (mConditions[i])->getDepth()) >= level)
                level = newlevel + 1;
        }
        
        return level;
    }

    void MamdaQuery::CondImpl::setField (const char* name)
    {
        fieldName = strdup(name);
    }

    void MamdaQuery::CondImpl::setBool (bool val)
    {
        bVal  = val;
        mType = COND_TYPE_BOOL;
    }

    void MamdaQuery::CondImpl::setInt (int val)
    {
        mInts.push_back (val);
        mType = COND_TYPE_INT;
    }

    void MamdaQuery::CondImpl::setString (const char* val)
    {
        mStrings.push_back (strdup (val));
        mType = COND_TYPE_CHAR;
    }

    void MamdaQuery::CondImpl::setDouble (double val)
    {
        mDoubles.push_back (val);
        mType = COND_TYPE_DOUBLE;
    }
     
    void MamdaQuery::CondImpl::setDate (MamaDateTime& start, MamaDateTime& end)
    {

        startDate = start.getEpochTimeMilliseconds();
        endDate   =   end.getEpochTimeMilliseconds();

        mType = COND_TYPE_DATE;
    }
         
    bool MamdaQuery::CondImpl::addInt (int val)
    {
        if (mType != COND_TYPE_INT)
            return false;
            
        mInts.push_back(val);
        return (true);

    }

    bool MamdaQuery::CondImpl::addString (const char* val)
    {
        if (mType != COND_TYPE_CHAR)
            return false;
            
        char* newItem = strdup(val);
                
        mStrings.push_back(newItem);
        return (true);
    }

    bool MamdaQuery::CondImpl::addDouble (double val)
    {
        if (mType != COND_TYPE_DOUBLE)
            return false;
            
        mDoubles.push_back (val);
        return (true);
    }

    bool MamdaQuery::CondImpl::getXML (char *restr)
    {
        size_t size = 256;
        char tempbuf [256];
        
        switch (mType)
        {
            case COND_TYPE_BOOL:
                snprintf (tempbuf, 
                          size, 
                          "<field>%s</field><value type=\"string\">%s</value>", 
                          fieldName, 
                          (bVal) ? "true" : "false");
                strcat (restr, tempbuf);
                break;

            case COND_TYPE_INT:

                snprintf (tempbuf, 
                          size,
                          "<field>%s</field>", 
                          fieldName);
                strcat (restr, tempbuf);

                if (mInts.size()>1)
                {
                    snprintf (tempbuf,
                              size,
                              "<list type=\"int\">");
                    strcat (restr, tempbuf);

                    for (size_t i=0; i<mInts.size(); i++)
                    {
                        snprintf (tempbuf,  
                                  size,
                                  "<item>%d</item>", 
                                  mInts[i]);
                    }

                    snprintf (tempbuf, 
                              size,
                              "</list>");
                    strcat (restr, tempbuf);
                }
                else
                {
                    snprintf (tempbuf,  
                              size,
                              "<value type=\"int\">%d</value>", 
                              mInts[0]);
                    strcat (restr, tempbuf);
                }
                break;

            case COND_TYPE_CHAR:
                snprintf (tempbuf,  
                          size,
                          "<field>%s</field>", 
                          fieldName);
                strcat (restr, tempbuf);

                if (mStrings.size()>1)
                {
                    snprintf (tempbuf,  
                              size,
                              "<list type=\"string\">");
                    strcat (restr, tempbuf);

                    for (size_t i=0; i<mStrings.size(); i++)
                    {
                        snprintf (tempbuf,  
                                  size,
                                  "<item>%s</item>", 
                                  mStrings[i]);
                        strcat (restr, tempbuf);
                    }

                    snprintf (tempbuf,  
                              size,
                              "</list>");
                    strcat (restr, tempbuf);
                }
                else
                {
                    snprintf (tempbuf,  
                              size,
                              "<value type=\"string\">%s</value>", 
                              mStrings[0]);
                    strcat (restr, tempbuf);
                }
                break;

            case COND_TYPE_DATE:
                snprintf (tempbuf,  
                          size,
                          "<start type=\"double\">%.0f</start><end type=\"double\">%.0f</end>", 
                          startDate, 
                          endDate);
                strcat (restr, tempbuf);
                break;

            case COND_TYPE_DOUBLE:
                snprintf (tempbuf,  
                          size,
                          "<field>%s</field>", 
                          fieldName);
                strcat (restr, tempbuf);

                if (mDoubles.size()>1)
                {
                    snprintf (tempbuf,  
                              size,
                              "<list type=\"double\">");
                    strcat (restr, tempbuf);

                    for (size_t i=0; i<mDoubles.size(); i++)
                    {
                        snprintf (tempbuf,  
                                  size,
                                  "<item>%f</item>", 
                                  mDoubles[i]);
                        strcat (restr, tempbuf);
                    }
                    snprintf (tempbuf,  
                              size,
                              "</list>");
                    strcat (restr, tempbuf);
                }
                else
                {
                    snprintf (tempbuf,  
                              size,
                              "<value type=\"double\">%f</value>", 
                              mDoubles[0]);
                    strcat (restr, tempbuf);
                }
                break;
            default:
                break;
        }
        
        return true;
    }

    int MamdaQuery::CondImpl::getDepth ()
    {   
        return 1;
    }

}

