set(HYPERMQ_TEST_SOURCES)

if(HYPERMQ_TEST_SOURCES)
    add_executable(hypermq_tests
        ${HYPERMQ_TEST_SOURCES}
    )

    target_link_libraries(hypermq_tests
        PRIVATE
            HyperMQ::HyperMQ
    )

    add_test(
        NAME hypermq_tests
        COMMAND hypermq_tests
    )
endif()
