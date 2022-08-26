//
// Created by Illia Plaksa on 26.08.2022.
//

#include <benchmark/benchmark.h>

#include <memory>
#include <thread>
#include <mutex>
#include <eventpp/eventqueue.h>
#include <dexode/EventBus.hpp>
#include "events.pb.h"

const static std::string message_type = "event";
const static size_t listeners_count = 10;
const static size_t events_count = 10000;

static size_t event_counter = 0;
std::mutex mutex;
std::condition_variable cv;

std::vector<Event> CreateEvents()
{
    auto events = std::vector<Event>{events_count};
    
    for (size_t i = 0; i < events.size(); ++i)
    {
        StringEvent str_event{};
        std::string value = "value";
        str_event.set_value(value + std::to_string(i));
        
        events[i].mutable_payload()->PackFrom(str_event);
    }
    return events;
}

void OnEventFunc(const Event& event)
{
    event_counter++;
    auto string_event = StringEvent{};
    event.payload().UnpackTo(&string_event);
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

std::vector<std::unique_ptr<dexode::EventBus::Listener>> RegisterEventBusListeners(std::shared_ptr<dexode::EventBus>& bus)
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
void RegisterEventQueueListeners(std::shared_ptr<eventpp::EventQueue<std::string, void(const Event&)>>& queue)
{
    for (size_t i = 0; i < listeners_count; ++i)
    {
        queue->appendListener(
            message_type,
            [&](const Event& event) { OnEventFunc(event); }
        );
    }
}

static void BM_EventppEventQueue(benchmark::State& state) {
    auto events = CreateEvents();
    auto queue = std::make_shared<eventpp::EventQueue<std::string, void(const Event&)>>();

    RegisterEventQueueListeners(queue);

    std::atomic_bool stop = false;

    std::thread worker{
        [&queue, &stop]() {
            while(!stop) {
                queue->processOne();
                cv.notify_one();
            }
        }
    };

    for (auto _ : state) {
        event_counter = 0;
        
        PropagateEventsToEventQueue(queue, events);

        std::unique_lock lk(mutex);
        cv.wait(lk, [&]() { return event_counter == events_count * listeners_count; });
    }

    stop.store(true);
    worker.join();
}
// Register the function as a benchmark
BENCHMARK(BM_EventppEventQueue);

// Define another benchmark
static void BM_DexodeEventBus(benchmark::State& state) {
    auto events = CreateEvents();
    auto bus = std::make_shared<dexode::EventBus>();

    auto listeners = RegisterEventBusListeners(bus);

    std::atomic_bool stop = false;

    std::thread worker{
        [&bus, &stop]() {
            while (!stop) {
                bus->process();

                cv.notify_one();
            }
        }
    };

    for (auto _ : state) {
        event_counter = 0;

        PropagateEventsToEventBus(bus, events);

        std::unique_lock lk(mutex);
        cv.wait(lk, [&]() { return event_counter == events_count * listeners_count; });
    }

    stop.store(true);
    worker.join();
}
BENCHMARK(BM_DexodeEventBus);

BENCHMARK_MAIN();