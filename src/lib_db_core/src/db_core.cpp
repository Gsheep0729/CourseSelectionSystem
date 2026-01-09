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
