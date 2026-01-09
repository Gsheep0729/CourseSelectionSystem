/**
* @file    src/CourseSelectionSystem/infrastructure/infra.db_adapter.cpp
* @date    2026-01-10
* @author  GY
* @brief   数据库适配器实现文件
*
* 实现 DBAdapter 类的方法。
* 负责将应用层的数据库请求转发给底层的 lib_db_core 库。
* 处理具体的类型转换和参数绑定。
*
* Change Log:
* [v1.0] GY   2026-01-10
* * 初始版本：实现 query 和 execute 方法。
*/

module;

#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <print>

#include "db_core.h"

module infrastructure; // 属于 course_system 主模块的实现单元
import std;


namespace db {

// PIMPL 实现：持有 DBCore 实例
struct DBAdapter::Impl {
    DBCore core;
};

// -------------------------------------------------------------------------
// DBAdapter 实现转发 (Forwarding)
// -------------------------------------------------------------------------

DBAdapter::DBAdapter() : m_pImpl(new Impl()) {}

DBAdapter::~DBAdapter() {
    delete m_pImpl;
}

DBAdapter::DBAdapter(DBAdapter&& other) noexcept : m_pImpl(other.m_pImpl) {
    other.m_pImpl = nullptr;
}
DBAdapter& DBAdapter::operator=(DBAdapter&& other) noexcept {
    if (this != &other) {
        delete m_pImpl;
        m_pImpl = other.m_pImpl;
        other.m_pImpl = nullptr;
    }
    return *this;
}


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
