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

package com.wombat.mama;

/**
 * This class is the java equivalent of the C subscription state enumeration.
 */
public class MamaSubscriptionState
{
    /* ************************************************** */
    /* Public Member Variables.
    /* ************************************************** */
    /* The following objects represent the different states of the subscription. */

    /* The state of the subscription is unknown. */
    public static final MamaSubscriptionState MAMA_SUBSCRIPTION_UNKNOWN = new MamaSubscriptionState(0);

    /* The subscription has been allocated in memory. */
    public static final MamaSubscriptionState MAMA_SUBSCRIPTION_ALLOCATED = new MamaSubscriptionState(1);

    /* Inital setup work has been done, mamaSubscription_activate must still be called. Note that this state is only valid
     * for market data subscriptions.
     */
    public static final MamaSubscriptionState MAMA_SUBSCRIPTION_SETUP = new MamaSubscriptionState(2);

    /* The subscription is now on the throttle queue waiting to be fully activated. */
    public static final MamaSubscriptionState MAMA_SUBSCRIPTION_ACTIVATING = new MamaSubscriptionState(3);

    /* The subscription is now fully activated and is processing messages. */
    public static final MamaSubscriptionState MAMA_SUBSCRIPTION_ACTIVATED = new MamaSubscriptionState(4);

    /* The subscription is being de-activated, it will not be fully deactivated until the onDestroy callback is received. */
    public static final MamaSubscriptionState MAMA_SUBSCRIPTION_DEACTIVATING = new MamaSubscriptionState(5);

    /* The subscription has been de-activated, messages are no longer being processed. */
    public static final MamaSubscriptionState MAMA_SUBSCRIPTION_DEACTIVATED = new MamaSubscriptionState(6);

    /* The subscription is being destroyed, it will not be fully destroyed until the onDestroy callback is received. */
    public static final MamaSubscriptionState MAMA_SUBSCRIPTION_DESTROYING = new MamaSubscriptionState(7);

    /* The subscription has been fully destroyed. */
    public static final MamaSubscriptionState MAMA_SUBSCRIPTION_DESTROYED = new MamaSubscriptionState(8);

    /* The subscription is in the process of being de-allocated, this state is only valid if the mamaSubscription_deallocate
     * function is called while the subscription is being destroyed.
     */
    public static final MamaSubscriptionState MAMA_SUBSCRIPTION_DEALLOCATING = new MamaSubscriptionState(9);

    /* The subscription has been de-allocated, this state is only supported so that the log entry will whenever the subscription
     * has finally been freed.
     */
    public static final MamaSubscriptionState MAMA_SUBSCRIPTION_DEALLOCATED = new MamaSubscriptionState(10);

    /* The subscription is being re-activated, this state can only occur if the mamaSubscription_activate has been called while
     * the subscription is being deactivated, (i.e. its state is MAMA_SUBSCRIPTION_DEACTIVATING.
     */
    public static final MamaSubscriptionState MAMA_SUBSCRIPTION_REACTIVATING = new MamaSubscriptionState(11);

    /* ************************************************** */
    /* Private Member Variables. */
    /* ************************************************** */

    // Holds the actual integer enumeration value
    private final int myState;

    /* ************************************************** */
    /* Public Functions. */
    /* ************************************************** */

    /**
     * Private constructor sets the state, this class is not constructed,
     * instead use one of the precreated instances declared above.
     * 
     * @param state
     */
    private MamaSubscriptionState(int state)
    {
        myState = state;
    }

    /* ************************************************** */
    /* Public Functions. */
    /* ************************************************** */

    /**
     * Returns the integer representation of the subscription state.
     * 
     * @return The state as an integer.
     */
    public int getValue()
    {
        return myState;
    }
   
    /**
     * This function converts the integer state value to a corresponding
     * MamaSubscriptionState object.
     *
     * @param state The state.
     * @return To return the MamaSusbscriptionState instance.
     */
    public static MamaSubscriptionState getSubscriptionState(int state)
    {
        // Returns
        MamaSubscriptionState ret = MAMA_SUBSCRIPTION_UNKNOWN;

        // Return the state object that corresponding to the integer value
        switch(state)
        {
            case 1:
            {
                ret = MAMA_SUBSCRIPTION_ALLOCATED;
                break;
            }
            case 2:
            {
                ret = MAMA_SUBSCRIPTION_SETUP;
                break;
            }
            case 3:
            {
                ret = MAMA_SUBSCRIPTION_ACTIVATING;
                break;
            }
            case 4:
            {
                ret = MAMA_SUBSCRIPTION_ACTIVATED;
                break;
            }
            case 5:
            {
                ret = MAMA_SUBSCRIPTION_DEACTIVATING;
                break;
            }
            case 6:
            {
                ret = MAMA_SUBSCRIPTION_DEACTIVATED;
                break;
            }
            case 7:
            {
                ret = MAMA_SUBSCRIPTION_DESTROYING;
                break;
            }
            case 8:
            {
                ret = MAMA_SUBSCRIPTION_DESTROYED;
                break;
            }

            case 9:
            {
                ret = MAMA_SUBSCRIPTION_DEALLOCATING;
                break;
            }

            case 10:
            {
                ret = MAMA_SUBSCRIPTION_DEALLOCATED;
                break;
            }

            case 11:
            {
                ret = MAMA_SUBSCRIPTION_REACTIVATING;
                break;
            }
        }

        return ret;
    }

    /**
     * This function will return the subscription state corresponding to
     * the supplied string. Note that if the string is not recognised then
     * MAMA_SUBSCRIPTION_UNKNOWN will be returned.
     *
     * @param state
     * @return The corresponding state object or MAMA_SUBSCRIPTION_UNKNOWN if
     *         the string isn't recognised.
     */
    public static MamaSubscriptionState parseSubscriptionState(String state)
    {
        // Returns
        MamaSubscriptionState ret = MAMA_SUBSCRIPTION_UNKNOWN;

        if (state.equalsIgnoreCase("MAMA_SUBSCRIPTION_ALLOCATED"))
        {
            ret = MAMA_SUBSCRIPTION_ALLOCATED;
        }
        else if (state.equalsIgnoreCase("MAMA_SUBSCRIPTION_SETUP"))
        {
            ret = MAMA_SUBSCRIPTION_SETUP;
        }
        else if (state.equalsIgnoreCase("MAMA_SUBSCRIPTION_ACTIVATING"))
        {
            ret = MAMA_SUBSCRIPTION_ACTIVATING;
        }
        else if (state.equalsIgnoreCase("MAMA_SUBSCRIPTION_ACTIVATED"))
        {
            ret = MAMA_SUBSCRIPTION_ACTIVATED;
        }
        else if (state.equalsIgnoreCase("MAMA_SUBSCRIPTION_DEACTIVATING"))
        {
            ret = MAMA_SUBSCRIPTION_DEACTIVATING;
        }
        else if (state.equalsIgnoreCase("MAMA_SUBSCRIPTION_DEACTIVATED"))
        {
            ret = MAMA_SUBSCRIPTION_DEACTIVATED;
        }
        else if (state.equalsIgnoreCase("MAMA_SUBSCRIPTION_DESTROYING"))
        {
            ret = MAMA_SUBSCRIPTION_DESTROYING;
        }
        else if (state.equalsIgnoreCase("MAMA_SUBSCRIPTION_DESTROYED"))
        {
            ret = MAMA_SUBSCRIPTION_DESTROYED;
        }
        else if (state.equalsIgnoreCase("MAMA_SUBSCRIPTION_DEALLOCATING"))
        {
            ret = MAMA_SUBSCRIPTION_DEALLOCATING;
        }
        else if (state.equalsIgnoreCase("MAMA_SUBSCRIPTION_DEALLOCATED"))
        {
            ret = MAMA_SUBSCRIPTION_DEALLOCATED;
        }
        else if (state.equalsIgnoreCase("MAMA_SUBSCRIPTION_REACTIVATING"))
        {
            ret = MAMA_SUBSCRIPTION_REACTIVATING;
        }

        return ret;
    }

    /**
     * This function returns the string representation of the state.
     *
     * @param state The state.
     * @return The string representation.
     */
    public static String toString(int state)
    {
        // Returns
        String ret = "MAMA_SUBSCRIPTION_UNKNOWN";
        
        // Return the state object that corresponding to the integer value
        switch(state)
        {
            case 1:
            {
                ret = "MAMA_SUBSCRIPTION_ALLOCATED";
                break;
            }
            case 2:
            {
                ret = "MAMA_SUBSCRIPTION_SETUP";
                break;
            }
            case 3:
            {
                ret = "MAMA_SUBSCRIPTION_ACTIVATING";
                break;
            }
            case 4:
            {
                ret = "MAMA_SUBSCRIPTION_ACTIVATED";
                break;
            }
            case 5:
            {
                ret = "MAMA_SUBSCRIPTION_DEACTIVATING";
                break;
            }
            case 6:
            {
                ret = "MAMA_SUBSCRIPTION_DEACTIVATED";
                break;
            }
            case 7:
            {
                ret = "MAMA_SUBSCRIPTION_DESTROYING";
                break;
            }
            case 8:
            {
                ret = "MAMA_SUBSCRIPTION_DESTROYED";
                break;
            }

            case 9:
            {
                ret = "MAMA_SUBSCRIPTION_DEALLOCATING";
                break;
            }

            case 10:
            {
                ret = "MAMA_SUBSCRIPTION_DEALLOCATED";
                break;
            }

            case 11:
            {
                ret = "MAMA_SUBSCRIPTION_REACTIVATING";
                break;
            }
        }

        return ret;
    }

    /**
     * This function returns the string representation of the state.
     *
     * @param state The state.
     * @return The string representation.
     */
    public static String toString(MamaSubscriptionState state)
    {
        return toString(state.myState);
    }
	
	/**
     * This function returns the string representation of the member state.
     *     
     * @return The string representation.
     */
    public String toString()
    {
        return toString(myState);
    }
}
