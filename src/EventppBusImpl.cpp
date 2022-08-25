//
// Created by Illia Plaksa on 23.08.2022.
//

#include "EventppBusImpl.h"

static const std::string message_type = "event";

EventppBusImpl::EventppBusImpl()
    :
    queue_(std::move(std::make_unique<EventBusType>()))
{
}
void EventppBusImpl::Subscribe(const std::shared_ptr<Subscriber>& subscriber)
{
    if (auto iter = subscribers_.find(subscriber); iter == subscribers_.end())
    {
        subscribers_[subscriber] = queue_->appendListener(
            message_type,
            [&](const Event& event)
            {
                subscriber->OnEvent(event);
            }
        );
    }
}
void EventppBusImpl::Unsubscribe(const std::shared_ptr<Subscriber>& subscriber)
{
    if (auto iter = subscribers_.find(subscriber); iter != subscribers_.end())
    {
        auto handle = iter->second;
        queue_->removeListener(message_type, handle);
        subscribers_.erase(iter);
    }
    
}
void EventppBusImpl::Publish(const Event& event)
{
    queue_->enqueue(message_type, event);
}
void EventppBusImpl::thread_fun()
{
    for(;;){
        queue_->wait();
        queue_->processOne();
    }
}
void EventppBusImpl::Run()
{
    worker_ = std::thread{[&](){ thread_fun(); }};
}
void EventppBusImpl::Stop()
{
    worker_.detach();
}
