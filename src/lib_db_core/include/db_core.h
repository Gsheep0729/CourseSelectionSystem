/**
 * @file    src/lib_db_core/include/db_core.h
 * @date    2026-01-09
 * @author  GY
 * @brief   Database Core Library Header
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
    /**
     * @brief 构造函数
     * 初始化 PIMPL 指针，不建立连接。
     */
    DBCore();

    /**
     * @brief 析构函数
     * 自动释放数据库连接资源。
     */
    ~DBCore();

    // 禁止拷贝，允许移动 (PIMPL 最佳实践)
    DBCore(const DBCore&) = delete;
    DBCore& operator=(const DBCore&) = delete;
    DBCore(DBCore&&) noexcept;
    DBCore& operator=(DBCore&&) noexcept;

    /**
     * @brief 设置连接字符串
     * @param conn_str PostgreSQL 连接字符串 (例如: "postgresql://user:pass@host/db")
     */
    void set_credentials(const std::string& conn_str);

    /**
     * @brief 建立数据库连接
     * 尝试使用存储的凭据连接数据库。如果连接已存在且有效，则直接返回 true。
     * @return 成功返回 true，失败返回 false (并打印错误日志)
     */
    bool connect();

    /**
     * @brief 执行非查询 SQL (INSERT, UPDATE, DELETE)
     * 这是一个事务性操作，自动提交。
     * @param sql 要执行的 SQL 语句
     * @return 执行成功返回 true，发生异常返回 false
     */
    bool execute(const std::string& sql);

    /**
     * @brief 执行查询 SQL (SELECT)
     * @param sql 要执行的查询语句
     * @return 成功返回结果集(Result)，失败返回 std::nullopt
     * @note 结果集中的 NULL 字段将被转换为空字符串 ""。
     */
    std::optional<Result> query(const std::string& sql);

    /**
     * @brief 检查是否已连接
     * @return 如果连接对象存在且处于开启状态，返回 true
     */
    bool is_connected() const;

private:
    struct Impl; // 前向声明实现结构体
    std::unique_ptr<Impl> pImpl; // 指向具体实现的指针
};