find_package(GTest CONFIG REQUIRED)

include(GoogleTest)

add_executable(hypermq_tests
    ${CMAKE_CURRENT_SOURCE_DIR}/partition_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/topic_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/broker_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/consumer_group_test.cpp
)

target_link_libraries(hypermq_tests
    PRIVATE
        HyperMQ::HyperMQ
        GTest::gtest_main
)

gtest_discover_tests(hypermq_tests)
