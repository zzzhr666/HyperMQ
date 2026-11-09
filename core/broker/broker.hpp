#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>

#include "topic/topic.hpp"

namespace hyper {

class Broker {
public:
    bool create_topic(
        const std::string& name,
        std::size_t partition_count = defaultPartitionSize);

    [[nodiscard]] Topic* topic(std::string_view name) noexcept;
    [[nodiscard]] const Topic* topic(std::string_view name) const noexcept;

private:
    std::unordered_map<std::string, Topic> topics_;
};

} // namespace hyper
