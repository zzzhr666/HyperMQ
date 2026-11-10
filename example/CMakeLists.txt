add_executable(hypermq_example
    main.cpp
)

target_link_libraries(hypermq_example
    PRIVATE
        HyperMQ::HyperMQ
)
