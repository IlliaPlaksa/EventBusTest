//
// Created by Illia Plaksa on 25.08.2022.
//

#ifndef TEST_SRC_EVENTBUSIMPL_H_
#define TEST_SRC_EVENTBUSIMPL_H_

#include "EventBus.h"
#include <dexode/EventBus.hpp>
#include <thread>

class EventBusImpl : public EventBus
{
public:
    EventBusImpl();

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
    std::shared_ptr<dexode::EventBus> queue_;
    std::map<std::shared_ptr<Subscriber>, std::unique_ptr<dexode::EventBus::Listener>> subscribers_;
    std::thread worker_;
};

#endif //TEST_SRC_EVENTBUSIMPL_H_
