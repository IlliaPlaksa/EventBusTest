//
// Created by Illia Plaksa on 26.08.2022.
//

#include <benchmark/benchmark.h>

#include <memory>
#include <eventpp/eventqueue.h>
#include <dexode/EventBus.hpp>
#include "events.pb.h"

const static std::string message_type = "event";
const static size_t listeners_count = 20;
const static size_t events_count = 1000;

std::vector<Event> CreateEvents()
{
    auto events = std::vector<Event>{events_count};
    
    for (auto& event : events)
    {
        StringEvent str_event{};
        str_event.set_value("value");
        
        event.mutable_payload()->PackFrom(str_event);
    }
    return events;
}

void OnEventFunc(const Event& event)
{
}

void PropagateEventsToEventQueue(std::shared_ptr<eventpp::EventQueue<std::string, void(const Event&)>>& queue,
                                 const std::vector<Event>& events)
{
    for (const auto& event : events) {
        queue->enqueue(message_type, event);
    }
}
void PropagateEventsToEventBus(std::shared_ptr<dexode::EventBus>& bus, const std::vector<Event>& events)
{
    for (const auto& event : events) {
        bus->postpone(event);
    }
}

std::vector<std::unique_ptr<dexode::EventBus::Listener>> 
RegisterEventBusListeners(std::shared_ptr<dexode::EventBus>& bus)
{
    auto listeners = std::vector<std::unique_ptr<dexode::EventBus::Listener>>{};
    for (size_t i = 0; i < listeners_count; ++i)
    {
        auto listener = std::make_unique<dexode::EventBus::Listener>(bus);
        listener->listen([&](const Event& event) { OnEventFunc(event); });

        listeners.emplace_back(std::move(listener));
    }
    return listeners;
}

std::vector<eventpp::EventQueue<std::string, void(const Event&)>::Handle>
RegisterEventQueueListeners(std::shared_ptr<eventpp::EventQueue<std::string, void(const Event&)>>& queue)
{
    std::vector<eventpp::EventQueue<std::string, void(const Event&)>::Handle> handles{};
    for (size_t i = 0; i < listeners_count; ++i)
    {
        auto handle = queue->appendListener(
            message_type,
            [&](const Event& event) { OnEventFunc(event); }
        );
        
        handles.emplace_back(handle);
    }
    return handles;
}

static void BM_EventppEventQueue(benchmark::State& state) {
    auto events = CreateEvents();
    auto queue = std::make_shared<eventpp::EventQueue<std::string, void(const Event&)>>();

    auto handles = RegisterEventQueueListeners(queue);
    
    for (auto _ : state) {
        PropagateEventsToEventQueue(queue, events);
        queue->process();
    }
}
// Register the function as a benchmark
BENCHMARK(BM_EventppEventQueue);

// Define another benchmark
static void BM_DexodeEventBus(benchmark::State& state) {
    auto events = CreateEvents();
    auto bus = std::make_shared<dexode::EventBus>();

    auto listeners = RegisterEventBusListeners(bus);

    for (auto _ : state) {
        PropagateEventsToEventBus(bus, events);
        bus->process();
    }
}
BENCHMARK(BM_DexodeEventBus);

BENCHMARK_MAIN();