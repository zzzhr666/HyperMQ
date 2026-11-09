#include "broker/broker.hpp"

namespace hyper {

bool Broker::create_topic(
    const std::string& name,
    const std::size_t partition_count) {
    if (name.empty()) {
        return false;
    }

    return topics_.try_emplace(name, name, partition_count).second;
}

Topic* Broker::topic(const std::string_view name) noexcept {
    const auto it = topics_.find(std::string{name});
    if (it == topics_.end()) {
        return nullptr;
    }
    return &it->second;
}

const Topic* Broker::topic(const std::string_view name) const noexcept {
    const auto it = topics_.find(std::string{name});
    if (it == topics_.end()) {
        return nullptr;
    }
    return &it->second;
}

} // namespace hyper
