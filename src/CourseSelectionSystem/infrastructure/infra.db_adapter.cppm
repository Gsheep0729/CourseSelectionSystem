/**
* @file    src/CourseSelectionSystem/infrastructure/infra.db_adapter.cppm
* @date    2026-01-07
* @author  GY
* @brief   Infrastructure partition: Database Adapter
*
* infra.db_adapter:基础设施层数据库适配器模块
* 封装 PostgreSQL 数据库访问逻辑，提供通用的执行 (execute) 和查询 (query) 接口
* 使用 Global Module Fragment 规避 C++ Modules 与 pqxx 头文件的冲突
* 采用“即开即闭”的连接策略，避免持久连接导致的模块导出复杂性
*
* Change Log:
* [v1.0] GY   2026-01-07
* * 借鉴 postgre_demo 项目实现数据库适配器
* * 解决 libpqxx 与 C++ Modules 的兼容性问题
* [v1.5] GY   2026-01-07
* * 重构代码结构，实现声明与实现分离
* * 优化接口注释，支持 std::optional 结果集和显式字符串拷贝，解决 ABI 兼容导致的查询空值问题
*/
module;
#include <pqxx/pqxx>

export module course_system:infrastructure;

import std;

    export namespace db {

// 数据库行类型别名，表示一行数据（字符串数组）
using Row = std::vector<std::string>;

// 数据库结果集类型别名，表示多行数据
using Result = std::vector<Row>;

class DBAdapter {
public:
    // 构造函数与析构函数
    DBAdapter() = default;
    ~DBAdapter() = default;

    // 设置数据库连接信息
    void set_credentials(std::string conn_str);

    // 检查数据库连接状态
    bool connect(const std::string& conn_str = "");

    // 执行增删改等非查询 SQL 语句
    bool execute(const std::string& sql);

    // 执行 SELECT 查询 SQL 语句
    std::optional<Result> query(const std::string& sql);

    // 检查适配器是否已配置连接信息
    bool is_connected() const;

private:
    std::string m_conn_str; ///< 数据库连接字符串
    // 注意：类内不持有 pqxx 成员对象，以保证 C++ Modules 的导出安全
};

// -------------------------------------------------------------------------
// 实现部分 (Implementation)
// -------------------------------------------------------------------------

/**
 * @brief 设置数据库连接信息
 * @param conn_str PostgreSQL 连接字符串
 */
void DBAdapter::set_credentials(std::string conn_str) {
    m_conn_str = std::move(conn_str);
}


/**
 * @brief 检查数据库连接状态
 * @param conn_str 可选的连接字符串，若提供则更新内部存储
 * @return 连接成功返回 true，否则返回 false
 */
bool DBAdapter::connect(const std::string& conn_str) {
    if (!conn_str.empty()) {
        m_conn_str = conn_str;
    }

    try {
        pqxx::connection C(m_conn_str);
        if (C.is_open()) {
            std::print("[DB] Connection check passed: {}\n", C.dbname());
            return true;
        }
    } catch (const std::exception& e) {
        std::print("[DB Exception] Connect check: {}\n", e.what());
    }
    return false;
}


/**
 * @brief 执行增删改等非查询 SQL 语句
 * @param sql 要执行的 SQL 语句
 * @return 执行成功返回 true，发生异常返回 false
 */
bool DBAdapter::execute(const std::string& sql) {
    try {
        // 采用即时连接模式，确保资源在操作完成后立即释放
        pqxx::connection C(m_conn_str);
        pqxx::work W(C);
        W.exec(sql);
        W.commit();
        return true;
    } catch (const std::exception& e) {
        std::print("[DB Exception] Execute: {}\nSQL: {}\n", e.what(), sql);
        return false;
    }
}


/**
 * @brief 执行 SELECT 查询 SQL 语句
 * @param sql 要执行的查询语句
 * @return 成功返回包含结果集的 std::optional，失败返回 std::nullopt
 */
std::optional<Result> DBAdapter::query(const std::string& sql) {
    try {
        pqxx::connection C(m_conn_str);
        pqxx::nontransaction N(C);
        pqxx::result R(N.exec(sql));

        Result result_set;
        result_set.reserve(R.size());

        for (const auto& row : R) {
            Row current_row;
            current_row.reserve(row.size());
            for (const auto& field : row) {
                // 处理空字段，转换为零长度字符串
                if (field.is_null()) {
                    current_row.push_back("");
                } else {
                    current_row.push_back(field.c_str());
                }
            }
            result_set.push_back(std::move(current_row));
        }
        return result_set;
    } catch (const std::exception& e) {
        std::print("[DB Exception] Query: {}\nSQL: {}\n", e.what(), sql);
        return std::nullopt;
    }
}


/**
 * @brief 检查适配器是否已配置连接信息
 */
bool DBAdapter::is_connected() const {
    return !m_conn_str.empty();
}

    } // namespace db
