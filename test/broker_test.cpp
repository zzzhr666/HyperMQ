#include <cassert>

#include "broker/broker.hpp"

int main() {
    hyper::Broker broker;

    assert(!broker.create_topic(""));
    assert(broker.topic("events") == nullptr);

    assert(broker.create_topic("events", 3));
    assert(!broker.create_topic("events", 5));

    auto* topic = broker.topic("events");
    assert(topic != nullptr);
    assert(topic->name() == "events");
    assert(topic->partition_count() == 3);

    assert(broker.topic("missing") == nullptr);

    const hyper::Broker& const_broker = broker;
    const auto* const_topic = const_broker.topic("events");
    assert(const_topic != nullptr);
    assert(const_topic->partition_count() == 3);
    assert(const_broker.topic("missing") == nullptr);

    assert(broker.create_topic("default-events"));
    assert(broker.topic("default-events")->partition_count()
           == hyper::defaultPartitionSize);

    return 0;
}
