#include <gtest/gtest.h>

#include "broker/broker.hpp"

TEST(BrokerTest, CreatesAndFindsTopics) {
    hyper::Broker broker;

    EXPECT_FALSE(broker.create_topic(""));
    EXPECT_EQ(broker.topic("events"), nullptr);

    EXPECT_TRUE(broker.create_topic("events", 3));
    EXPECT_FALSE(broker.create_topic("events", 5));

    auto* topic = broker.topic("events");
    ASSERT_NE(topic, nullptr);
    EXPECT_EQ(topic->name(), "events");
    EXPECT_EQ(topic->partition_count(), 3);

    EXPECT_EQ(broker.topic("missing"), nullptr);

    const hyper::Broker& const_broker = broker;
    const auto* const_topic = const_broker.topic("events");
    ASSERT_NE(const_topic, nullptr);
    EXPECT_EQ(const_topic->partition_count(), 3);
    EXPECT_EQ(const_broker.topic("missing"), nullptr);
}

TEST(BrokerTest, UsesTopicDefaultPartitionCount) {
    hyper::Broker broker;

    ASSERT_TRUE(broker.create_topic("default-events"));

    const auto* topic = broker.topic("default-events");
    ASSERT_NE(topic, nullptr);
    EXPECT_EQ(topic->partition_count(), hyper::defaultPartitionSize);
}
