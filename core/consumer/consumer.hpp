#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "group/consumer_group.hpp"

namespace hyper {
    class Broker;
    class ConsumerGroup;


    struct ConsumerRecord {
        PartitionId partition_id{};
        Message message;
    };

    class Consumer {
    public:
        Consumer(Broker& broker, ConsumerGroup& group, std::string consumer_id);

        [[nodiscard]] std::vector<ConsumerRecord> poll(std::string_view topic, std::size_t max_records);

        [[nodiscard]]bool commit(std::string_view topic);

    private:
        Broker* broker_;
        ConsumerGroup* group_;
        std::string consumer_id_;
        std::unordered_map<TopicPartition, Offset, TopicPartitionHash> positions_;
    };
} // hyper
