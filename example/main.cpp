#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "broker/broker.hpp"
#include "consumer/consumer.hpp"
#include "group/consumer_group.hpp"

namespace {

void print_records(const std::string_view consumer_name, const std::vector<hyper::ConsumerRecord>& records) {
    std::cout << consumer_name << " received " << records.size() << " record(s)\n";

    for (const auto& [partition_id, message] : records) {
        std::cout << "  partition=" << partition_id << ", offset=" << message.offset
                  << ", key=" << message.key << ", payload=" << message.payload << '\n';
    }
}

} // namespace

int main() {
    hyper::Broker broker;

    if (!broker.create_topic("player-events", 3)) {
        std::cerr << "failed to create topic\n";
        return 1;
    }

    auto* topic = broker.topic("player-events");
    if (topic == nullptr) {
        std::cerr << "failed to find topic\n";
        return 1;
    }

    const auto publish = [topic](std::string key, std::string payload) {
        const auto [partition_id, offset] = topic->publish(std::move(key), std::move(payload));

        std::cout << "published to partition=" << partition_id << ", offset=" << offset << '\n';
    };

    publish("player-1001", "login");
    publish("player-1001", "level-up");
    publish("player-1002", "login");
    publish("player-1002", "logout");

    hyper::ConsumerGroup group{broker, "analytics"};
    if (!group.join("analytics-worker-1")) {
        std::cerr << "failed to join consumer group\n";
        return 1;
    }

    hyper::Consumer consumer{broker, group, "analytics-worker-1"};

    const auto first_batch = consumer.poll("player-events", 2);
    print_records("analytics-worker-1 / first poll", first_batch);

    if (!consumer.commit("player-events")) {
        std::cerr << "failed to commit first batch\n";
        return 1;
    }

    hyper::Consumer resumed_consumer{broker, group, "analytics-worker-1"};

    const auto resumed_batch = resumed_consumer.poll("player-events", 10);
    print_records("analytics-worker-1 / resumed poll", resumed_batch);

    return 0;
}
