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
    src/fileIOHandler.cpp
    src/Crypto.cpp
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


find_package(PkgConfig REQUIRED)
pkg_check_modules(SODIUM REQUIRED libsodium)

target_include_directories(vault PRIVATE ${SODIUM_INCLUDE_DIRS})
target_link_directories(vault PRIVATE ${SODIUM_LIBRARY_DIRS})
target_link_libraries(vault PRIVATE ${SODIUM_LIBRARIES})
target_compile_options(vault PRIVATE ${SODIUM_CFLAGS_OTHER})
