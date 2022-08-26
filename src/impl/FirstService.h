//
// Created by Illia Plaksa on 19.08.2022.
//

#ifndef TEST__FIRSTSERVICE_H_
#define TEST__FIRSTSERVICE_H_

#include "events.pb.h"
#include <eventpp/eventqueue.h>
#include <memory>
#include "Subscriber.h"
#include "EventBus.h"

class FirstService : public Subscriber 
{
public:
    explicit FirstService(const std::shared_ptr<EventBus>& event_queue);
    ~FirstService();
public:
    void OnEvent(const Event& event) override;
    void thread_fun();

    void Run();
    void Stop();
private:
    std::shared_ptr<EventBus> event_bus_;
    std::unique_ptr<std::thread> thread_;
};

#endif //TEST__FIRSTSERVICE_H_
