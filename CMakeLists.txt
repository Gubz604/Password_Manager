cmake_minimum_required(VERSION 3.20)

project(PasswordManager
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(vault
    src/main.cpp
    src/Vault.cpp
)

target_include_directories(vault PRIVATE
    src
)

# Warnings (GCC/Clang vs MSVC)
if (MSVC)
    target_compile_options(vault PRIVATE /W4 /permissive-)
else()
    target_compile_options(vault PRIVATE -Wall -Wextra -Wpedantic)
endif()
