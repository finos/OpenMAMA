package com.wombat.mama;

/* $Id$ */

/**
 * @see MamaBasicSubscription
 * @author ldelaney
 */
public interface MamaBasicWildCardSubscriptionCallback
{
    /**
     * Method invoked when subscription creation is complete.
     * Since subscriptions are created asynchronous by throttle, this callback
     * provides the subscription instance after the throttle processes the
     * creation request.
     * In the case where a subscription is created on a queue other than the default
     * it is possible for <code>onMsg</code> calls to be processed to be called before
     * the <code>onCreate</code> callback is processed.
     *
     * @param subscription The subscription.
     */
    void onCreate (MamaBasicSubscription subscription);

    /**
     * Invoked if an error occurs during prior to subscription creation or if the
     * subscription receives a message for an unentitled subject.
     * <p>
     * If the status
     * is <code>MamaMsgStatus.NOT_ENTITTLED</code> the subject parameter is the
     * specific unentitled subject. If the subscription subject contains
     * wildcards, the subscription may still receive messages for other
     * entitled subjects.
     *
     * @param subscription The subscription.
     * @param wombatStatus The wombat error code.
     * @param platformError Third party, platform specific messaging error.
     * @param subject The subject for NOT_ENTITLED
     */
    void onError(MamaBasicSubscription subscription,
                 short wombatStatus,
                 int platformError,
                 String subject);

    /**
     * Invoked when a message arrives.
     *
     * @param subscription the <code>MamaBasicSubscription</code>.
     * @param msg The MamaMsg.
     */
    void onMsg (MamaBasicSubscription subscription, MamaMsg msg, String topic);

    void onDestroy(MamaBasicSubscription subscription);
}
