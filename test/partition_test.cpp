#include <gtest/gtest.h>

#include "partition/partition.hpp"

TEST(PartitionTest, AppendAssignsOffsetsAndReadRespectsRange) {
    hyper::Partition partition{7};

    EXPECT_EQ(partition.id(), 7);
    EXPECT_EQ(partition.next_offset(), 0);

    EXPECT_EQ(partition.append("player-1", "login"), 0);
    EXPECT_EQ(partition.append("player-2", "level-up"), 1);
    EXPECT_EQ(partition.append("player-1", "logout"), 2);
    EXPECT_EQ(partition.next_offset(), 3);

    const auto messages = partition.read_from(1, 2);
    ASSERT_EQ(messages.size(), 2);
    EXPECT_EQ(messages[0].offset, 1);
    EXPECT_EQ(messages[0].key, "player-2");
    EXPECT_EQ(messages[0].payload, "level-up");
    EXPECT_EQ(messages[1].offset, 2);
    EXPECT_EQ(messages[1].key, "player-1");
    EXPECT_EQ(messages[1].payload, "logout");

    const auto limited_messages = partition.read_from(0, 1);
    ASSERT_EQ(limited_messages.size(), 1);
    EXPECT_EQ(limited_messages[0].offset, 0);

    EXPECT_TRUE(partition.read_from(3, 10).empty());
    EXPECT_TRUE(partition.read_from(10, 10).empty());
    EXPECT_TRUE(partition.read_from(0, 0).empty());
}
