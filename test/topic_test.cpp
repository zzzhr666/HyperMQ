#include <gtest/gtest.h>

#include "topic/topic.hpp"

TEST(TopicTest, CreatesAndFindsPartitions) {
    hyper::Topic topic{"player-events", 3};

    EXPECT_EQ(topic.name(), "player-events");
    EXPECT_EQ(topic.partition_count(), 3);

    for (hyper::PartitionId id = 0; id < 3; ++id) {
        const auto* partition = topic.partition(id);
        ASSERT_NE(partition, nullptr);
        EXPECT_EQ(partition->id(), id);
        EXPECT_EQ(partition->next_offset(), 0);
    }

    EXPECT_EQ(topic.partition(3), nullptr);

    const hyper::Topic& const_topic = topic;
    const auto* const_partition = const_topic.partition(1);
    ASSERT_NE(const_partition, nullptr);
    EXPECT_EQ(const_partition->id(), 1);
    EXPECT_EQ(const_topic.partition(3), nullptr);
}

TEST(TopicTest, UsesDefaultPartitionCountWhenRequestedCountIsZero) {
    hyper::Topic topic{"default-partitions", 0};

    EXPECT_EQ(topic.partition_count(), hyper::defaultPartitionSize);
    EXPECT_NE(topic.partition(topic.partition_count() - 1), nullptr);
    EXPECT_EQ(topic.partition(topic.partition_count()), nullptr);
}

TEST(TopicTest, RoutesKeyedAndUnkeyedMessages) {
    hyper::Topic topic{"publish-test", 3};

    const auto first_keyed = topic.publish("player-1", "event-1");
    const auto second_keyed = topic.publish("player-1", "event-2");
    EXPECT_EQ(first_keyed.first, second_keyed.first);
    EXPECT_EQ(first_keyed.second, 0);
    EXPECT_EQ(second_keyed.second, 1);

    const auto first_expected_offset = topic.partition(0)->next_offset();
    const auto first_round_robin = topic.publish("", "event-3");

    const auto second_expected_offset = topic.partition(1)->next_offset();
    const auto second_round_robin = topic.publish("", "event-4");

    const auto third_expected_offset = topic.partition(2)->next_offset();
    const auto third_round_robin = topic.publish("", "event-5");

    EXPECT_EQ(first_round_robin.first, 0);
    EXPECT_EQ(second_round_robin.first, 1);
    EXPECT_EQ(third_round_robin.first, 2);
    EXPECT_EQ(first_round_robin.second, first_expected_offset);
    EXPECT_EQ(second_round_robin.second, second_expected_offset);
    EXPECT_EQ(third_round_robin.second, third_expected_offset);
}
