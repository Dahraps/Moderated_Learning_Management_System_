cmake_minimum_required(VERSION 3.16)
project(MLMS VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(MLMS_ENABLE_GTEST "Build the GoogleTest Phase 5 test suite" ON)
option(MLMS_ENABLE_COVERAGE "Enable GCC/Clang coverage instrumentation" OFF)

find_package(SQLite3 REQUIRED)
find_package(OpenSSL REQUIRED)

if(MLMS_ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(--coverage -O0 -g)
        add_link_options(--coverage)
    else()
        message(FATAL_ERROR "MLMS_ENABLE_COVERAGE currently supports GCC or Clang")
    endif()
endif()

add_library(mlms_core
    src/LMS.cpp
    src/Console.cpp
    src/User.cpp
    src/SubmissionRepository.cpp
    src/DataAccess.cpp
)
target_include_directories(mlms_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(mlms_core PUBLIC SQLite::SQLite3 OpenSSL::Crypto)

if(MSVC)
    target_compile_options(mlms_core PRIVATE /W4)
else()
    target_compile_options(mlms_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(mlms src/main.cpp)
target_link_libraries(mlms PRIVATE mlms_core)

enable_testing()

# Existing project regression tests
set(MLMS_EXISTING_TESTS
    test_logic
    test_roles
    test_srs
    test_rules
    test_patterns
    test_password_security
    test_audit
    test_validation
)
foreach(test_name IN LISTS MLMS_EXISTING_TESTS)
    set(target_name "mlms_${test_name}")
    add_executable(${target_name} "tests/${test_name}.cpp")
    target_link_libraries(${target_name} PRIVATE mlms_core)
    add_test(NAME ${target_name} COMMAND ${target_name})
endforeach()
set_tests_properties(mlms_test_roles mlms_test_srs mlms_test_rules mlms_test_audit
    PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

if(MLMS_ENABLE_GTEST)
    include(FetchContent)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(googletest)

    add_executable(mlms_phase5_gtests tests/test_phase5_gtest.cpp)
    target_link_libraries(mlms_phase5_gtests PRIVATE mlms_core GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(mlms_phase5_gtests)
endif()
