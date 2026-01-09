/**
* @file    src/CourseSelectionSystem/infrastructure/infra.db_adapter.cppm
* @date    2026-01-07
* @author  GY
* @brief   基础设施层分区：数据库适配器
*
* infra.db_adapter:基础设施层数据库适配器模块
* 封装 PostgreSQL 数据库访问逻辑，提供通用的执行 (execute) 和查询 (query) 接口
* 使用 Global Module Fragment 规避 C++ Modules 与 pqxx 头文件的冲突
* 采用“即开即闭”的连接策略，避免持久连接导致的模块导出复杂性
* 本模块作为 `lib_db_core` 的上层封装，向应用层提供标准的数据库访问能力。
*
* 架构说明：
 * - 接口纯净：完全不包含任何非标准库的 #include，确保模块接口的轻量和快速扫描。
 * - 依赖隔离：通过 PIMPL 模式将对 `lib_db_core` (及其背后的 `libpqxx`) 的依赖隐藏在实现文件中。
*
* Change Log:
* [v1.0] GY   2026-01-07
* * 借鉴 postgre_demo 项目实现数据库适配器
* * 解决 libpqxx 与 C++ Modules 的兼容性问题
* [v1.5] GY   2026-01-07
* * 重构代码结构，实现声明与实现分离
* * 优化接口注释，支持 std::optional 结果集和显式字符串拷贝，解决 ABI 兼容导致的查询空值问题
* [v2.0] GY   2026-01-09
* * 重构为纯模块接口，移除全局模块片段中的 include <pqxx>，解决 GCC 路径 Bug。
*/

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
class DBAdapter : public lib_db_core::DBCore {
public:
    DBAdapter(); // 构造函数：初始化适配器
    ~DBAdapter(); // 析构函数：释放资源
    
    bool connect(const std::string& conn_str); // 连接数据库
    bool execute(const std::string& sql); // 执行非查询 SQL
    std::optional<Result> query(const std::string& sql); // 执行查询 SQL
};

} // namespace db
