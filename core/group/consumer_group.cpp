#include "consumer_group.hpp"

#include <algorithm>
#include <utility>

#include "broker/broker.hpp"

hyper::ConsumerGroup::ConsumerGroup(const Broker& broker, std::string group_id) : broker_(&broker), group_id_(std::move(group_id)) {}

bool hyper::ConsumerGroup::join(std::string consumer_id) {
    if (consumer_id.empty()) {
        return false;
    }
    const auto it = std::ranges::lower_bound(members_, consumer_id);
    if (it != members_.end() && *it == consumer_id) {
        return false;
    }
    members_.insert(it, std::move(consumer_id));
    return true;
}

bool hyper::ConsumerGroup::leave(const std::string_view consumer_id) {
    if (consumer_id.empty()) {
        return false;
    }
    const auto it = std::ranges::lower_bound(members_, consumer_id);
    if (it == members_.end() || *it != consumer_id) {
        return false;
    }
    members_.erase(it);
    return true;
}

std::vector<hyper::PartitionId> hyper::ConsumerGroup::assignments_for(const std::string_view consumer_id, const std::string_view topic) const {
    if (members_.empty() || consumer_id.empty() || topic.empty()) {
        return {};
    }

    const auto t = broker_->topic(topic);
    if (t == nullptr || t->partition_count() == 0) {
        return {};
    }

    const auto it = std::ranges::lower_bound(members_, consumer_id);
    if (it == members_.end() || *it != consumer_id) {
        return {};
    }
    const auto member_index = static_cast<std::size_t>(
        it - members_.begin());
    auto ret = std::vector<PartitionId>{};
    for (std::size_t i = member_index; i < t->partition_count(); i += members_.size()) {
        ret.emplace_back(static_cast<PartitionId>(i));
    }
    return ret;
}

std::optional<hyper::Offset> hyper::ConsumerGroup::committed_offset(const std::string_view topic, const PartitionId partition_id) const noexcept {
    if (topic.empty()) {
        return std::nullopt;
    }
    const auto it = committed_offsets_.find(TopicPartition{
        .topic = std::string(topic),
        .partition_id = partition_id,
    });
    if (it == committed_offsets_.end()) {
        return std::nullopt;
    }
    return it->second;
}

bool hyper::ConsumerGroup::commit(const std::string_view consumer_id, const std::string_view topic, const PartitionId partition_id, const Offset next_offset) {
    if (consumer_id.empty() || topic.empty()) {
        return false;
    }
    auto partition_ids = assignments_for(consumer_id,topic);
    if (const auto it = std::ranges::lower_bound(partition_ids, partition_id); it == partition_ids.end() || *it != partition_id) {
        return false;
    }
    const auto t = broker_->topic(topic);
    if (t == nullptr) {
        return false;
    }
    const auto partition = t->partition(partition_id);
    if (partition == nullptr) {
        return false;
    }
    if (partition->next_offset() < next_offset) {
        return false;
    }

    const auto current_offset = committed_offset(topic, partition_id);
    if (current_offset.has_value() && next_offset < current_offset.value()) {
        return false;
    }

    committed_offsets_[{.topic = std::string(topic),.partition_id = partition_id}] = next_offset;
    return true;
}
