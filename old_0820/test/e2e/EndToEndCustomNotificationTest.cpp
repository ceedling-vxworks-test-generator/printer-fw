#include <gtest/gtest.h>

#include "printer_a.h"

#include "NotificationReceiver.hpp"

#include "PublishManager.hpp"
#include "ChangeNotifyManager.hpp"
#include "PeriodicNotifyManager.hpp"

#include "SubscriptionStore.hpp"
#include "SubscriberMailboxManager.hpp"
#include "CallbackSubscriptionRegistry.hpp"

#include "NotificationMessage.hpp"
#include "NotificationTarget.hpp"
#include "NotificationTrigger.hpp"
#include "NotificationTargetType.hpp"
#include "SubscriptionInfo.hpp"
#include "DeliveryMethod.hpp"
#include "PrinterAProductDefinition.hpp"

class TestCustomDeliveryHandler
    : public rim::ICustomDeliveryHandler
{
public:

    bool Deliver(
        const rim::SubscriptionInfo&,
        const rim::NotificationMessage&)
        override
    {
        ++count_;
        return true;
    }

    uint64_t count_{0};
};

TEST(
    EndToEndCustomNotificationTest,
    CustomDeliveryCalled)
{
    rim::SubscriptionStore
        subscriptionStore;

    rim::SubscriberMailboxManager
        mailboxManager;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    TestCustomDeliveryHandler
        customHandler;

    rim::CallbackQueue callbackQueue; 

    rim::ChangeNotifyManager
        notifyManager(
            subscriptionStore,
            mailboxManager,
            callbackRegistry,
            callbackQueue);

    notifyManager.SetCustomDeliveryHandler(
        &customHandler);

    rim::PeriodicNotifyManager
        periodicNotifyManager;
    
    rim::RouteProvider routeProvider;

    routeProvider.Initialize(
    rim::kPrinterAProductDefinition);

    rim::PublishManager
        publishManager(
            notifyManager,
            periodicNotifyManager,
            subscriptionStore,
            routeProvider);

    const auto subscriptionId =
        subscriptionStore
            .CreateSubscriptionId();

    rim::SubscriptionInfo info{};

    info.id =
        subscriptionId;

    info.target =
    {
        rim::NotificationTargetType::Capability,
        RI_CAPABILITY_ENVIRONMENT
    };

    info.method =
        rim::DeliveryMethod::Custom;

    info.trigger =
        rim::NotificationTrigger::OnChange;

    subscriptionStore.Register(
        info);
        
    EXPECT_EQ(
        customHandler.count_,
        0U);

    publishManager.Publish(
        info.target,
        rim::NotificationTrigger::OnChange);

    EXPECT_EQ(
        customHandler.count_,
        1U);

}