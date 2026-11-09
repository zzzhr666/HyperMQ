#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "message/message.hpp"

namespace hyper {

using PartitionId = std::uint32_t;

class Partition {
public:
    explicit Partition(PartitionId id);

    Offset append(std::string key, std::string payload);

    [[nodiscard]] std::vector<Message> read_from(
        Offset offset,
        std::size_t limit) const;

    [[nodiscard]] PartitionId id() const noexcept {
        return id_;
    }

    [[nodiscard]] Offset next_offset() const noexcept {
        return messages_.size();
    }

private:
    PartitionId id_{};
    std::vector<Message> messages_;
};

} // namespace hyper
