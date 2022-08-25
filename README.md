# EventBusTest
This application was created to test and compare [`eventpp`](https://github.com/wqking/eventpp), [`EventBus`](https://github.com/gelldur/EventBus) 
libraries as core of Event test system.

System makes a simulation of usage of EventBus

## Main instances

- `EventBus` - an interface of even bus used by services for communication
- `EventBusImpl` - implementation of `EventBus` based on `eventpp::EventQueue`
- `EventppBusImpl` - implementation of `EventBus` based on `dexode::EventBus`
- `Subscriber`- interface created in order to make it possible service to subscribe to `EventBus`
- `FirstService`, `SecondService` - simple services implment logic of `Subscriber` and use `EventBus` to Publish their `Event`
- `Event` - main DTO instance in system, based on `protobuf` message

# Libraries comparison
### `eventpp::EventQueue`
**Source code: https://github.com/wqking/eventpp**

**C++ version:** `C++11`

**Last contribution: `31.07.2022`**

### Main features
To make it possible to filter event by their "type" we can only use first template parameter of `eventpp::EventQueue`
Like so:
```c++
auto queue = eventpp::EventQueue</*event type filter*/ std::string, // enums can be used as well
                                 /*signature of handler function*/ void (const Event&)>{};
```
### Example usage:
```c++
eventpp::EventQueue<std::string, void (const Event&)> queue;

queue.appendListener("ButtonClicked", [&](const Event& event) {
    button_subscriber->onEvent(event);
});
queue.appendListener("SessionLoggedOut", [&](const Event& event) {
    session_subscriber->onEvent(event);
});

// The listeners are not triggered during enqueue.
queue.enqueue("ButtonClicked", button_clicked);
queue.enqueue("SessionLoggedOut", session_logged_out);

// Process the event queue, dispatch all queued events.
queue.process();
```
### Note


### `dexode::EventBus`
**Source code: https://github.com/wqking/eventpp**

**C++ version:** `C++17`

**Last contribution: `03.08.2021`**

###Main features
Allows to subscribe on messages with specified C++ type eg.
```c++
namespace event {
    struct ButtonClicked;
    struct SessionLoggedOut;
}

// ... 

auto bus = std::make_shared<dexode::EventBus>{};

auto subscriber = std::make_shared<Subscriber>{bus};

auto listener = std::make_unique<dexode::EventBus::Listener>{bus};

listener->listen<event::ButtonClicked>(std::bind(&Subscriber::onButtonClicked, subscriber, std::placeholders::_1));

listener->listen<event::SessionLoggedOut>(std::bind(&Subscriber::onSessionLoggedOut, subscriber, std::placeholders::_1));

bus->pospone(button_clicked);
```
Moreover listener can handle events using lambdas as well
```c++
listener->listen(
    [](const Event& event)
    { 
        std::cout << "Processing event: " << event.DebugString() << std::endl;
    });
```

# Summary
Both libraries make the same deal using possible features of their C++ versions. 

As `dexode::Eventbus` uses the `C++17` allowing us to use filtering of events by type specifying, 
the using of the library appears much easier. 
Regardless this nice feature, library wasn't updated since `Aug 2021` so it could be a problem if some issues will reveal in a future.

But usage of `eventpp` without native filtering with types can be simplified
with moving of filtering responsibility on subscribers, 
which can filter incoming events with facilities of `protobuf.Any` as a payload of Event message. 

Furthermore `eventpp::EventQueue` has methods based on conditional variable, 
which allow us to wait until new events will be enqueued to reduce in vain processor usage.

In cases like this:

```c++
auto bus = std::make_shared<evetnpp::EventQueue<...>>{};

for (;;) {
    bus->wait();
    bus->processOne();
}
```
