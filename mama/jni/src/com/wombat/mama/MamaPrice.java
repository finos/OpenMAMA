/* $Id:
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

package com.wombat.mama;

public class MamaPrice implements Comparable
{
    static
    {
        initIDs();
    }
    
    /* A long value containing a pointer to the underlying C message structure*/
    private long pricePointer_i = 0;
    protected long getPointerVal()
    {
        return pricePointer_i;
    }
    
    /**
     * MAMA_PRICE_EPSILON a value that is considered equivalent to zero.
     */
    public static final double EPSILON = 0.00000000001;
    
    
    public MamaPrice ()
    {
        _create (); 
    }

    public MamaPrice (double value)
    {
        _create ();
        setValue (value);
        _setPrecision (MamaPricePrecision.PRECISION_100.myPrecision);
    }

    public MamaPrice (double             value,
                      MamaPricePrecision precision)
    {
        _create ();
        setValue (value);
        setPrecision (precision);
    }

    public MamaPrice (MamaPrice src)
    {
        _create ();
        copy (src);
    }    
    
    public int hashCode ()
    {
        return (int) getValue();
    }

    protected Object clone () throws CloneNotSupportedException
    {
        super.clone ();
        return new MamaPrice (this);
    }

    public boolean equals (Object obj)
    {
        if (obj instanceof MamaPrice)
        {
            return (getValue() == ((MamaPrice)obj).getValue ());
        }
        if (obj instanceof Double)
        {
           return (getValue() == ((Double)obj).doubleValue ());
        }

        return false;  
    }

    public int compareTo (Object obj)
    {
        if (obj instanceof Double)
        {
            return Double.compare (getValue (), ((Double)obj).doubleValue ());
        }
        else
        {
            // This will throw a ClassCastException per the compareTo()
            // requirements.
            MamaPrice price = (MamaPrice) obj;
            return Double.compare (getValue(), price.getValue());
        }
    }
    
    public void add (MamaPrice value)
    {
        setValue (getValue () + value.getValue ());
    }

    public void add (double value)
    {
        setValue (getValue () + value);
    }

    public void subtract (MamaPrice value)
    {
        setValue (getValue () - value.getValue ());
    }

    public void subtract (double value)
    {
        setValue (getValue () - value);
    }

    public native void clear ();
    
    public void set (double priceValue,
                     MamaPriceHints hints)
    {
        setValue (priceValue);
        _setHints (hints.mHints);
    }

    public native void setValue (double value);

    public void setPrecision (MamaPricePrecision precision)
    {
        _setPrecision(precision.myPrecision);    
    }

    public void setHints (MamaPriceHints hints)
    {
        _setHints (hints.mHints);
    }
    
    public native double getValue ();
    
	/**
	 * This function obtains the double value of the price rounded to zero precision. 
	 *
	 * @return The rounded value.
	 * @exception com.wombat.mama MamaException thrown if the underlying JNI object has been destroyed.
	 * @exception com.wombat.common.WombatException for other general MAMA errors.
	 */
    public double getRoundedValue ()
    {
    	return _getRoundedValue((byte) 0);
    }
    
	/**
	 * This function obtains the double value of the price rounded to the supplied precision. 
	 *
	 * @param[in] precision The precision to use for rounding.
	 * @return The rounded value.
	 * @exception com.wombat.mama MamaException thrown if the underlying JNI object has been destroyed.
	 * @exception com.wombat.common.WombatException for other general MAMA errors.
	 */
    public double getRoundedValue (MamaPricePrecision precision)
    {
    	return _getRoundedValue(precision.myPrecision);
    }
    
    private native double _getRoundedValue (byte precision);

    public MamaPricePrecision getPrecision ()
    {
        return MamaPricePrecision.precisionFromByte (_getPrecision());
    }

    public MamaPriceHints getHints ()
    {
        return new MamaPriceHints (_getHints());
    }

    public native String toString ();

    public void negate ()
    {
        setValue (-getValue());
    }

    public boolean isZero ()
    {
        double value = getValue ();
        return (value > -EPSILON && value < EPSILON);
    }

    public native void setIsValidPrice (boolean valid);
    
    public native boolean getIsValidPrice ();
    
    public native void copy (MamaPrice price);
    
    public native void destroy (); 
    
    private native void _create ();   
    
    private native void _setPrecision (byte precision);  
       
    private native byte _getPrecision ();  
    
    private native byte _getHints ();  
    
    private native void _setHints (byte hints);  
    
    /*Used to cache ids for callback methods fields*/
    private static native void initIDs();
    
    protected void finalize()
    {
        if (pricePointer_i != 0)
            destroy();
    }
                     
}
