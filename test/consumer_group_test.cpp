#include <gtest/gtest.h>

#include "broker/broker.hpp"
#include "group/consumer_group.hpp"

TEST(ConsumerGroupTest, AssignsPartitionsToMembers) {
    hyper::Broker broker;
    ASSERT_TRUE(broker.create_topic("events", 4));

    hyper::ConsumerGroup group{broker, "workers"};
    EXPECT_EQ(group.group_id(), "workers");

    EXPECT_TRUE(group.join("worker-b"));
    EXPECT_TRUE(group.join("worker-a"));
    EXPECT_FALSE(group.join("worker-a"));
    EXPECT_FALSE(group.join(""));

    EXPECT_EQ(
        group.assignments_for("worker-a", "events"),
        (std::vector<hyper::PartitionId>{0, 2}));
    EXPECT_EQ(
        group.assignments_for("worker-b", "events"),
        (std::vector<hyper::PartitionId>{1, 3}));

    EXPECT_TRUE(group.assignments_for("unknown", "events").empty());
    EXPECT_TRUE(group.assignments_for("worker-a", "missing").empty());
    EXPECT_TRUE(group.assignments_for("worker-a", "").empty());
}

TEST(ConsumerGroupTest, CommitsOnlyValidAndMonotonicOffsets) {
    hyper::Broker broker;
    ASSERT_TRUE(broker.create_topic("events", 4));

    auto* topic = broker.topic("events");
    ASSERT_NE(topic, nullptr);
    auto* partition = topic->partition(0);
    ASSERT_NE(partition, nullptr);
    partition->append("key-1", "event-1");
    partition->append("key-2", "event-2");

    hyper::ConsumerGroup group{broker, "workers"};
    ASSERT_TRUE(group.join("worker-a"));
    ASSERT_TRUE(group.join("worker-b"));

    EXPECT_FALSE(group.committed_offset("events", 0).has_value());
    EXPECT_FALSE(group.commit("worker-b", "events", 0, 1));
    EXPECT_TRUE(group.commit("worker-a", "events", 0, 1));

    ASSERT_TRUE(group.committed_offset("events", 0).has_value());
    EXPECT_EQ(group.committed_offset("events", 0).value(), 1);

    EXPECT_TRUE(group.commit("worker-a", "events", 0, 1));
    EXPECT_FALSE(group.commit("worker-a", "events", 0, 0));
    EXPECT_FALSE(group.commit("worker-a", "events", 0, 3));
    EXPECT_EQ(group.committed_offset("events", 0).value(), 1);
}

TEST(ConsumerGroupTest, LeavingMemberLosesAssignmentButKeepsOffset) {
    hyper::Broker broker;
    ASSERT_TRUE(broker.create_topic("events", 4));

    auto* topic = broker.topic("events");
    ASSERT_NE(topic, nullptr);
    auto* partition = topic->partition(0);
    ASSERT_NE(partition, nullptr);
    partition->append("key", "event");

    hyper::ConsumerGroup group{broker, "workers"};
    ASSERT_TRUE(group.join("worker-a"));
    ASSERT_TRUE(group.join("worker-b"));
    ASSERT_TRUE(group.commit("worker-a", "events", 0, 1));

    EXPECT_TRUE(group.leave("worker-a"));
    EXPECT_FALSE(group.leave("worker-a"));
    EXPECT_TRUE(group.assignments_for("worker-a", "events").empty());
    EXPECT_FALSE(group.commit("worker-a", "events", 0, 1));
    ASSERT_TRUE(group.committed_offset("events", 0).has_value());
    EXPECT_EQ(group.committed_offset("events", 0).value(), 1);
}
