add_executable(hypermq_example
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_link_libraries(hypermq_example
    PRIVATE
        HyperMQ::HyperMQ
)
