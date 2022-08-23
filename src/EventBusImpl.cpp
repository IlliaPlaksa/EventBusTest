//
// Created by Illia Plaksa on 23.08.2022.
//

#include "EventBusImpl.h"

static const std::string message_type = "event";

EventBusImpl::EventBusImpl()
    :
    queue_(std::make_unique<EventBusType>())
{
}
void EventBusImpl::Subscribe(const std::shared_ptr<Subscriber>& subscriber)
{
    subscribers_[subscriber] = queue_->appendListener(
        message_type,
        [&](const Event& event) {
            subscriber->OnEvent(event); 
        }
    );
}
void EventBusImpl::Unsubscribe(const std::shared_ptr<Subscriber>& subscriber)
{
    auto handle = subscribers_[subscriber];
    queue_->removeListener(message_type, handle);
}
void EventBusImpl::Publish(const Event& event)
{
    queue_->enqueue(message_type, event);
}
void EventBusImpl::thread_fun()
{
    for(;;){
        queue_->wait();
        queue_->processOne();
    }
}
void EventBusImpl::Run()
{
    worker_ = std::thread{[&](){ thread_fun(); }};
}
void EventBusImpl::Stop()
{
    worker_.detach();
}
