set(HYPERMQ_CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/broker/broker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/partition/partition.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/topic/topic.cpp
)

set(HYPERMQ_CORE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/broker/broker.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/message/message.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/partition/partition.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/topic/topic.hpp
)

add_library(hyper_mq STATIC
    ${HYPERMQ_CORE_SOURCES}
    ${HYPERMQ_CORE_HEADERS}
)

set_target_properties(hyper_mq PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

add_library(HyperMQ::HyperMQ ALIAS hyper_mq)

target_compile_features(hyper_mq PUBLIC
    cxx_std_20
)

target_include_directories(hyper_mq PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
