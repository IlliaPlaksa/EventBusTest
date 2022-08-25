#include <memory>
#include <thread>
#include "FirstService.h"
#include "SecondService.h"
#include "EventppBusImpl.h"
#include "EventBusImpl.h"

void TestEventppBusImpl();
void TestEventBusImpl();

int main(int argc, char** argv)
{
    TestEventppBusImpl();
    // TestEventBusImpl();
    return 0;
}

void TestEventppBusImpl()
{
    auto event_bus = std::make_shared<EventppBusImpl>();

    auto first_service = std::make_shared<FirstService>(event_bus);
    auto second_service = std::make_shared<SecondService>(event_bus);

    event_bus->Subscribe(first_service);
    event_bus->Subscribe(second_service);

    event_bus->Unsubscribe(first_service);

    event_bus->Run();

    first_service->Run();
    second_service->Run();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    first_service->Stop();
    second_service->Stop();
    event_bus->Stop();
}
void TestEventBusImpl()
{
    auto event_bus = std::make_shared<EventBusImpl>();

    auto first_service = std::make_shared<FirstService>(event_bus);
    auto second_service = std::make_shared<SecondService>(event_bus);

    event_bus->Subscribe(first_service);
    event_bus->Subscribe(second_service);

    event_bus->Unsubscribe(first_service);

    event_bus->Run();

    first_service->Run();
    second_service->Run();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    first_service->Stop();
    second_service->Stop();
    event_bus->Stop();
}
