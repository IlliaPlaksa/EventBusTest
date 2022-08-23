//
// Created by Illia Plaksa on 23.08.2022.
//

#ifndef TEST__EVENTBUS_H_
#define TEST__EVENTBUS_H_

#include <memory>
#include "Subscriber.h"

#include "events.pb.h"

class EventBus
{
public:
    virtual void Subscribe(const std::shared_ptr<Subscriber>& subscriber) = 0;
    virtual void Unsubscribe(const std::shared_ptr<Subscriber>& subscriber) = 0;

    virtual void Publish(const Event& event) = 0;
};

#endif //TEST__EVENTBUS_H_
