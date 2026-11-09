#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "partition/partition.hpp"

namespace hyper {
    class Broker;

    struct TopicPartition {
        std::string topic;
        PartitionId partition_id{};

        bool operator==(const TopicPartition&) const = default;
    };

    struct TopicPartitionHash {
        std::size_t operator()(const TopicPartition& value) const noexcept {
            const auto topic_hash = std::hash<std::string>{}(value.topic);
            const auto partition_hash = std::hash<PartitionId>{}(value.partition_id);
            return topic_hash ^ partition_hash + 0x9e3779b9 + (topic_hash << 6) + (topic_hash >> 2);
        }
    };

    class ConsumerGroup {
    public:
        ConsumerGroup(const Broker& broker, std::string group_id);

        [[nodiscard]] std::string_view group_id() const noexcept {
            return group_id_;
        }

        bool join(std::string consumer_id);

        bool leave(std::string_view consumer_id);

        [[nodiscard]] std::vector<PartitionId> assignments_for(std::string_view consumer_id, std::string_view topic) const;

        [[nodiscard]] std::optional<Offset> committed_offset(std::string_view topic, PartitionId partition_id) const noexcept;

        bool commit(std::string_view consumer_id, std::string_view topic, PartitionId partition_id, Offset next_offset);

    private:
        const Broker* broker_;
        std::string group_id_;
        std::vector<std::string> members_;
        std::unordered_map<TopicPartition, Offset, TopicPartitionHash> committed_offsets_;
    };
} // namespace hyper
