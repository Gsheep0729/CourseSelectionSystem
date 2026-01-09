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