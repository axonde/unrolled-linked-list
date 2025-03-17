include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
    unrolled-list-lib-tests
    allocator_ut.cpp
    # exception_safety_ut.cpp
    named_requirements_ut.cpp
    no_default_constructible_ut.cpp
    simple_ut.cpp
    iterators_ut.cpp
)

target_link_libraries(
    unrolled-list-lib-tests
    GTest::gtest_main
    GTest::gmock_main
)

target_include_directories(unrolled-list-lib-tests PUBLIC ${PROJECT_SOURCE_DIR})

include(GoogleTest)

gtest_discover_tests(unrolled-list-lib-tests)
