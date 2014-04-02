/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/cpp/mama/fieldcache/MamaFieldCacheFieldTypes.h#1 $
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

#ifndef MAMA_FIELD_CACHE_FIELD_TYPES_CPP_H__
#define MAMA_FIELD_CACHE_FIELD_TYPES_CPP_H__

#include <mama/mamacpp.h>
#include <mama/fieldcache/MamaFieldCacheField.h>
#include <mama/MamaPrice.h>
#include <mama/MamaDateTime.h>
#include <stdexcept>

namespace Wombat 
{

/**
 * MamaFieldCacheFieldBasic.
 * Base class for basic single-value types. Allows to set and get the value of the field.
 */
template <typename T, mamaFieldType fieldType>
class MAMACPPExpDLL MamaFieldCacheFieldBasic : public MamaFieldCacheFieldBase
{
public:
    virtual ~MamaFieldCacheFieldBasic() {}

    /**
     * Set the field value.
     * 
     * @param field The field to set the value to.
     * @param value The new value of the field.
     */
    void set(MamaFieldCacheField& field, const T& value);
    /**
     * Return the value of the field.
     * 
     * @param field The field to get the value from.
     * @return The value of the field.
     */
    T get(const MamaFieldCacheField& field) const;

protected:
    void checkType(const MamaFieldCacheField& field) const
    {
        if (field.getType() != fieldType)
            throw std::invalid_argument("MamaFieldCacheFieldBasic");
    }
};

// Basic single-value types
/**
 * MamaFieldCacheFieldChar.
 * Class used to set and get the value of a char single-value <code>MamaFieldCacheField</code>.
 */
typedef MamaFieldCacheFieldBasic<char, MAMA_FIELD_TYPE_CHAR>
    MamaFieldCacheFieldChar;
/**
 * MamaFieldCacheFieldBool.
 * Class used to set and get the value of a bool single-value <code>MamaFieldCacheField</code>.
 */
typedef MamaFieldCacheFieldBasic<mama_bool_t, MAMA_FIELD_TYPE_BOOL>
    MamaFieldCacheFieldBool;
/**
 * MamaFieldCacheFieldI8.
 * Class used to set and get the value of a mama_i8_t single-value <code>MamaFieldCacheField</code>.
 */
typedef MamaFieldCacheFieldBasic<mama_i8_t, MAMA_FIELD_TYPE_I8>
    MamaFieldCacheFieldI8;
/**
 * MamaFieldCacheFieldU8.
 * Class used to set and get the value of a mama_u8_t single-value <code>MamaFieldCacheField</code>.
 */
typedef MamaFieldCacheFieldBasic<mama_u8_t, MAMA_FIELD_TYPE_U8>
    MamaFieldCacheFieldU8;
/**
 * MamaFieldCacheFieldI16.
 * Class used to set and get the value of a mama_i16_t single-value <code>MamaFieldCacheField</code>.
 */
typedef MamaFieldCacheFieldBasic<mama_i16_t, MAMA_FIELD_TYPE_I16>
    MamaFieldCacheFieldI16;
/**
 * MamaFieldCacheFieldU16.
 * Class used to set and get the value of a mama_u16_t single-value <code>MamaFieldCacheField</code>.
 */
typedef MamaFieldCacheFieldBasic<mama_u16_t, MAMA_FIELD_TYPE_U16>
    MamaFieldCacheFieldU16;
/**
 * MamaFieldCacheFieldI32.
 * Class used to set and get the value of a mama_i32_t single-value <code>MamaFieldCacheField</code>.
 */
typedef MamaFieldCacheFieldBasic<mama_i32_t, MAMA_FIELD_TYPE_I32>
    MamaFieldCacheFieldI32;
/**
 * MamaFieldCacheFieldU32.
 * Class used to set and get the value of a mama_u32_t single-value <code>MamaFieldCacheField</code>.
 */
typedef MamaFieldCacheFieldBasic<mama_u32_t, MAMA_FIELD_TYPE_U32>
    MamaFieldCacheFieldU32;
/**
 * MamaFieldCacheFieldI64.
 * Class used to set and get the value of a mama_i64_t single-value <code>MamaFieldCacheField</code>.
 */
typedef MamaFieldCacheFieldBasic<mama_i64_t, MAMA_FIELD_TYPE_I64>
    MamaFieldCacheFieldI64;
/**
 * MamaFieldCacheFieldU64.
 * Class used to set and get the value of a mama_u64_t single-value <code>MamaFieldCacheField</code>.
 */
typedef MamaFieldCacheFieldBasic<mama_u64_t, MAMA_FIELD_TYPE_U64>
    MamaFieldCacheFieldU64;
/**
 * MamaFieldCacheFieldF32.
 * Class used to set and get the value of a mama_f32_t single-value <code>MamaFieldCacheField</code>.
 */
typedef MamaFieldCacheFieldBasic<mama_f32_t, MAMA_FIELD_TYPE_F32>
    MamaFieldCacheFieldF32;
/**
 * MamaFieldCacheFieldF64.
 * Class used to set and get the value of a mama_f64_t single-value <code>MamaFieldCacheField</code>.
 */
typedef MamaFieldCacheFieldBasic<mama_f64_t, MAMA_FIELD_TYPE_F64>
    MamaFieldCacheFieldF64;

// Special single-value types
/**
 * MamaFieldCacheFieldString.
 * Class used to set and get the value of a char* single-value <code>MamaFieldCacheField</code>.
 */
class MAMACPPExpDLL MamaFieldCacheFieldString : public MamaFieldCacheFieldBase
{
public:
    /**
     * Set the field value.
     * 
     * @param field The field to set the value to.
     * @param value The new value of the field.
     */
    void set(MamaFieldCacheField& field, const char* value, mama_size_t len = 0);
    /**
     * Return the value of the field.
     * 
     * @param field The field to get the value from.
     * @return The value of the field.
     */
    const char* get(const MamaFieldCacheField& field) const;

protected:
    void checkType(const MamaFieldCacheField& field) const
    {
        if (field.getType() != MAMA_FIELD_TYPE_STRING)
            throw std::invalid_argument("MamaFieldCacheFieldString");
    }
};

/**
 * MamaFieldCacheFieldPrice.
 * Class used to set and get the value of a MamaPrice single-value <code>MamaFieldCacheField</code>.
 */
class MAMACPPExpDLL MamaFieldCacheFieldPrice : public MamaFieldCacheFieldBase
{
public:
    /**
     * Set the field value.
     * 
     * @param field The field to set the value to.
     * @param value The new value of the field.
     */
    void set(MamaFieldCacheField& field, const MamaPrice& value);
    /**
     * Return the value of the field.
     * 
     * @param field The field to get the value from.
     * @return The value of the field.
     */
    const MamaPrice& get(const MamaFieldCacheField& field) const;

protected:
    void checkType(const MamaFieldCacheField& field) const
    {
        if (field.getType() != MAMA_FIELD_TYPE_PRICE)
            throw std::invalid_argument("MamaFieldCacheFieldPrice");
    }

private:
    mutable MamaPrice mPrice;
};

/**
 * MamaFieldCacheFieldDateTime.
 * Class used to set and get the value of a MamaDateTime single-value <code>MamaFieldCacheField</code>.
 */
class MAMACPPExpDLL MamaFieldCacheFieldDateTime : public MamaFieldCacheFieldBase
{
public:
    /**
     * Set the field value.
     * 
     * @param field The field to set the value to.
     * @param value The new value of the field.
     */
    void set(MamaFieldCacheField& field, const MamaDateTime& value);
    /**
     * Return the value of the field.
     * 
     * @param field The field to get the value from.
     * @return The value of the field.
     */
    const MamaDateTime& get(const MamaFieldCacheField& field) const;

protected:
    void checkType(const MamaFieldCacheField& field) const
    {
        if (field.getType() != MAMA_FIELD_TYPE_TIME)
            throw std::invalid_argument("MamaFieldCacheFieldDateTime");
    }

private:
    mutable MamaDateTime mDateTime;
};


/**
 * MamaFieldCacheFieldVectorBasic.
 * Base class for basic vector types. Allows to set and get the value of the field.
 */
template <typename T, mamaFieldType fieldType>
class MAMACPPExpDLL MamaFieldCacheFieldVectorBasic : public MamaFieldCacheFieldBase
{
public:
    /**
     * Set the field value.
     * 
     * @param field The field to set the values to.
     * @param values The new values of the field.
     * @param size The number of fields in <code>values</code>.
     */
    void set(MamaFieldCacheField& field, const T* values, mama_size_t size);
    /**
     * Return the field values.
     * 
     * @param field The field to get the values from.
     * @param values A reference to the values to be returned.
     * @param size A reference to the size of the vector to be returned.
     */
    void get(const MamaFieldCacheField& field, const T*& values, mama_size_t& size) const;
    /**
     * Return the value of a specific field of a vector field.
     * 
     * @param field The field to get the value from.
     * @param index The index of the element of the vector to get the value from.
     * @return The value of the field.
     */
    const T& get(const MamaFieldCacheField& field, mama_size_t index) const;
    
protected:
    void checkType(const MamaFieldCacheField& field) const
    {
        if (field.getType() != fieldType)
            throw std::invalid_argument("MamaFieldCacheFieldVectorBasic");
    }
};

/**
 * MamaFieldCacheFieldI8Vector.
 * Class used to set and get the value of a mama_i8_t vector <code>MamaFieldCacheField</code>.
 */
typedef MamaFieldCacheFieldVectorBasic<mama_i8_t, MAMA_FIELD_TYPE_VECTOR_I8>
    MamaFieldCacheFieldI8Vector;
/**
 * MamaFieldCacheFieldU8Vector.
 * Class used to set and get the value of a mama_u8_t vector <code>MamaFieldCacheField</code>.
 */
typedef MamaFieldCacheFieldVectorBasic<mama_u8_t, MAMA_FIELD_TYPE_VECTOR_U8>
    MamaFieldCacheFieldU8Vector;
/**
 * MamaFieldCacheFieldI16Vector.
 * Class used to set and get the value of a mama_i16_t vector <code>MamaFieldCacheField</code>.
 */
typedef MamaFieldCacheFieldVectorBasic<mama_i16_t, MAMA_FIELD_TYPE_VECTOR_I16>
    MamaFieldCacheFieldI16Vector;
/**
 * MamaFieldCacheFieldU16Vector.
 * Class used to set and get the value of a mama_u16_t vector <code>MamaFieldCacheField</code>.
 */
typedef MamaFieldCacheFieldVectorBasic<mama_u16_t, MAMA_FIELD_TYPE_VECTOR_U16>
    MamaFieldCacheFieldU16Vector;
/**
 * MamaFieldCacheFieldI32Vector.
 * Class used to set and get the value of a mama_i32_t vector <code>MamaFieldCacheField</code>.
 */
typedef MamaFieldCacheFieldVectorBasic<mama_i32_t, MAMA_FIELD_TYPE_VECTOR_I32>
    MamaFieldCacheFieldI32Vector;
/**
 * MamaFieldCacheFieldU32Vector.
 * Class used to set and get the value of a mama_u32_t vector <code>MamaFieldCacheField</code>.
 */
typedef MamaFieldCacheFieldVectorBasic<mama_u32_t, MAMA_FIELD_TYPE_VECTOR_U32>
    MamaFieldCacheFieldU32Vector;
/**
 * MamaFieldCacheFieldI64Vector.
 * Class used to set and get the value of a mama_i64_t vector <code>MamaFieldCacheField</code>.
 */
typedef MamaFieldCacheFieldVectorBasic<mama_i64_t, MAMA_FIELD_TYPE_VECTOR_I64>
    MamaFieldCacheFieldI64Vector;
/**
 * MamaFieldCacheFieldU64Vector.
 * Class used to set and get the value of a mama_u64_t vector <code>MamaFieldCacheField</code>.
 */
typedef MamaFieldCacheFieldVectorBasic<mama_u64_t, MAMA_FIELD_TYPE_VECTOR_U64>
    MamaFieldCacheFieldU64Vector;
/**
 * MamaFieldCacheFieldF32Vector.
 * Class used to set and get the value of a mama_f32_t vector <code>MamaFieldCacheField</code>.
 */
typedef MamaFieldCacheFieldVectorBasic<mama_f32_t, MAMA_FIELD_TYPE_VECTOR_F32>
    MamaFieldCacheFieldF32Vector;
/**
 * MamaFieldCacheFieldF64Vector.
 * Class used to set and get the value of a mama_f64_t vector <code>MamaFieldCacheField</code>.
 */
typedef MamaFieldCacheFieldVectorBasic<mama_f64_t, MAMA_FIELD_TYPE_VECTOR_F64>
    MamaFieldCacheFieldF64Vector;


// Special vector types
/**
 * MamaFieldCacheField: String vector field
 */
class MAMACPPExpDLL MamaFieldCacheFieldStringVector
    : public MamaFieldCacheFieldBase
{
public:
    /**
     * Set the values of a string vector field.
     * 
     * @param field The field to set the values to.
     * @param values The new values of the field.
     * @param size The number of fields in <code>values</code>.
     */
    void set(MamaFieldCacheField& field, const char** values, mama_size_t size);
    /**
     * Return the values of a string vector.
     * 
     * @param field The field to get the values from.
     * @param values A reference to the values to be returned.
     * @param size A reference to the size of the vector to be returned.
     */
    void get(const MamaFieldCacheField& field, const char**& values, mama_size_t& size) const;
    /**
     * Return the value of a specific field of a string vector field.
     * 
     * @param field The field to get the value from.
     * @param index The index of the element of the vector to get the value from.
     * @return The value of the field.
     */
    const char* get(const MamaFieldCacheField& field, mama_size_t index) const;

protected:
    void checkType(const MamaFieldCacheField& field) const
    {
        if (field.getType() != MAMA_FIELD_TYPE_VECTOR_STRING)
            throw std::invalid_argument("MamaFieldCacheFieldStringVector");
    }
};


template <typename T>
class MamaFieldCacheFieldVector
{
protected:
    MamaFieldCacheFieldVector()
        : mValues(NULL)
        , mSize(0)
    {
    }
    ~MamaFieldCacheFieldVector()
    {
        if (!mValues || !mSize)
        {
            return;
        }
        delete[] mValues;
    }

    void grow(mama_size_t newSize) const
    {
        if (newSize <= mSize)
            return;

        T* newVector = new T[newSize];
        for (mama_size_t i = 0; i < mSize; ++i)
        {
            newVector[i] = mValues[i];
        }

        if (mValues) 
            delete[] mValues;

        mValues = newVector;
        mSize = newSize;
    }

protected:
    mutable T* mValues;
    mutable mama_size_t mSize;
};

/**
 * MamaFieldCacheField: Price vector field
 */
class MAMACPPExpDLL MamaFieldCacheFieldPriceVector
    : public MamaFieldCacheFieldBase
    , public MamaFieldCacheFieldVector<MamaPrice>
{
public:
    /**
     * Set the values of a MamaPrice vector field.
     * 
     * @param field The field to set the values to.
     * @param values The new values of the field.
     * @param size The number of fields in <code>values</code>.
     */
    void set(MamaFieldCacheField& field, const MamaPrice* values, mama_size_t size);
    /**
     * Return the values of a MamaPrice vector field.
     * 
     * @param field The field to get the values from.
     * @param values A reference to the values to be returned.
     * @param size A reference to the size of the vector to be returned.
     */
    void get(const MamaFieldCacheField& field, const MamaPrice*& values, mama_size_t& size) const;
    /**
     * Return the value of a specific field of a MamaPrice vector field.
     * 
     * @param field The field to get the value from.
     * @param index The index of the element of the vector to get the value from.
     * @return The value of the field.
     */
    const MamaPrice* get(const MamaFieldCacheField& field, mama_size_t index) const;

protected:
    void checkType(const MamaFieldCacheField& field) const
    {
        if (field.getType() != MAMA_FIELD_TYPE_VECTOR_PRICE)
            throw std::invalid_argument("MamaFieldCacheFieldPriceVector");
    }

private:
    mutable MamaPrice mValue;
};

/**
 * MamaFieldCacheField: DateTime vector field
 */
class MAMACPPExpDLL MamaFieldCacheFieldDateTimeVector
    : public MamaFieldCacheFieldBase
    , public MamaFieldCacheFieldVector<MamaDateTime>
{
public:
    /**
     * Set the values of MamDateTime vector field.
     * 
     * @param field The field to set the values to.
     * @param values The new values of the field.
     * @param size The number of fields in <code>values</code>.
     */
    void set(MamaFieldCacheField& field, const MamaDateTime* values, mama_size_t size);
    /**
     * Return the values of a MamaDateTime vector field.
     * 
     * @param field The field to get the values from.
     * @param values A reference to the values to be returned.
     * @param size A reference to the size of the vector to be returned.
     */
    void get(const MamaFieldCacheField& field, const MamaDateTime*& values, mama_size_t& size) const;
    /**
     * Return the value of a specific field of a MamaDateTime vector field.
     * 
     * @param field The field to get the value from.
     * @param index The index of the element of the vector to get the value from.
     * @return The value of the field.
     */
    const MamaDateTime* get(const MamaFieldCacheField& field, mama_size_t index) const;

protected:
    void checkType(const MamaFieldCacheField& field) const
    {
        if (field.getType() != MAMA_FIELD_TYPE_VECTOR_TIME)
            throw std::invalid_argument("MamaFieldCacheFieldTimeVector");
    }

private:
    mutable MamaDateTime mValue;
};


/**
 * Helper function to set the value of a String field.
 * 
 * @param field The field to set the value to.
 * @param value The value to set.
 */
void setFieldValue(MamaFieldCacheField& field, const char* value);

/**
 * Helper function to set the value of a MamaPrice field.
 * 
 * @param field The field to set the value to.
 * @param value The value to set.
 */
void setFieldValue(MamaFieldCacheField& field, const MamaPrice& value);

/**
 * Helper function to set the value of a MamaDateTime field.
 * 
 * @param field The field to set the value to.
 * @param value The value to set.
 */
void setFieldValue(MamaFieldCacheField& field, const MamaDateTime& value);

/*
template <class Field, class Value>
void setFieldValue(MamaFieldCacheField& field, const Value& value)
{
    Field setField;
    setField.set(field, value);
}
*/

template <mamaFieldType fieldType>
class Type
{
};

template <typename T>
void setV(Type<MAMA_FIELD_TYPE_CHAR> type, MamaFieldCacheField& field, const T& value)
{
    MamaFieldCacheFieldChar setField;
    setField.set(field, value);
}

template <typename T>
void setV(Type<MAMA_FIELD_TYPE_BOOL> type, MamaFieldCacheField& field, const T& value)
{
    MamaFieldCacheFieldBool setField;
    setField.set(field, value);
}

template <typename T>
void setV(Type<MAMA_FIELD_TYPE_I8> type, MamaFieldCacheField& field, const T& value)
{
    MamaFieldCacheFieldI8 setField;
    setField.set(field, value);
}
template <typename T>
void setV(Type<MAMA_FIELD_TYPE_U8> type, MamaFieldCacheField& field, const T& value)
{
    MamaFieldCacheFieldU8 setField;
    setField.set(field, value);
}

template <typename T>
void setV(Type<MAMA_FIELD_TYPE_I16> type, MamaFieldCacheField& field, const T& value)
{
    MamaFieldCacheFieldI16 setField;
    setField.set(field, value);
}
template <typename T>
void setV(Type<MAMA_FIELD_TYPE_U16> type, MamaFieldCacheField& field, const T& value)
{
    MamaFieldCacheFieldU16 setField;
    setField.set(field, value);
}

template <typename T>
void setV(Type<MAMA_FIELD_TYPE_I32> type, MamaFieldCacheField& field, const T& value)
{
    MamaFieldCacheFieldI32 setField;
    setField.set(field, value);
}
template <typename T>
void setV(Type<MAMA_FIELD_TYPE_U32> type, MamaFieldCacheField& field, const T& value)
{
    MamaFieldCacheFieldU32 setField;
    setField.set(field, value);
}

template <typename T>
void setV(Type<MAMA_FIELD_TYPE_I64>, MamaFieldCacheField& field, const T& value)
{
    MamaFieldCacheFieldI64 setField;
    setField.set(field, value);
}
template <typename T>
void setV(Type<MAMA_FIELD_TYPE_U64> type, MamaFieldCacheField& field, const T& value)
{
    MamaFieldCacheFieldU64 setField;
    setField.set(field, value);
}

template <typename T>
void setV(Type<MAMA_FIELD_TYPE_F32> type, MamaFieldCacheField& field, const T& value)
{
    MamaFieldCacheFieldF32 setField;
    setField.set(field, value);
}
template <typename T>
void setV(Type<MAMA_FIELD_TYPE_F64> type, MamaFieldCacheField& field, const T& value)
{
    MamaFieldCacheFieldF64 setField;
    setField.set(field, value);
}

template <typename T>
void getV(Type<MAMA_FIELD_TYPE_CHAR> type, const MamaFieldCacheField& field, T& value)
{
    MamaFieldCacheFieldChar getField;
    value = getField.get(field);
}

template <typename T>
void getV(Type<MAMA_FIELD_TYPE_BOOL> type, const MamaFieldCacheField& field, T& value)
{
    MamaFieldCacheFieldBool getField;
    value = getField.get(field);
}

template <typename T>
void getV(Type<MAMA_FIELD_TYPE_I8> type, const MamaFieldCacheField& field, T& value)
{
    MamaFieldCacheFieldI8 getField;
    value = getField.get(field);
}
template <typename T>
void getV(Type<MAMA_FIELD_TYPE_U8> type, const MamaFieldCacheField& field, T& value)
{
    MamaFieldCacheFieldU8 getField;
    value = getField.get(field);
}

template <typename T>
void getV(Type<MAMA_FIELD_TYPE_I16> type, const MamaFieldCacheField& field, T& value)
{
    MamaFieldCacheFieldI16 getField;
    value = getField.get(field);
}
template <typename T>
void getV(Type<MAMA_FIELD_TYPE_U16> type, const MamaFieldCacheField& field, T& value)
{
    MamaFieldCacheFieldU16 getField;
    value = getField.get(field);
}

template <typename T>
void getV(Type<MAMA_FIELD_TYPE_I32> type, const MamaFieldCacheField& field, T& value)
{
    MamaFieldCacheFieldI32 getField;
    value = getField.get(field);
}
template <typename T>
void getV(Type<MAMA_FIELD_TYPE_U32> type, const MamaFieldCacheField& field, T& value)
{
    MamaFieldCacheFieldU32 getField;
    value = getField.get(field);
}

template <typename T>
void getV(Type<MAMA_FIELD_TYPE_I64> type, const MamaFieldCacheField& field, T& value)
{
    MamaFieldCacheFieldI64 getField;
    value = getField.get(field);
}
template <typename T>
void getV(Type<MAMA_FIELD_TYPE_U64> type, const MamaFieldCacheField& field, T& value)
{
    MamaFieldCacheFieldU64 getField;
    value = getField.get(field);
}

template <typename T>
void getV(Type<MAMA_FIELD_TYPE_F32> type, const MamaFieldCacheField& field, T& value)
{
    MamaFieldCacheFieldF32 getField;
    value = getField.get(field);
}
template <typename T>
void getV(Type<MAMA_FIELD_TYPE_F64> type, const MamaFieldCacheField& field, T& value)
{
    MamaFieldCacheFieldF64 getField;
    value = getField.get(field);
}

/**
 * Helper function to set the value of a generic single-value field.
 * 
 * @param field The field to set the value to.
 * @param value The value to set.
 */
template <typename T>
void setFieldValue(MamaFieldCacheField& field, const T& value)
{
    switch(field.getType())
    {
        case MAMA_FIELD_TYPE_CHAR:
        {
            setV(Type<MAMA_FIELD_TYPE_CHAR>(), field, value);
            break;
        }
        case MAMA_FIELD_TYPE_BOOL:
        {
            setV(Type<MAMA_FIELD_TYPE_BOOL>(), field, value);
            break;
        }
        case MAMA_FIELD_TYPE_I8:
        {
            setV(Type<MAMA_FIELD_TYPE_I8>(), field, value);
            break;
        }
        case MAMA_FIELD_TYPE_U8:
        {
            setV(Type<MAMA_FIELD_TYPE_U8>(), field, value);
            break;
        }
        case MAMA_FIELD_TYPE_I16:
        {
            setV(Type<MAMA_FIELD_TYPE_I16>(), field, value);
            break;
        }
        case MAMA_FIELD_TYPE_U16:
        {
            setV(Type<MAMA_FIELD_TYPE_U16>(), field, value);
            break;
        }
        case MAMA_FIELD_TYPE_I32:
        {
            setV(Type<MAMA_FIELD_TYPE_I32>(), field, value);
            break;
        }
        case MAMA_FIELD_TYPE_U32:
        {
            setV(Type<MAMA_FIELD_TYPE_U32>(), field, value);
            break;
        }
        case MAMA_FIELD_TYPE_I64:
        {
            setV(Type<MAMA_FIELD_TYPE_I64>(), field, value);
            break;
        }
        case MAMA_FIELD_TYPE_U64:
        {
            setV(Type<MAMA_FIELD_TYPE_U64>(), field, value);
            break;
        }
        case MAMA_FIELD_TYPE_F32:
        {
            setV(Type<MAMA_FIELD_TYPE_F32>(), field, value);
            break;
        }
        case MAMA_FIELD_TYPE_F64:
        {
            setV(Type<MAMA_FIELD_TYPE_F64>(), field, value);
            break;
        }
        default:
            break;
    }
}

/**
 * Helper function to get the value of a string field.
 * 
 * @param field The field to get the value from.
 * @param value The value to return.
 */
void getFieldValue(const MamaFieldCacheField& field, const char*& value);

/**
 * Helper function to get the value of a MamaPrice field.
 * 
 * @param field The field to get the value from.
 * @param value The value to return.
 */
void getFieldValue(const MamaFieldCacheField& field, MamaPrice& value);

/**
 * Helper function to get the value of a MamaDateTime field.
 * 
 * @param field The field to get the value from.
 * @param value The value to return.
 */
void getFieldValue(const MamaFieldCacheField& field, MamaDateTime& value);

/**
 * Helper function to get the value of a generic single-value field.
 * 
 * @param field The field to get the value from.
 * @param value The value to return.
 */
template <typename T>
void getFieldValue(const MamaFieldCacheField& field, T& value)
{
    switch(field.getType())
    {
        case MAMA_FIELD_TYPE_CHAR:
        {
            getV(Type<MAMA_FIELD_TYPE_CHAR>(), field, value);
            break;
        }
        case MAMA_FIELD_TYPE_BOOL:
        {
            getV(Type<MAMA_FIELD_TYPE_BOOL>(), field, value);
            break;
        }
        case MAMA_FIELD_TYPE_I8:
        {
            getV(Type<MAMA_FIELD_TYPE_I8>(), field, value);
            break;
        }
        case MAMA_FIELD_TYPE_U8:
        {
            getV(Type<MAMA_FIELD_TYPE_U8>(), field, value);
            break;
        }
        case MAMA_FIELD_TYPE_I16:
        {
            getV(Type<MAMA_FIELD_TYPE_I16>(), field, value);
            break;
        }
        case MAMA_FIELD_TYPE_U16:
        {
            getV(Type<MAMA_FIELD_TYPE_U16>(), field, value);
            break;
        }
        case MAMA_FIELD_TYPE_I32:
        {
            getV(Type<MAMA_FIELD_TYPE_I32>(), field, value);
            break;
        }
        case MAMA_FIELD_TYPE_U32:
        {
            getV(Type<MAMA_FIELD_TYPE_U32>(), field, value);
            break;
        }
        case MAMA_FIELD_TYPE_I64:
        {
            getV(Type<MAMA_FIELD_TYPE_I64>(), field, value);
            break;
        }
        case MAMA_FIELD_TYPE_U64:
        {
            getV(Type<MAMA_FIELD_TYPE_U64>(), field, value);
            break;
        }
        case MAMA_FIELD_TYPE_F32:
        {
            getV(Type<MAMA_FIELD_TYPE_F32>(), field, value);
            break;
        }
        case MAMA_FIELD_TYPE_F64:
        {
            getV(Type<MAMA_FIELD_TYPE_F64>(), field, value);
            break;
        }
        default:
            break;
    }
}

/**
 * Helper function to set the values of a generic vector field.
 * 
 * @param field The field to set the values to.
 * @param values The values to set.
 * @param size The number of values in the field.
 */
template <typename T>
void setFieldValue(MamaFieldCacheField& field, const T* values, mama_size_t size)
{
    switch(field.getType())
    {
        case MAMA_FIELD_TYPE_VECTOR_I8:
        {
            MamaFieldCacheFieldI8Vector getField;
            getField.set(field, (const mama_i8_t*)values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U8:
        {
            MamaFieldCacheFieldU8Vector getField;
            getField.set(field, (const mama_u8_t*)values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_I16:
        {
            MamaFieldCacheFieldI16Vector getField;
            getField.set(field, (const mama_i16_t*)values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U16:
        {
            MamaFieldCacheFieldU16Vector getField;
            getField.set(field, (const mama_u16_t*)values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_I32:
        {
            MamaFieldCacheFieldI32Vector getField;
            getField.set(field, (const mama_i32_t*)values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U32:
        {
            MamaFieldCacheFieldU32Vector getField;
            getField.set(field, (const mama_u32_t*)values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_I64:
        {
            MamaFieldCacheFieldI64Vector getField;
            getField.set(field, (const mama_i64_t*)values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U64:
        {
            MamaFieldCacheFieldU64Vector getField;
            getField.set(field, (const mama_u64_t*)values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_F32:
        {
            MamaFieldCacheFieldF32Vector getField;
            getField.set(field, (const mama_f32_t*)values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_F64:
        {
            MamaFieldCacheFieldF64Vector getField;
            getField.set(field, (const mama_f64_t*)values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_STRING:
        {
            MamaFieldCacheFieldStringVector getField;
            getField.set(field, (const char**)values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_PRICE:
        {
            MamaFieldCacheFieldPriceVector getField;
            getField.set(field, (const MamaPrice*)values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_TIME:
        {
            MamaFieldCacheFieldDateTimeVector getField;
            getField.set(field, (const MamaDateTime*)values, size);
            break;
        }
        default:
            break;
    }
}

/**
 * Helper function to set the values of a string vector field.
 * 
 * @param field The field to set the values to.
 * @param values The values to set.
 * @param size The number of values in the field.
 */
void getFieldValue(const MamaFieldCacheField& field, const char**& values, mama_size_t& size);

// TODO: NOT IMPLEMENTED YET
// Need to fix the C++ MamaPrice and MamaDateTime interface to allow replacing
// the inner C structure (otherwise a copy is needed causing either a memory leak
// or an early delete.
void getFieldValue(const MamaFieldCacheField& field, const MamaPrice*& values, mama_size_t& size);

void getFieldValue(const MamaFieldCacheField& field, const mamaDateTime*& values, mama_size_t& size);

/**
 * Helper function to get the values of a generic vector field.
 * 
 * @param field The field to get the values from.
 * @param values The values to return.
 * @param size The number of values in the field.
 */
template <typename T>
void getFieldValue(const MamaFieldCacheField& field, const T*& values, mama_size_t& size)
{
    switch(field.getType())
    {
        case MAMA_FIELD_TYPE_VECTOR_I8:
        {
            MamaFieldCacheFieldI8Vector getField;
            getField.get(field, (const mama_i8_t*&)values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U8:
        {
            MamaFieldCacheFieldU8Vector getField;
            getField.get(field, (const mama_u8_t*&)values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_I16:
        {
            MamaFieldCacheFieldI16Vector getField;
            getField.get(field, (const mama_i16_t*&)values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U16:
        {
            MamaFieldCacheFieldU16Vector getField;
            getField.get(field, (const mama_u16_t*&)values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_I32:
        {
            MamaFieldCacheFieldI32Vector getField;
            getField.get(field, (const mama_i32_t*&)values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U32:
        {
            MamaFieldCacheFieldU32Vector getField;
            getField.get(field, (const mama_u32_t*&)values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_I64:
        {
            MamaFieldCacheFieldI64Vector getField;
            getField.get(field, (const mama_i64_t*&)values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U64:
        {
            MamaFieldCacheFieldU64Vector getField;
            getField.get(field, (const mama_u64_t*&)values, size);
            break;
        }        
        case MAMA_FIELD_TYPE_VECTOR_F32:
        {
            MamaFieldCacheFieldF32Vector getField;
            getField.get(field, (const mama_f32_t*&)values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_F64:
        {
            MamaFieldCacheFieldF64Vector getField;
            getField.get(field, (const mama_f64_t*&)values, size);
            break;
        }
        default:
            break;
    }
}

void mamaFieldCacheFieldFromString(MamaFieldCacheField& field, const std::string& value);

} // namespace Wombat

#endif // MAMA_FIELD_CACHE_FIELD_TYPES_CPP_H__
