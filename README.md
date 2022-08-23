# EventBusTest
This application was created to test [`eventpp`](https://github.com/wqking/eventpp) library as core of EventBus test system

System makes a simulation of real usage of EventBus

## Main instances

- `EventBus` - an interface of even bus used by services for communication
- `EventBusImpl` - implementation of `EventBus` based on `eventpp::EventQueue`
- `Subscriber`- interface created in order to make it possible service to subscribe to `EventBus`
- `FirstService`, `SecondService` - simple services implment logic of `Subscriber` and use `EventBus` to Publish their `Event`
- `Event` - main DTO instance in system, based on `protobuf` message
