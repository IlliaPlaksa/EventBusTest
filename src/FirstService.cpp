//
// Created by Illia Plaksa on 19.08.2022.
//

#include "FirstService.h"

#include <iostream>
#include <chrono>
#include <thread>

FirstService::FirstService(const std::shared_ptr<EventBus>& event_bus)
    :
    event_bus_(event_bus)
{
}
FirstService::~FirstService()
{
}

void FirstService::Run()
{
    thread_ = std::make_unique<std::thread>([&](){ thread_fun(); });
}

void FirstService::Stop() { thread_->detach(); }


void FirstService::OnEvent(const Event& event)
{
    std::cout << "FirstService::OnEvent" << std::endl;
    std::cout << "Processing event: " << event.DebugString() << std::endl;
}
void FirstService::thread_fun()
{
    for(size_t i = 0; i < 5; ++i) {
        StringEvent string_event;
        string_event.set_value("value" + std::to_string(i));
        Event event;
        event.mutable_payload()->PackFrom(string_event);
        
        event_bus_->Publish(event);
    }
}
