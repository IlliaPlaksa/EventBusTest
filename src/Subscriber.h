//
// Created by Illia Plaksa on 19.08.2022.
//

#ifndef TEST__SUBSCRIBER_H_
#define TEST__SUBSCRIBER_H_

#include "events.pb.h"

class Subscriber
{
public:
    virtual void OnEvent(const Event& event) = 0; 
};

#endif //TEST__SUBSCRIBER_H_