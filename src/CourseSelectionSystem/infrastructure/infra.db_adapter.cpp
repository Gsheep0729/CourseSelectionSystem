/**
 * @file    src/CourseSelectionSystem/infrastructure/infra.db_adapter.cpp
 * @date    2026-01-09
 * @author  GY
 * @brief   Infrastructure Partition Implementation
 *
 * 实现了 DBAdapter 类。
 * 此文件是 C++ Modules 编译环境下的特殊产物。
 *
 * !!! 架构警告 (Architecture Warning) !!!
 * 为了规避 GCC 14/15 模块扫描器在处理包含非 ASCII (中文) 路径的 #include 时崩溃的 Bug，
 * 本文件采用了 "手动内联声明 (Manual Inline Declaration)" 策略。
 * 我们没有 #include "db_core.h"，而是将其类定义直接复制到了 Global Module Fragment 中。
 * 这样做是为了欺骗构建系统，使其无需解析外部头文件路径，从而保证编译通过。
 * 一旦编译器修复此 Bug，应恢复为标准的 #include "db_core.h"。
 */

module;

#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <print>

// =========================================================================================
// [Workaround Start] 手动内联 db_core.h
// 目的：规避 GCC/CMake 模块扫描器在中文路径下的 "INCLUDE-TRANSLATE" 错误。
// 注意：必须保持与 lib_db_core/include/db_core.h 完全一致！
// =========================================================================================

using Row = std::vector<std::string>;
using Result = std::vector<Row>;

class DBCore {
public:
    DBCore();
    ~DBCore();

    DBCore(const DBCore&) = delete;
    DBCore& operator=(const DBCore&) = delete;
    DBCore(DBCore&&) noexcept;
    DBCore& operator=(DBCore&&) noexcept;

    void set_credentials(const std::string& conn_str);
    bool connect();
    bool execute(const std::string& sql);
    std::optional<Result> query(const std::string& sql);
    bool is_connected() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
// =========================================================================================
// [Workaround End]
// =========================================================================================

module course_system; // 属于 course_system 主模块的实现单元
import :infrastructure; // 导入 infrastructure 分区定义

import std;

namespace db {

// PIMPL 实现：持有 DBCore 实例
struct DBAdapter::Impl {
    DBCore core;
};

// -------------------------------------------------------------------------
// DBAdapter 实现转发 (Forwarding)
// -------------------------------------------------------------------------

DBAdapter::DBAdapter() : m_pImpl(std::make_unique<Impl>()) {}

DBAdapter::~DBAdapter() = default;

DBAdapter::DBAdapter(DBAdapter&&) noexcept = default;
DBAdapter& DBAdapter::operator=(DBAdapter&&) noexcept = default;


/**
 * @brief 配置数据库连接凭据
 * @param conn_str PostgreSQL 连接字符串
 */
void DBAdapter::set_credentials(std::string conn_str) {
    m_pImpl->core.set_credentials(conn_str);
}


/**
 * @brief 连接数据库
 * @param conn_str (可选) 连接字符串，若提供则更新配置
 * @return true 连接成功
 */
bool DBAdapter::connect(const std::string& conn_str) {
    if (!conn_str.empty()) {
        m_pImpl->core.set_credentials(conn_str);
    }
    // 添加简单的上层日志
    // std::println("[DBAdapter] Connecting..."); 
    return m_pImpl->core.connect();
}


/**
 * @brief 执行非查询 SQL
 * @param sql SQL 语句
 * @return true 执行成功
 */
bool DBAdapter::execute(const std::string& sql) {
    return m_pImpl->core.execute(sql);
}


/**
 * @brief 执行查询 SQL
 * @param sql SELECT 语句
 * @return std::optional<Result> 查询结果
 */
std::optional<Result> DBAdapter::query(const std::string& sql) {
    return m_pImpl->core.query(sql);
}


/**
 * @brief 连接状态检查
 */
bool DBAdapter::is_connected() const {
    return m_pImpl->core.is_connected();
}

} // namespace db
