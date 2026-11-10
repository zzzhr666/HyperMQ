#include <gtest/gtest.h>

#include "broker/broker.hpp"
#include "consumer/consumer.hpp"
#include "group/consumer_group.hpp"

TEST(ConsumerTest, PollAdvancesLocalPositionWithoutCommitting) {
    hyper::Broker broker;
    ASSERT_TRUE(broker.create_topic("events", 2));

    auto* topic = broker.topic("events");
    ASSERT_NE(topic, nullptr);
    auto* partition_zero = topic->partition(0);
    auto* partition_one = topic->partition(1);
    ASSERT_NE(partition_zero, nullptr);
    ASSERT_NE(partition_one, nullptr);

    partition_zero->append("key-0", "event-0");
    partition_zero->append("key-0", "event-1");
    partition_one->append("key-1", "event-2");

    hyper::ConsumerGroup group{broker, "workers"};
    ASSERT_TRUE(group.join("worker-a"));

    hyper::Consumer consumer{broker, group, "worker-a"};
    const auto records = consumer.poll("events", 2);

    ASSERT_EQ(records.size(), 2);
    EXPECT_EQ(records[0].partition_id, 0);
    EXPECT_EQ(records[0].message.offset, 0);
    EXPECT_EQ(records[1].partition_id, 0);
    EXPECT_EQ(records[1].message.offset, 1);

    EXPECT_FALSE(group.committed_offset("events", 0).has_value());
    EXPECT_FALSE(group.committed_offset("events", 1).has_value());
}

TEST(ConsumerTest, CommitPersistsPositionAndNewConsumerResumesFromIt) {
    hyper::Broker broker;
    ASSERT_TRUE(broker.create_topic("events", 2));

    auto* topic = broker.topic("events");
    ASSERT_NE(topic, nullptr);
    auto* partition_zero = topic->partition(0);
    ASSERT_NE(partition_zero, nullptr);
    partition_zero->append("key", "event-0");
    partition_zero->append("key", "event-1");

    hyper::ConsumerGroup group{broker, "workers"};
    ASSERT_TRUE(group.join("worker-a"));

    hyper::Consumer consumer{broker, group, "worker-a"};
    ASSERT_EQ(consumer.poll("events", 1).size(), 1);
    EXPECT_TRUE(consumer.commit("events"));

    ASSERT_TRUE(group.committed_offset("events", 0).has_value());
    EXPECT_EQ(group.committed_offset("events", 0).value(), 1);

    hyper::Consumer resumed_consumer{broker, group, "worker-a"};
    const auto records = resumed_consumer.poll("events", 10);

    ASSERT_EQ(records.size(), 1);
    EXPECT_EQ(records[0].partition_id, 0);
    EXPECT_EQ(records[0].message.offset, 1);
}

TEST(ConsumerTest, RejectsInvalidCommitTopic) {
    hyper::Broker broker;
    ASSERT_TRUE(broker.create_topic("events", 1));

    hyper::ConsumerGroup group{broker, "workers"};
    ASSERT_TRUE(group.join("worker-a"));

    hyper::Consumer consumer{broker, group, "worker-a"};

    EXPECT_FALSE(consumer.commit(""));
    EXPECT_FALSE(consumer.commit("missing"));
    EXPECT_TRUE(consumer.poll("events", 10).empty());
}
