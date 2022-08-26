//
// Created by Illia Plaksa on 23.08.2022.
//

#ifndef TEST__EVENTBUSIMPL_H_
#define TEST__EVENTBUSIMPL_H_

#include "EventBus.h"
#include <eventpp/eventqueue.h>
#include <thread>

class EventppBusImpl : public EventBus
{
public:
    EventppBusImpl();

public:
    void Subscribe(const std::shared_ptr<Subscriber>& subscriber) override;
    void Unsubscribe(const std::shared_ptr<Subscriber>& subscriber) override;
    
    void Publish(const Event& event) override;

public:
    void Run();
    void Stop();
    
private:
    void thread_fun();
    
private:
    using EventBusType = eventpp::EventQueue<std::string, void(const Event&)>;
    std::unique_ptr<EventBusType> queue_;
    std::map<std::shared_ptr<Subscriber>, EventBusType::Handle> subscribers_;
    
    std::thread worker_;
};

#endif //TEST__EVENTBUSIMPL_H_
