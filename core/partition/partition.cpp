#include "partition.hpp"

#include <algorithm>
#include <utility>


hyper::Partition::Partition(const PartitionId id) : id_(id) {}

hyper::Offset hyper::Partition::append(std::string key, std::string payload) {
    const auto offset = messages_.size();
    messages_.emplace_back(offset, std::move(key), std::move(payload));
    return offset;
}

std::vector<hyper::Message> hyper::Partition::read_from(const Offset offset, const std::size_t limit) const {
    if (offset >= messages_.size() || limit == 0) {
        return {};
    }

    const auto start = offset;
    const auto count = std::min(messages_.size() - start, limit);

    return {
        messages_.begin() + static_cast<std::ptrdiff_t>(start),
        messages_.begin() + static_cast<std::ptrdiff_t>(start + count)
    };
}
