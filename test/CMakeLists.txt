find_package(GTest CONFIG REQUIRED)

include(GoogleTest)

add_executable(hypermq_tests
    partition_test.cpp
    topic_test.cpp
    broker_test.cpp
    consumer_group_test.cpp
    consumer_test.cpp
)

target_link_libraries(hypermq_tests
    PRIVATE
        HyperMQ::HyperMQ
        GTest::gtest_main
)

gtest_discover_tests(hypermq_tests)
