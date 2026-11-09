#include <cassert>

#include "topic/topic.hpp"

int main() {
    hyper::Topic topic{"player-events", 3};

    assert(topic.name() == "player-events");
    assert(topic.partition_count() == 3);

    for (hyper::PartitionId id = 0; id < 3; ++id) {
        const auto* partition = topic.partition(id);
        assert(partition != nullptr);
        assert(partition->id() == id);
        assert(partition->next_offset() == 0);
    }

    assert(topic.partition(3) == nullptr);

    const hyper::Topic& const_topic = topic;
    const auto* const_partition = const_topic.partition(1);
    assert(const_partition != nullptr);
    assert(const_partition->id() == 1);
    assert(const_topic.partition(3) == nullptr);

    hyper::Topic default_topic{"default-partitions", 0};
    assert(default_topic.partition_count() == hyper::defaultPartitionSize);
    assert(default_topic.partition(default_topic.partition_count() - 1) != nullptr);
    assert(default_topic.partition(default_topic.partition_count()) == nullptr);

    hyper::Topic publish_topic{"publish-test", 3};

    const auto first_keyed = publish_topic.publish("player-1", "event-1");
    const auto second_keyed = publish_topic.publish("player-1", "event-2");
    assert(first_keyed.first == second_keyed.first);
    assert(first_keyed.second == 0);
    assert(second_keyed.second == 1);

    const auto first_expected_offset =
        publish_topic.partition(0)->next_offset();
    const auto first_round_robin = publish_topic.publish("", "event-3");

    const auto second_expected_offset =
        publish_topic.partition(1)->next_offset();
    const auto second_round_robin = publish_topic.publish("", "event-4");

    const auto third_expected_offset =
        publish_topic.partition(2)->next_offset();
    const auto third_round_robin = publish_topic.publish("", "event-5");

    assert(first_round_robin.first == 0);
    assert(second_round_robin.first == 1);
    assert(third_round_robin.first == 2);
    assert(first_round_robin.second == first_expected_offset);
    assert(second_round_robin.second == second_expected_offset);
    assert(third_round_robin.second == third_expected_offset);

    return 0;
}
