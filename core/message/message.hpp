#pragma once

#include <cstdint>
#include <string>

namespace hyper {
    using Offset = std::uint64_t;

    struct Message {
        Offset offset{0};
        std::string key;
        std::string payload;
    };
}
