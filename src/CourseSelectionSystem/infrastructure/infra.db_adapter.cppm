/**
 *
 * @file    src/CourseSelectionSystem/infrastructure/infra.db_adapter.cppm
 * @date    2026-01-09
 * @author  GY
 * @brief   Infrastructure Partition: Database Adapter Interface
 *
 * 定义了数据库适配器模块接口。
 * 本模块作为 `lib_db_core` 的上层封装，向应用层提供标准的数据库访问能力。
 *
 * 架构说明：
 * - 接口纯净：完全不包含任何非标准库的 #include，确保模块接口的轻量和快速扫描。
 * - 依赖隔离：通过 PIMPL 模式将对 `lib_db_core` (及其背后的 `libpqxx`) 的依赖隐藏在实现文件中。
 *
 * Change Log:
 * [v2.0] GY   2026-01-09
 * * 重构为纯模块接口，移除全局模块片段中的 include <pqxx>，解决 GCC 路径
 * [v4.1] GY   2026-01-10
 * * 修复编译错误：移除错误的继承声明，正确实现 PIMPL 模式接口定义，与实现文件对齐。
 *
 */


export module infrastructure:db_adapter;

import std;

export namespace db {

// 数据库行类型 (字符串向量)
using Row = std::vector<std::string>;

//  数据库结果集类型 (行向量)
using Result = std::vector<Row>;

// 数据库适配器类:负责管理数据库连接并执行 SQL 操作。
class DBAdapter {
public:
    DBAdapter();
    ~DBAdapter();

    // 禁止拷贝，允许移动
    DBAdapter(const DBAdapter&) = delete;
    DBAdapter& operator=(const DBAdapter&) = delete;
    DBAdapter(DBAdapter&&) noexcept;
    DBAdapter& operator=(DBAdapter&&) noexcept;

    void set_credentials(std::string conn_str); // 设置连接凭据
    bool connect(const std::string& conn_str = ""); // 连接数据库
    bool execute(const std::string& sql); // 执行非查询 SQL
    std::optional<Result> query(const std::string& sql); // 执行查询 SQL
    bool is_connected() const; // 检查连接状态

private:
    struct Impl;
    Impl* m_pImpl;
};

} // namespace db
