#include "consumer.hpp"

#include <algorithm>
#include <utility>

#include "broker/broker.hpp"


hyper::Consumer::Consumer(Broker& broker, ConsumerGroup& group, std::string consumer_id)
    : broker_(&broker), group_(&group), consumer_id_(std::move(consumer_id)) {}

std::vector<hyper::ConsumerRecord> hyper::Consumer::poll(const std::string_view topic, std::size_t max_records) {
    if (max_records == 0) {
        return {};
    }
    const auto t = broker_->topic(topic);
    if (t == nullptr) {
        return {};
    }
    std::vector<ConsumerRecord> records;
    const auto partitions = group_->assignments_for(consumer_id_, topic);
    auto remaining = max_records;

    for (const auto partition_id : partitions) {
        if (remaining == 0) {
            break;
        }

        const auto key = TopicPartition{
            .topic = std::string(topic),
            .partition_id = partition_id,
        };

        auto position_it = positions_.find(key);
        if (position_it == positions_.end()) {
            position_it = positions_.try_emplace(key, group_->committed_offset(topic, partition_id).value_or(0)).first;
        }

        const auto partition = t->partition(partition_id);
        if (partition == nullptr) {
            continue;
        }

        auto messages = partition->read_from(position_it->second, remaining);
        for (auto& message : messages) {
            records.emplace_back(partition_id, std::move(message));
        }

        position_it->second += messages.size();
        remaining -= messages.size();
    }

    return records;
}

bool hyper::Consumer::commit(std::string_view topic) {
    const auto t = broker_->topic(topic);
    if (t == nullptr) {
        return false;
    }
    auto pred = [this,topic](const auto& kv) -> bool {
        const auto&[topic_partition,position] = kv;
        const auto&[key_topic,partition_id] = topic_partition;
        if (key_topic != topic) {
            return true;
        }
        return group_->commit(consumer_id_,topic,partition_id,position);
    };
    return std::ranges::all_of(positions_,pred);
}
