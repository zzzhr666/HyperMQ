#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "partition/partition.hpp"

namespace hyper {

constexpr std::size_t defaultPartitionSize = 16;

class Topic {
public:
    explicit Topic(std::string name, std::size_t partition_count = defaultPartitionSize);

    [[nodiscard]] std::string_view name() const noexcept {
        return name_;
    }

    [[nodiscard]] std::size_t partition_count() const noexcept {
        return partitions_.size();
    }

    [[nodiscard]] Partition* partition(const PartitionId id) noexcept {
        if (id >= partitions_.size()) {
            return nullptr;
        }
        return &partitions_[id];
    }

    [[nodiscard]] const Partition* partition(const PartitionId id) const noexcept {
        if (id >= partitions_.size()) {
            return nullptr;
        }
        return &partitions_[id];
    }

    [[nodiscard]] std::pair<PartitionId, Offset> publish(
        std::string key,
        std::string payload);

private:
    PartitionId next_partition_id_{0};
    std::string name_;
    std::vector<Partition> partitions_;

};

} // namespace hyper
