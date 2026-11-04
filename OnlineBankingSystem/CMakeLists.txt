cmake_minimum_required(VERSION 3.12)
project(OnlineBankingSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(OnlineBankingSystem
    src/main.cpp
    src/Account.cpp
    src/Authentication.cpp
    src/Admin.cpp
    src/Bank.cpp
    src/Customer.cpp
)

target_include_directories(OnlineBankingSystem PRIVATE include)

if (MSVC)
    target_compile_options(OnlineBankingSystem PRIVATE /W4)
else()
    target_compile_options(OnlineBankingSystem PRIVATE -Wall -Wextra -Wpedantic)
endif()
