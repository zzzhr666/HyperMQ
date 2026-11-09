#include "topic.hpp"

#include <functional>
#include <utility>

namespace {
    hyper::PartitionId select_partition(const std::string_view key, const std::size_t partition_count) {
        return static_cast<hyper::PartitionId>(
            std::hash<std::string_view>{}(key) % partition_count);
    }
}


hyper::Topic::Topic(std::string name, std::size_t partition_count)
    : name_(std::move(name)) {
    if (partition_count == 0) {
        partition_count = defaultPartitionSize;
    }

    partitions_.reserve(partition_count);
    for (std::size_t i = 0; i < partition_count; ++i) {
        partitions_.emplace_back(i);
    }
}

std::pair<hyper::PartitionId, hyper::Offset> hyper::Topic::publish(std::string key, std::string payload) {
    PartitionId partition_id{};
    if (!key.empty()) {
        partition_id = select_partition(key, partitions_.size());
    } else {
        partition_id = next_partition_id_;
        next_partition_id_ = (next_partition_id_ + 1) % partitions_.size();
    }

    const auto offset = partitions_[partition_id].append(
        std::move(key),
        std::move(payload));
    return {partition_id, offset};
}
