//
// Created by Illia Plaksa on 19.08.2022.
//

#ifndef TEST__SECONDSERVIE_H_
#define TEST__SECONDSERVIE_H_

#include "events.pb.h"
#include <eventpp/eventqueue.h>
#include <memory>
#include "Subscriber.h"
#include "EventBus.h"

class SecondService : public Subscriber
{
public:
    explicit SecondService(const std::shared_ptr<EventBus>& event_bus);
     ~SecondService();
public:
    void OnEvent(const Event& event);
    void thread_fun();

    void Run();
    void Stop();
    
private:
    std::shared_ptr<EventBus> event_bus_;
    std::unique_ptr<std::thread> thread_;
};

#endif //TEST__SECONDSERVIE_H_
