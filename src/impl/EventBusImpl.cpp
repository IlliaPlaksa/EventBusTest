//
// Created by Illia Plaksa on 23.08.2022.
//

#include "EventBusImpl.h"

static const std::string message_type = "event";

EventBusImpl::EventBusImpl()
    :
    queue_(std::move(std::make_shared<dexode::EventBus>()))
{
}
void EventBusImpl::Subscribe(const std::shared_ptr<Subscriber>& subscriber)
{
    if (auto iter = subscribers_.find(subscriber); iter == subscribers_.end())
    {
        auto listener = std::make_unique<dexode::EventBus::Listener>(queue_);
        listener->listen(
            [&subscriber](const Event& event) {
                subscriber->OnEvent(event);
            }
        );
        subscribers_.insert( {subscriber, std::move(listener)} );
    }
}
void EventBusImpl::Unsubscribe(const std::shared_ptr<Subscriber>& subscriber)
{
    if (auto iter = subscribers_.find(subscriber); iter != subscribers_.end())
    {
        (iter->second)->unlisten<Event>();
        subscribers_.erase(iter);
    }
}
void EventBusImpl::Publish(const Event& event)
{
    queue_->postpone(event);
}
void EventBusImpl::thread_fun()
{
    for(;;){
        queue_->process();
    }
}
void EventBusImpl::Run()
{
    worker_ = std::thread{
        [&](){ 
            thread_fun();
        }
    };
}
void EventBusImpl::Stop()
{
    worker_.detach();
}
