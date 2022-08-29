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

# Benchmark testing
Sources: [`benchmarks.cpp`](benchmarks/benchmarks.cpp)

Compilation mode: `Release`

<b>Task:</b> dispatch a `single` event to `one` subscriber
```text
2022-08-28T14:37:43+03:00
Running ./Benchmark
Run on (4 X 2900 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB
  L1 Instruction 32 KiB
  L2 Unified 256 KiB (x2)
  L3 Unified 3072 KiB
Load Average: 5.48, 6.07, 7.18
---------------------------------------------------------------
Benchmark                     Time             CPU   Iterations
---------------------------------------------------------------
BM_EventppEventQueue        556 ns          552 ns      1170275
BM_DexodeEventBus          1197 ns         1194 ns       565647
```

<b>Task:</b> dispatch a `1000` events to `one` subscriber
```text
Running ./Benchmark
Run on (4 X 2900 MHz CPU s)
CPU Caches:
L1 Data 32 KiB
L1 Instruction 32 KiB
L2 Unified 256 KiB (x2)
L3 Unified 3072 KiB
Load Average: 6.86, 4.93, 5.47
---------------------------------------------------------------
Benchmark                     Time             CPU   Iterations
---------------------------------------------------------------
BM_EventppEventQueue     519188 ns       515688 ns         1257
BM_DexodeEventBus        968409 ns       964199 ns          677
```
Eventpp `EventQueue`: `~0.519 ms`

Dexode `EventBus`: `~0.968 ms`

<b>Task:</b> dispatch a `single` events between `100` subscriber
```text
2022-08-28T15:20:47+03:00
Running ./Benchmark
Run on (4 X 2900 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB
  L1 Instruction 32 KiB
  L2 Unified 256 KiB (x2)
  L3 Unified 3072 KiB
Load Average: 7.43, 8.81, 8.40
---------------------------------------------------------------
Benchmark                     Time             CPU   Iterations
---------------------------------------------------------------
BM_EventppEventQueue       4063 ns         4049 ns       171642
BM_DexodeEventBus          1344 ns         1340 ns       514188
```
Eventpp `EventQueue`: `~0.004 ms`

Dexode `EventBus`: `~0.0013 ms`

<b>Task:</b> dispatch a `1000` events between `100` subscriber
```text
2022-08-28T15:21:51+03:00
Running ./Benchmark
Run on (4 X 2900 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB
  L1 Instruction 32 KiB
  L2 Unified 256 KiB (x2)
  L3 Unified 3072 KiB
Load Average: 12.61, 9.62, 8.71
---------------------------------------------------------------
Benchmark                     Time             CPU   Iterations
---------------------------------------------------------------
BM_EventppEventQueue    5018500 ns      4897491 ns          116
BM_DexodeEventBus       1320416 ns      1312960 ns          524
```
Eventpp `EventQueue`: `~5.01 ms` 

Dexode `EventBus`: `~1.32 ms`

<b>Task:</b> dispatch a `1000` events between `20` subscriber

_<b>Note:</b> The most possible scenario in real usage_
```text
2022-08-28T16:51:25+03:00
Running ./Benchmark
Run on (4 X 2900 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB
  L1 Instruction 32 KiB
  L2 Unified 256 KiB (x2)
  L3 Unified 3072 KiB
Load Average: 9.78, 7.84, 6.18
---------------------------------------------------------------
Benchmark                     Time             CPU   Iterations
---------------------------------------------------------------
BM_EventppEventQueue    1220383 ns      1208945 ns          560
BM_DexodeEventBus       1047737 ns      1024182 ns          686
```

Eventpp `EventQueue`: `~1.22 ms`

Dexode `EventBus`: `~1.04 ms`

## Benchmark summary
<b>Benchmark scenrio</b>: sequential addition of events to the queue with their subsequent dispatching among subscribers

<b>Eventpp `EventQueue`</b>: Has an advantage in processing events between small number of subscribers(`1-20`).
However when the number of subscribers increases the time of processing increases proportionately.

<b>Dexode `EventBus`</b>: Has a great advantage in propagating events between subscribers (seems like subscribers' callbacks are execute in parallel).
But processing of events with low number of subscribers(`1-10`) is about twice longer then Eventpp's `EventQueue`.
