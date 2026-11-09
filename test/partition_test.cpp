#include <cassert>

#include "partition/partition.hpp"

int main() {
    hyper::Partition partition{7};

    assert(partition.id() == 7);
    assert(partition.next_offset() == 0);

    const auto first_offset = partition.append("player-1", "login");
    const auto second_offset = partition.append("player-2", "level-up");
    const auto third_offset = partition.append("player-1", "logout");

    assert(first_offset == 0);
    assert(second_offset == 1);
    assert(third_offset == 2);
    assert(partition.next_offset() == 3);

    const auto messages = partition.read_from(1, 2);
    assert(messages.size() == 2);
    assert(messages[0].offset == 1);
    assert(messages[0].key == "player-2");
    assert(messages[0].payload == "level-up");
    assert(messages[1].offset == 2);
    assert(messages[1].key == "player-1");
    assert(messages[1].payload == "logout");

    const auto limited_messages = partition.read_from(0, 1);
    assert(limited_messages.size() == 1);
    assert(limited_messages[0].offset == 0);

    assert(partition.read_from(3, 10).empty());
    assert(partition.read_from(10, 10).empty());
    assert(partition.read_from(0, 0).empty());

    return 0;
}
