cmake_minimum_required(VERSION 3.20)

project(db_core LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 libpqxx
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

# 创建静态库
add_library(db_core STATIC
    src/db_core.cpp
)

# 包含头文件目录
target_include_directories(db_core SYSTEM PUBLIC
    include
)

# 链接 libpqxx (仅作为私有依赖，避免头文件泄露给使用者)
target_include_directories(db_core PRIVATE ${PQXX_INCLUDE_DIRS})
target_link_libraries(db_core PRIVATE ${PQXX_LIBRARIES})
target_link_directories(db_core PRIVATE ${PQXX_LIBRARY_DIRS}) # Best practice to include lib dirs too

# 确保 PIC (Position Independent Code)，因为它是静态库，可能会被链接到动态库或其他位置
set_property(TARGET db_core PROPERTY POSITION_INDEPENDENT_CODE ON)
