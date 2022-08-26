//
// Created by Illia Plaksa on 19.08.2022.
//

#include "SecondService.h"

#include <iostream>
#include <chrono>
#include <thread>

SecondService::SecondService(const std::shared_ptr<EventBus>& event_bus)
    :
    event_bus_(event_bus)
{
}
SecondService::~SecondService()
{
}

void SecondService::Run()
{
    thread_ = std::make_unique<std::thread>([&](){ thread_fun(); });
}

void SecondService::Stop() { thread_->detach(); }

void SecondService::OnEvent(const Event& event)
{
    std::cout << "SecondService::OnEvent" << std::endl;
    std::cout << "Processing event: " << event.DebugString() << std::endl;
}
void SecondService::thread_fun()
{
    for(size_t i = 0; i < 5; ++i) {
        IntEvent int_event;
        int_event.set_value(i + 1);
        Event event;
        event.mutable_payload()->PackFrom(int_event);

        event_bus_->Publish(event);
    }
}
