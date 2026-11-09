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
