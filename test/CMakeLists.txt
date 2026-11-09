add_executable(hypermq_partition_tests
    ${CMAKE_CURRENT_SOURCE_DIR}/partition_test.cpp
)

target_link_libraries(hypermq_partition_tests
    PRIVATE
        HyperMQ::HyperMQ
)

add_test(
    NAME hypermq_partition_tests
    COMMAND hypermq_partition_tests
)

add_executable(hypermq_topic_tests
    ${CMAKE_CURRENT_SOURCE_DIR}/topic_test.cpp
)

target_link_libraries(hypermq_topic_tests
    PRIVATE
        HyperMQ::HyperMQ
)

add_test(
    NAME hypermq_topic_tests
    COMMAND hypermq_topic_tests
)

add_executable(hypermq_broker_tests
    ${CMAKE_CURRENT_SOURCE_DIR}/broker_test.cpp
)

target_link_libraries(hypermq_broker_tests
    PRIVATE
        HyperMQ::HyperMQ
)

add_test(
    NAME hypermq_broker_tests
    COMMAND hypermq_broker_tests
)
