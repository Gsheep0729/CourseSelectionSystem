# 项目: lib_db_core

## 项目特征总结





---

## 构建配置文件
---

### File: CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 4.1.1)

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
    include/db_core.h
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

```

---

## 项目源文件
---

### File: src/db_core.cpp
```cpp
/**
* @file    src/lib_db_core/src/db_core.cpp
* @date    2026-01-10
* @author  GY
* @brief   数据库核心库实现文件
*
* 实现 db_core.h 中定义的数据库操作接口。
* 包含 PIMPL (DBCoreImpl) 的具体定义，持有 pqxx::connection 对象。
* 负责执行实际的 SQL 语句，并将 pqxx::result 转换为通用的 Result 类型。
*
* Change Log:
* [v1.0] GY   2026-01-10
* * 初始版本：实现 PIMPL 模式封装 pqxx::connection。
* * 提供基本的 connect, execute, query 接口。
* */

#include "db_core.h"
#include <pqxx/pqxx>
#include <iostream>
#include <mutex>
#include <format> // 用于简单的日志格式化 (C++20)

// PIMPL 实现结构体定义
struct DBCore::Impl {
    std::string connection_string;          // 数据库连接字符串
    std::unique_ptr<pqxx::connection> connection; // pqxx 连接对象指针
    std::mutex db_mutex;                    // 保护连接对象的线程安全互斥锁

    /**
     * @brief 确保连接可用
     * 如果连接未建立或已断开，尝试重连。
     * @return 连接是否可用
     */
    bool ensure_connection() {
        if (connection && connection->is_open()) {
            return true;
        }
        try {
            if (connection_string.empty()) {
                std::cerr << "[DBCore Error] Connection string is empty. Please call set_credentials first." << std::endl;
                return false;
            }
            std::cout << "[DBCore] Attempting to connect to database..." << std::endl;
            connection = std::make_unique<pqxx::connection>(connection_string);
            
            if (connection->is_open()) {
                std::cout << "[DBCore] Connection successfully established: " << connection->dbname() << std::endl;
                return true;
            } else {
                std::cerr << "[DBCore Error] Connection object created but is not open." << std::endl;
                return false;
            }
        } catch (const std::exception& e) {
            std::cerr << "[DBCore Exception] Reconnect failed: " << e.what() << std::endl;
            return false;
        }
    }
};

// 构造与析构
DBCore::DBCore() : pImpl(std::make_unique<Impl>()) {}
DBCore::~DBCore() = default;

// 移动语义
DBCore::DBCore(DBCore&&) noexcept = default;
DBCore& DBCore::operator=(DBCore&&) noexcept = default;

void DBCore::set_credentials(const std::string& conn_str) {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    pImpl->connection_string = conn_str;
    // 注意：这里不立即连接，采用 Lazy Connection 策略
}

bool DBCore::connect() {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    return pImpl->ensure_connection();
}

bool DBCore::execute(const std::string& sql) {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    if (!pImpl->ensure_connection()) return false;

    try {
        pqxx::work W(*pImpl->connection);
        W.exec(sql);
        W.commit();
        // std::cout << "[DBCore] Executed SQL successfully." << std::endl; // 过于啰嗦，可注释掉
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[DBCore Exception] Execute failed.\n"
                  << "  Error: " << e.what() << "\n"
                  << "  SQL:   " << sql << std::endl;
        return false;
    }
}

std::optional<Result> DBCore::query(const std::string& sql) {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    if (!pImpl->ensure_connection()) return std::nullopt;

    try {
        pqxx::nontransaction N(*pImpl->connection);
        pqxx::result R(N.exec(sql));

        Result result_set;
        result_set.reserve(R.size());

        for (const auto& row : R) {
            Row current_row;
            current_row.reserve(row.size());
            for (const auto& field : row) {
                // 处理数据库中的 NULL 值，将其转换为空字符串，防止程序崩溃
                if (field.is_null()) {
                    current_row.push_back(""); // 空值转空字符串
                } else {
                    current_row.push_back(field.c_str());
                }
            }
            result_set.push_back(std::move(current_row));
        }
        return result_set;
    } catch (const std::exception& e) {
        std::cerr << "[DBCore Exception] Query failed.\n"
                  << "  Error: " << e.what() << "\n"
                  << "  SQL:   " << sql << std::endl;
        return std::nullopt;
    }
}

bool DBCore::is_connected() const {
    // 线程不安全读取，仅作简单状态检查
    return pImpl->connection && pImpl->connection->is_open();
}

```

---

### File: include/db_core.h
```cpp
/**
 * @file    src/lib_db_core/include/db_core.h
 * @date    2026-01-09
 * @author  GY
 * @brief   数据库核心库头文件
 *
 * 定义了基于 PostgreSQL 的数据库核心操作接口。
 * 采用 PIMPL (Pointer to Implementation) 模式隐藏 pqxx 依赖，确保 ABI 稳定性和编译隔离。
 * 提供连接管理、SQL 执行和结果集查询功能。
 *
 * Change Log:
 * [v1.0] GY   2026-01-09
 * * 初始版本：实现 PIMPL 模式封装 pqxx::connection。
 * * 提供基本的 connect, execute, query 接口。
 * * 定义通用的 Row 和 Result 类型别名。
 */

#pragma once

#include <vector>
#include <string>
#include <memory>
#include <optional>

// 定义与现有项目一致的数据类型
using Row = std::vector<std::string>;
using Result = std::vector<Row>;

class DBCore {
public:
    DBCore(); // 构造函数：初始化 PIMPL 指针
    ~DBCore(); // 析构函数：释放数据库连接资源

    // 禁止拷贝，允许移动 (PIMPL 最佳实践)
    DBCore(const DBCore&) = delete;
    DBCore& operator=(const DBCore&) = delete;
    DBCore(DBCore&&) noexcept;
    DBCore& operator=(DBCore&&) noexcept;

    void set_credentials(const std::string& conn_str); // 设置连接字符串
    bool connect(); // 建立数据库连接
    bool execute(const std::string& sql); // 执行非查询 SQL (INSERT, UPDATE, DELETE)
    std::optional<Result> query(const std::string& sql); // 执行查询 SQL (SELECT)
    bool is_connected() const; // 检查是否已连接

private:
    struct Impl; // 前向声明实现结构体
    std::unique_ptr<Impl> pImpl; // 指向具体实现的指针 (PIMPL)
};
```

---

