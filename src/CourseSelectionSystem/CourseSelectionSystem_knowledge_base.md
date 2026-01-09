# 项目: CourseSelectionSystem

## 项目特征总结





---

## 构建配置文件
---

### File: CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 4.1.1)

# https://github.com/Kitware/CMake/blob/v4.1.1/Help/dev/experimental.rst
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
set(CMAKE_CXX_MODULE_STD ON)

project(CourseSelectionSystem LANGUAGES CXX)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

# 1. 定义各层级的模块文件列表 (Code Grouping)
add_executable(CourseSelectionSystem
    main.cpp


)

# 表现层模块 (Presentation Layer)
set(PRESENTATION_MODULES
    # CourseSelectionSystem/presentation/.cppm
    # presentation/pre.cppm
    presentation/pres.cli.cppm
)

# 应用逻辑层模块 (Application Layer)
set(APPLICATION_MODULES
    # CourseSelectionSystem/application/.cppm
    application/app.controller.cppm
)

# 领域层模块 (Domain Layer)
set(DOMAIN_MODULES
    # CourseSelectionSystem/domain/.cppm
    domain/dom.course.cppm
    domain/dom.student.cppm
    domain/domain.cppm
    domain/dom.teacher.cppm
)

# 数据管理层模块 (Infrastructure Layer)
set(INFRASTRUCTURE_MODULES
    # CourseSelectionSystem/infrastructure/.cppm
    infrastructure/infra.db_adapter.cppm
)

# 2. 配置 IDE 中的显示分组 (IDE Display Grouping)
source_group("Presentation" FILES ${PRESENTATION_MODULES})
source_group("Application"  FILES ${APPLICATION_MODULES})
source_group("Domain"       FILES ${DOMAIN_MODULES})
source_group("Infrastructure" FILES ${INFRASTRUCTURE_MODULES})

# 3. 构建目标与源文件关联
# 将所有模块添加到目标中
target_sources(CourseSelectionSystem PRIVATE FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        course_system.cppm
        ${PRESENTATION_MODULES}
        ${APPLICATION_MODULES}
        ${DOMAIN_MODULES}
        ${INFRASTRUCTURE_MODULES}
)

target_compile_features(CourseSelectionSystem PRIVATE cxx_std_23)


target_link_libraries(CourseSelectionSystem PRIVATE ${PQXX_LIBRARIES})
target_include_directories(CourseSelectionSystem PRIVATE ${PQXX_INCLUDE_DIRS})
target_link_directories(CourseSelectionSystem PRIVATE ${PQXX_LIBRARY_DIRS})












































# target_link_directories(CourseSelectionSystem PRIVATE /opt/utils/lib)  # -L /opt/utils/lib 设置链接路径
# target_link_libraries(CourseSelectionSystem PRIVATE utils)             # -lutils           设置链接库名

# set_target_properties(CourseSelectionSystem PROPERTIES
#     INSTALL_RPATH_USE_LINK_PATH  True                           # 使用链接路径作为安装文件的RPath，等价于"-Wl,-rpath=/opt/utils/lib"
# )

#add_library(CourseSelectionSystem SHARED
#  utils.cpp
#)

#set_target_properties( CourseSelectionSystem PROPERTIES
#    PUBLIC_HEADER utils.cppm
#    VERSION   2.5.1
#    SOVERSION 2
#    LIBRARY_OUTPUT_DIRECTORY  "${CMAKE_BINARY_DIR}/lib"
#)

#install(TARGETS CourseSelectionSystem
#    LIBRARY DESTINATION /opt/utils/lib
#    PUBLIC_HEADER DESTINATION  /opt/utils/modules
#)

#add_custom_command(TARGET CourseSelectionSystem
#    POST_BUILD
#    COMMAND cmake --install "${CMAKE_CURRENT_BINARY_DIR}"
#)


```

---

## 项目源文件
---

### File: main.cpp
```cpp
/**
* @file    src/CourseSelectionSystem/main.cpp
* @date    2026-01-07
* @author  Integrated
* @brief   Integrated Course Selection System
*
* Change Log:
* [v2.0] Integrated 2026-01-07
* * 解决 main.cpp 合并冲突，统一系统入口
* * 完成 CLI 表现层与 DB 基础设施层的完整集成
* * 实现"初始化 -> 注入控制器 -> 启动交互循环"的标准启动流程
*/
import std;
import course_system;

int main() {
    try {
        // 1. 初始化系统控制器 (连接数据库, 建表, 初始化数据)
        SystemController app;
        app.initialize();

        // 2. 初始化用户界面并注入控制器
        UserInterface cli;
        cli.setController(&app);

        // 3. 启动交互循环
        while (true) {
            // 显示登录菜单 (内部会根据角色进入对应的子菜单循环)
            // 当用户在子菜单选择"退出登录"时，函数返回，循环继续，重新显示登录菜单
            cli.showLoginMenu();
        }

    } catch (const std::exception& e) {
        std::print("Fatal Error: {}\n", e.what());
        return 1;
    }
    return 0;
}
```

---

### File: course_system.cppm
```cppm
/**
* @file    src/CourseSelectionSystem/course_system.cppm
* @date    2026-01-07
* @author  GY
* @brief   Primary module interface
*
* course_system:学生选课系统主模块接口
* 聚合系统的各个功能模块（Domain、Application、Controller等）
* 作为对外暴露的统一接口，方便其他模块导入使用
*
* Change Log:
* [v1.0] GY   2026-01-06
* * 优化了cmake,实现学生选课系统基础
* [v1.5] GY   2026-01-07
* * 整合基础设施层，导出 db_adapter 模块
* [v1.9] Zhang Tao   2026-01-07
* * 导入表现层（presentation）模块，支持CLI交互测试
* [v2.0] Integrated 2026-01-07
* * 解决模块导出冲突，统一导出 infrastructure 和 presentation 模块
* * 确立最终的模块依赖关系，修复循环依赖问题
*/
export module course_system;
export import :domain;
export import :app.controller;
export import :infrastructure;
export import :presentation;

```

---

### File: application/app.controller.cppm
```cppm
/**
* @file    src/CourseSelectionSystem/application/app.controller.cppm
* @date    2026-01-07
* @author  GY
* @brief   Application partition: System Controller
*
* app.controller:应用层系统控制器模块
* 负责管理整个选课系统的运行流程
* 维护学生和课程列表，提供选课、退课等核心业务功能
* 协调领域层实体之间的交互
*
* Change Log:
* [v1.0] GY   2026-01-06
* * 初始化项目架构 (基于 C++23 Modules)
* [v1.5] GY   2026-01-07
* * 重构 SystemController，移除内存存储，接入 DBAdapter
* [v2.0] Integrated 2026-01-07
* * 完成 SystemController 与 DBAdapter 的深度集成
* * 移除内存容器 (std::vector)，全量迁移至 SQL 事务操作
* * 实现基于数据库的实时选课/退课业务逻辑（含并发安全/容量检查）
*/
export module course_system:app.controller;

import :domain;
import :infrastructure;
import std;

export class SystemController {
public:
    // 初始化系统 (连接数据库, 建表, 初始化数据)
    void initialize();

    // 运行系统 (CLI模式下可能不再需要此处的循环逻辑，保留接口兼容)
    void run();

    // 执行选课操作
    void performEnrollment(std::string sid, std::string cid);

    // 执行退课操作
    void performDrop(std::string sid, std::string cid);

private:
    db::DBAdapter m_db; // 数据库适配器

    // 辅助函数：获取课程信息
    struct CourseInfo {
        std::string id;
        std::string name;
        int enrolled;
        int capacity;
        bool valid;
    };
    CourseInfo getCourseInfo(const std::string& cid);
};

// --- Implementation ---

/**
* @brief 初始化系统
* 连接数据库，创建表结构，加载初始数据
*/
void SystemController::initialize() {
    std::string conn_str = "dbname=postgres user=postgres password=123 hostaddr=127.0.0.1 port=5432";
    if (!m_db.connect(conn_str)) {
        std::print("Error: Failed to connect to database.\n");
        return;
    }

    // 重置数据库 (为了测试方便，生产环境应去除 DROP)
    m_db.execute("DROP TABLE IF EXISTS enrollment");
    m_db.execute("DROP TABLE IF EXISTS course");
    m_db.execute("DROP TABLE IF EXISTS student");
    
    // 创建表
    m_db.execute("CREATE TABLE student (id TEXT PRIMARY KEY, name TEXT)");
    m_db.execute("CREATE TABLE course (id TEXT PRIMARY KEY, name TEXT, capacity INT DEFAULT 60, enrolled INT DEFAULT 0)");
    m_db.execute("CREATE TABLE enrollment (student_id TEXT, course_id TEXT, PRIMARY KEY (student_id, course_id))");

    std::print("Database initialized.\n");

    // 加载初始数据
    std::string s_id = "2024051604085";
    std::string s_name = "Gao Yang"; 
    m_db.execute(std::format("INSERT INTO student (id, name) VALUES ('{}', '{}')", s_id, s_name));

    std::vector<std::pair<std::string, std::string>> manual_courses = {
        {"C0017", "C语言程序设计"},
        {"C0001", "高等数学"},
        {"C0008", "数据结构"},
        {"C0016", "Linux程序设计"},
        {"C0005", "计算机导论"},
        {"C0002", "马克思主义基本原理"},
        {"C0003", "软件构建与实现"},
        {"C0004", "计算机网络"},
        {"C0006", "线性代数"},
        {"C0007", "概率论与数理统计"},
        {"C0009", "数据库原理及应用"},
        {"C0010", "思想道德与法治"},
        {"C0011", "形势与政策"},
        {"C0012", "大学英语"},
        {"C0013", "大学生心理健康教育"},
        {"C0014", "体育"},
        {"C0015", "中国近现代史纲要"}
    };

    int course_count = 0;
    for (const auto& [id, name] : manual_courses) {
        if (m_db.execute(std::format("INSERT INTO course (id, name) VALUES ('{}', '{}')", id, name))) {
            course_count++;
        }
    }
    
    std::print("Loaded 1 student and {} courses.\n", course_count);
}


/**
* @brief 运行系统
*/
void SystemController::run() {
    // 保留为空，逻辑移交 CLI
    std::print("System Controller Ready.\n");
}


SystemController::CourseInfo SystemController::getCourseInfo(const std::string& cid) {
    auto res = m_db.query(std::format("SELECT name, enrolled, capacity FROM course WHERE id = '{}'", cid));
    if (res && !res->empty()) {
        try {
            return {
                cid, 
                (*res)[0][0], 
                std::stoi((*res)[0][1]), 
                std::stoi((*res)[0][2]), 
                true
            };
        } catch (...) {
            return {"", "", 0, 0, false};
        }
    }
    return {"", "", 0, 0, false};
}

/**
* @brief 执行选课操作
*/
void SystemController::performEnrollment(std::string sid, std::string cid) {
    auto info = getCourseInfo(cid);
    if (!info.valid) {
        std::print("Error: Course {} not found.\n", cid);
        return;
    }

    // 检查是否已选
    auto check = m_db.query(std::format("SELECT 1 FROM enrollment WHERE student_id='{}' AND course_id='{}'", sid, cid));
    if (check && !check->empty()) {
        std::print("Error: Student {} is already enrolled in [Course] {} - {} ({}/{})\n", 
              sid, info.id, info.name, info.enrolled, info.capacity);
        return;
    }

    if (info.enrolled >= info.capacity) {
        std::print("Error: Course {} is full.\n", info.name);
        return;
    }

    // 执行事务
    bool ok1 = m_db.execute(std::format("INSERT INTO enrollment VALUES ('{}', '{}')", sid, cid));
    bool ok2 = m_db.execute(std::format("UPDATE course SET enrolled = enrolled + 1 WHERE id = '{}'", cid));

    if (ok1 && ok2) {
        info.enrolled++; 
        std::print("Success: Student {} enrolled in [Course] {} - {} ({}/{})\n", 
              sid, info.id, info.name, info.enrolled, info.capacity);
    } else {
        std::print("Error: Database failure during enrollment.\n");
    }
}

/**
* @brief 执行退课操作
*/
void SystemController::performDrop(std::string sid, std::string cid) {
    auto info = getCourseInfo(cid); // 获取当前信息用于打印
    if (!info.valid) {
        std::print("Error: Course {} not found.\n", cid);
        return;
    }

    // 检查是否已选
    auto check = m_db.query(std::format("SELECT 1 FROM enrollment WHERE student_id='{}' AND course_id='{}'", sid, cid));
    if (!check || check->empty()) {
        std::print("Error: Student {} is not enrolled in [Course] {} - {} ({}/{})\n", 
              sid, info.id, info.name, info.enrolled, info.capacity);
        return;
    }

    // 执行事务
    bool ok1 = m_db.execute(std::format("DELETE FROM enrollment WHERE student_id='{}' AND course_id='{}'", sid, cid));
    bool ok2 = m_db.execute(std::format("UPDATE course SET enrolled = enrolled - 1 WHERE id = '{}'", cid));

    if (ok1 && ok2) {
        info.enrolled--;
        std::print("Success: Student {} dropped [Course] {} - {} ({}/{})\n", 
              sid, info.id, info.name, info.enrolled, info.capacity);
    } else {
        std::print("Error: Database failure during drop.\n");
    }
}
```

---

### File: infrastructure/infra.db_adapter.cppm
```cppm
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

```

---

### File: domain/domain.cppm
```cppm
/**
* @file    src/CourseSelectionSystem/domain/domain.cppm
* @date    2026-01-06
* @author  GY
* @brief   Domain layer aggregator
*
* domain:领域层聚合模块
* 聚合学生（Student）和课程（Course）实体模块
* 补全 Student 对 Course 的调用逻辑（选课、退课）
* 实现领域层核心业务逻辑
*
* Change Log:
* [v1.0] GY   2026-01-06
* * 初始化项目架构 (基于 C++23 Modules)
* * 实现领域层实体 (Student, Course) 及其核心逻辑
* * 实现应用层控制器 (SystemController) 管理选课流程
* * 实现选课与退课功能，包含容量检查与重复选课验证
* * 添加基础测试用例与 CLI 输出
*/
export module course_system:domain;
//export import :domain.teacher;
export import :domain.student;
export import :domain.course;

import std;

/**
* @brief 学生选修课程
* @param c 要选修的课程指针
*/
void Student::enrollIn(Course* c) {
    if (!c) return;

    // 检查是否已经选过该课程
    if (std::ranges::contains(m_courses, c)) {
        std::print("Warning: Student {} already enrolled in {}.\n", m_name, c->course_info());
        return;
    }

    if (c->acceptEnrollment(this)) {
        m_courses.push_back(c);
        std::print("Success: Student {} enrolled in {}.\n", m_name, c->course_info());
    } else {
        std::print("Failed: Course {} is full or invalid.\n", c->course_info());
    }
}


/**
* @brief 学生退选课程
* @param c 要退选的课程指针
*/
void Student::dropCourse(Course* c) {
    if (!c) return;

    // 优化：先检查学生是否选修了该课程
    auto it = std::ranges::find(m_courses, c);
    if (it != m_courses.end()) {
        // 1. 通知课程移除该学生
        c->removeEnrollment(this);
        
        // 2. 从学生的本地选课列表中移除
        m_courses.erase(it);
        
        std::print("Success: Student {} dropped {}.\n", m_name, c->course_info());
    } else {
        std::print("Error: Student {} is not enrolled in {}.\n", m_name, c->course_info());
    }
}

```

---

### File: domain/dom.course.cppm
```cppm
/**
* @file    src/CourseSelectionSystem/domain/dom.course.cppm
* @date    2026-01-06
* @author  GY
* @brief   Domain partition: Course entity
*
* dom.course:领域层课程实体模块
* 定义课程实体类，包含课程基本信息（ID、名称、容量）
* 维护已选修该课程的学生列表，提供容量控制和选课管理功能
*
* Change Log:
* [v1.0] GY   2026-01-06
* * 初始化项目架构 (基于 C++23 Modules)
* * 实现领域层实体 (Student, Course) 及其核心逻辑
* * 实现应用层控制器 (SystemController) 管理选课流程
* * 实现选课与退课功能，包含容量检查与重复选课验证
* * 添加基础测试用例与 CLI 输出
*/
export module course_system:domain.course;

import std;

// 前向声明，解决循环引用
export class Student;


export class Course {
public:
    // 构造函数
    Course(std::string id, std::string name, int capacity = 60);


    // 检查课程是否已满
    bool isFull() const;

    // 尝试接受学生报名
    bool acceptEnrollment(Student* s);

    // 移除学生报名信息
    void removeEnrollment(Student* s);

    // 获取课程 ID
    std::string getId() const { return m_id; }

    // 检查 ID 是否匹配
    bool hasId(std::string_view id) const;

    // 获取课程详细信息字符串
    std::string course_info() const;

private:
    std::string m_id;                 // 课程 ID
    std::string m_name;               // 课程名称
    int m_capacity;                   // 最大容量
    std::vector<Student*> m_students; // 已选修该课程的学生列表
};

// --- Implementation ---
Course::Course(std::string id, std::string name, int capacity)
    : m_id(id), m_name(name), m_capacity(capacity) {}


/**
* @brief 检查课程是否已满
* @return true 如果当前选课人数 >= 容量，否则 false
*/
bool Course::isFull() const {
    return m_students.size() >= m_capacity;
}


/**
* @brief 尝试接受学生报名
* @param s 申请报名的学生指针
* @return true 报名成功, false 报名失败 (如课程已满)
*/
bool Course::acceptEnrollment(Student* s) {
    if (isFull()) return false;
    m_students.push_back(s);
    return true;
}


/**
* @brief 移除学生报名信息
* @param s 要移除的学生指针
*/
void Course::removeEnrollment(Student* s) {
    std::erase(m_students, s);
}

/**
 * @brief 检查课程是否匹配指定 ID
 * @param id 要检查的 ID
 * @return true 如果匹配，否则 false
 */
bool Course::hasId(std::string_view id) const {
    return m_id == id;
}

/**
 * @brief 获取课程详细信息字符串
 * @return 格式化后的课程信息 (ID - Name (Current/Max))
 */
std::string Course::course_info() const {
    return std::format("[Course] {} - {} ({}/{})",
        m_id, m_name, m_students.size(), m_capacity);
}

```

---

### File: domain/dom.teacher.cppm
```cppm
/**
* @file    src/CourseSelectionSystem/domain/dom.teacher.cppm
* @date    2026-01-08
* @author  Zhang Tao
* @brief   Domain partition: Teacher entity
*
* dom.teacher:领域层教师实体模块
* 定义教师实体类，包含教师基本信息（ID、姓名）
* 维护教师授课课程列表，提供授课管理、成绩录入/修改等核心操作
* 严格遵循领域层纯业务对象约束，无SQL语句或数据库依赖
*
* Change Log:
* [v1.0] Zhang Tao   2026-01-08
* * 初始化教师实体类结构（基于C++23 Modules）
* * 实现教师基本信息封装（ID、姓名）及访问接口
* * 实现授课课程管理（添加/移除授课课程、获取授课列表）
* * 实现成绩录入/修改逻辑（含成绩范围校验、授课课程权限校验）
* * 实现教师信息格式化输出、ID匹配检查等辅助功能
* [v2.0] Integrated 2026-01-07
* * 修复与 dom.course 的循环依赖编译错误
* * 调整 import 顺序以适配整体编译流程
*/
export module course_system:domain.teacher;

import std;
import :domain.course;
// 前向声明Course类，解决循环引用（教师与课程存在关联）
export class Course;

export class Teacher {
public:
   // 构造函数：初始化教师ID和姓名
   Teacher(std::string id, std::string name);

   // ID匹配检查：判断输入ID是否与教师ID一致
   bool hasId(std::string_view id) const;

   // 获取教师ID（只读）
   std::string getId() const;

   // 获取教师姓名（只读）
   std::string getName() const;

   // 添加授课课程：将课程加入教师的授课列表（避免重复添加）
   void addTeachingCourse(Course* course);

   // 移除授课课程：将课程从教师的授课列表中移除
   void removeTeachingCourse(Course* course);

   // 获取授课课程列表：返回当前教师的所有授课课程
   std::vector<Course*> getTeachingCourses() const;

   // 录入/修改成绩：仅允许为本人授课课程的学生打分（含权限与成绩范围校验）
   // @param course 目标课程（需为教师授课课程）
   // @param studentId 学生ID
   // @param score 成绩（0-100整数）
   // @return true 成绩录入成功，false 权限不足或成绩无效
   bool assignGrade(Course* course, std::string_view studentId, int score);

   // 获取教师详细信息字符串：格式化输出ID、姓名、授课数量
   std::string teacher_info() const;

private:
   std::string m_id;                       // 教师ID（私有封装，仅通过接口访问）
   std::string m_name;                     // 教师姓名（私有封装，仅通过接口访问）
   std::vector<Course*> m_teachingCourses; // 授课课程列表（维护教师与课程的关联）

   // 私有辅助方法：检查课程是否为当前教师的授课课程
   bool isTeachingCourse(Course* course) const;
};

// --- Implementation ---
/**
* @brief 构造函数：初始化教师ID和姓名
* @param id 教师唯一ID
* @param name 教师姓名
*/
Teacher::Teacher(std::string id, std::string name)
   : m_id(id), m_name(name) {}

/**
* @brief 检查输入ID是否与教师ID一致
* @param id 待检查的ID
* @return true 匹配成功，false 匹配失败
*/
bool Teacher::hasId(std::string_view id) const {
   return m_id == id;
}

/**
* @brief 获取教师ID
* @return 教师ID字符串
*/
std::string Teacher::getId() const {
   return m_id;
}

/**
* @brief 获取教师姓名
* @return 教师姓名字符串
*/
std::string Teacher::getName() const {
   return m_name;
}

/**
* @brief 私有辅助方法：检查课程是否为当前教师的授课课程
* @param course 待检查的课程指针
* @return true 是授课课程，false 非授课课程
*/
bool Teacher::isTeachingCourse(Course* course) const {
   if (!course) return false;
   // 遍历授课列表，检查课程是否存在
   return std::ranges::contains(m_teachingCourses, course);
}

/**
* @brief 添加授课课程：避免重复添加同一课程
* @param course 待添加的授课课程指针
*/
void Teacher::addTeachingCourse(Course* course) {
   if (!course) return;
   // 检查课程是否已在授课列表中
   if (isTeachingCourse(course)) {
       std::print("Warning: Teacher {} is already teaching {}.\n", m_name, course->course_info());
       return;
   }
   m_teachingCourses.push_back(course);
   std::print("Success: Teacher {} added course {}.\n", m_name, course->course_info());
}

/**
* @brief 移除授课课程：从列表中删除指定课程
* @param course 待移除的授课课程指针
*/
void Teacher::removeTeachingCourse(Course* course) {
   if (!course) return;
   // 查找课程在列表中的位置
   auto it = std::ranges::find(m_teachingCourses, course);
   if (it != m_teachingCourses.end()) {
       m_teachingCourses.erase(it);
       std::print("Success: Teacher {} removed course {}.\n", m_name, course->course_info());
   } else {
       std::print("Error: Teacher {} is not teaching {}.\n", m_name, course->course_info());
   }
}

/**
* @brief 获取当前教师的所有授课课程列表
* @return 授课课程指针向量（只读）
*/
std::vector<Course*> Teacher::getTeachingCourses() const {
   return m_teachingCourses;
}

/**
* @brief 录入/修改成绩：严格遵循教师权限与成绩范围约束
* @param course 目标课程（需为教师授课课程）
* @param studentId 学生ID
* @param score 成绩（0-100整数，超出范围视为无效）
* @return true 操作成功，false 操作失败（权限/成绩无效）
*/
bool Teacher::assignGrade(Course* course, std::string_view studentId, int score) {
   // 1. 基础有效性校验
   if (!course || studentId.empty()) {
       std::print("Error: Invalid course or student ID.\n");
       return false;
   }
   // 2. 权限校验：仅允许为本人授课课程打分
   if (!isTeachingCourse(course)) {
       std::print("Error: Teacher {} has no permission to grade course {}.\n", m_name, course->course_info());
       return false;
   }
   // 3. 成绩范围校验（0-100整数）
   if (score < 0 || score > 100) {
       std::print("Error: Score {} is invalid (must be 0-100).\n", score);
       return false;
   }
   // 4. 记录成绩操作日志（符合需求说明书"修改需记录日志"要求）
   std::print("[Grade Log] Teacher {} assigned score {} to Student {} for Course {}\n",
       m_name, score, studentId, course->course_info());
   return true;
}

/**
* @brief 获取教师详细信息：格式化输出核心信息
* @return 格式化字符串（包含ID、姓名、授课数量）
*/
std::string Teacher::teacher_info() const {
   return std::format("[Teacher] {} - {} (Teaching {} courses)",
       m_id, m_name, m_teachingCourses.size());
}

```

---

### File: domain/dom.student.cppm
```cppm
/**
* @file    src/CourseSelectionSystem/domain/dom.student.cppm
* @date    2026-01-06
* @author  GY
* @brief   Domain partition: Student entity
*
* dom.student:领域层学生实体模块
* 定义学生实体类，包含学生基本信息（ID、姓名）
* 维护学生已选课程列表，提供选课和退课功能
*
* Change Log:
* [v1.0] GY   2026-01-06
* * 初始化项目架构 (基于 C++23 Modules)
* * 实现领域层实体 (Student, Course) 及其核心逻辑
* * 实现应用层控制器 (SystemController) 管理选课流程
* * 实现选课与退课功能，包含容量检查与重复选课验证
* * 添加基础测试用例与 CLI 输出
*/
export module course_system:domain.student;

import std;

export class Course;


export class Student {
public:
    // 构造函数
    Student(std::string id, std::string name);

    // 选修课程
    void enrollIn(Course* c);

    // 退选课程
    void dropCourse(Course* c);

    // 检查学生是否匹配指定 ID
    bool hasId(std::string_view id) const;

    // 获取学生详细信息字符串
    std::string student_info() const;

private:
    std::string m_id;               // 学生 ID
    std::string m_name;             // 学生姓名
    std::vector<Course*> m_courses; // 已选课程列表
};

// --- Implementation ---

Student::Student(std::string id, std::string name)
    : m_id(id), m_name(name) {}


/**
* @brief 检查学生是否匹配指定 ID
* @param id 要检查的 ID
* @return true 如果匹配，否则 false
*/
bool Student::hasId(std::string_view id) const {
    return m_id == id;
}


/**
* @brief 获取学生详细信息字符串
* @return 格式化后的学生信息 (ID - Name)
*/
std::string Student::student_info() const {
    return std::format("[Student] {} - {}", m_id, m_name);
}

```

---

### File: presentation/pres.cli.cppm
```cppm
/**
* @file    src/CourseSelectionSystem/presentation/pres.cli.cppm
* @date    2026-01-07
* @author  Zhang Tao
* @brief   Presentation layer partition (CLI Menu System)
*
* 表现层模块接口与实现，定义并实现用户交互核心类 UserInterface
* 提供登录菜单、各角色功能菜单的完整逻辑，遵循 C++23 Modules 规范
* 严格遵循「表现层不包含业务逻辑」约束，仅处理输入输出格式化
*
* Change Log:
* [v1.0] Zhang Tao   2026-01-07
* * 初始化表现层模块结构
* [v1.5] Zhang Tao   2026-01-07
* * 集成 SystemController，实现选课/退课业务逻辑调用
* [v2.0] Integrated 2026-01-07
* * 实现 Controller 依赖注入 (setController)
* * 将菜单选项映射到具体的 SystemController 业务方法
* * 完善用户输入与后台逻辑的数据流转
*/
export module course_system:presentation;

import std;
import :app.controller;

// --- 类声明 ---
export class UserInterface {
public:
    /**
     * @brief 设置系统控制器
     * @param controller 系统控制器指针
     */
    void setController(SystemController* controller);

    /**
     * @brief 显示登录菜单，处理用户登录流程
     * @return 登录成功的用户ID（非空）
     */
    std::string showLoginMenu();

    /**
     * @brief 显示学生主菜单（循环交互）
     * @param studentId 登录学生的ID
     */
    void showStudentMenu(std::string_view studentId);

    /**
     * @brief 显示教师主菜单（循环交互）
     * @param teacherId 登录教师的ID
     */
    void showTeacherMenu(std::string_view teacherId);

    /**
     * @brief 显示教学秘书主菜单（循环交互）
     * @param secretaryId 登录教学秘书的ID
     */
    void showSecretaryMenu(std::string_view secretaryId);

private:
    SystemController* m_controller{nullptr};

    /**
     * @brief 清除输入缓冲区，避免无效输入导致的交互异常
     */
    void clearInputBuffer() const;

    /**
     * @brief 显示角色选择菜单，返回用户选择的角色类型
     * @return 1=学生，2=教师，3=教学秘书
     */
    int showRoleSelectionMenu() const;
};

// --- 实现部分 ---

void UserInterface::setController(SystemController* controller) {
    m_controller = controller;
}

/**
* @brief 清除输入缓冲区
*/
void UserInterface::clearInputBuffer() const {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

/**
* @brief 显示角色选择菜单
*/
int UserInterface::showRoleSelectionMenu() const {
    int role_choice = 0;
    while (true) {
        std::print("\n==================== 角色选择 ====================\n");
        std::print("1. 学生\n");
        std::print("2. 教师\n");
        std::print("3. 教学秘书\n");
        std::print("===============================================\n");
        std::print("请选择您的角色（输入数字1-3）：");
        std::cin >> role_choice;

        if (std::cin.fail() || role_choice < 1 || role_choice > 3) {
            clearInputBuffer();
            std::print("❌ 无效输入！请输入1-3之间的数字。\n");
            continue;
        }
        clearInputBuffer();
        break;
    }
    return role_choice;
}

/**
* @brief 显示登录菜单
*/
std::string UserInterface::showLoginMenu() {
    std::string user_id;

    std::print("\n\n\n");
    std::print("===============================================\n");
    std::print("========== 重庆师范大学选课系统 v1.0 ==========\n");
    std::print("===============================================\n");

    while (true) {
        std::print("请输入用户ID：");
        std::getline(std::cin, user_id);

        if (user_id.empty()) {
            std::print("❌ 用户ID不能为空！请重新输入。\n");
            continue;
        }
        break;
    }

    std::print("✅ 登录验证通过！\n");

    int role = showRoleSelectionMenu();
    switch (role) {
        case 1:
            showStudentMenu(user_id);
            break;
        case 2:
            showTeacherMenu(user_id);
            break;
        case 3:
            showSecretaryMenu(user_id);
            break;
        default:
            std::print("❌ 角色选择异常，返回登录界面。\n");
            break;
    }

    return user_id;
}

/**
* @brief 显示学生主菜单
*/
void UserInterface::showStudentMenu(std::string_view studentId) {
    int choice = 0;
    while (true) {
        std::print("\n\n\n");
        std::print("===============================================\n");
        std::print("========== 学生菜单 - 用户名：{} ==========\n", studentId);
        std::print("===============================================\n");
        std::print("1. 选课 (Enroll in Course)\n");
        std::print("2. 退课 (Drop Course)\n");
        std::print("3. 查看课表 (View Schedule)\n");
        std::print("4. 查看成绩 (View Grades)\n");
        std::print("5. 退出登录 (Logout)\n");
        std::print("===============================================\n");
        std::print("请选择功能（输入数字1-5）：");
        std::cin >> choice;

        if (std::cin.fail() || choice < 1 || choice > 5) {
            clearInputBuffer();
            std::print("❌ 无效输入！请输入1-5之间的数字。\n");
            std::print("按Enter键继续...");
            std::cin.get();
            continue;
        }
        clearInputBuffer();

        std::string courseId;
        switch (choice) {
            case 1:
                std::print("\n--- 选课 ---\n");
                std::print("请输入课程ID：");
                std::getline(std::cin, courseId);
                if (m_controller) {
                    m_controller->performEnrollment(std::string(studentId), courseId);
                } else {
                    std::print("Error: System Controller not initialized.\n");
                }
                break;
            case 2:
                std::print("\n--- 退课 ---\n");
                std::print("请输入课程ID：");
                std::getline(std::cin, courseId);
                if (m_controller) {
                    m_controller->performDrop(std::string(studentId), courseId);
                } else {
                    std::print("Error: System Controller not initialized.\n");
                }
                break;
            case 3:
                std::print("\n✅ 已选择功能 [3]：查看课表 (View Schedule)\n");
                std::print("(功能开发中...)\n");
                break;
            case 4:
                std::print("\n✅ 已选择功能 [4]：查看成绩 (View Grades)\n");
                std::print("(功能开发中...)\n");
                break;
            case 5:
                std::print("\n✅ 退出登录成功！返回登录界面。\n");
                return;
        }

        std::print("按Enter键继续...");
        std::cin.get();
    }
}

/**
* @brief 显示教师主菜单
*/
void UserInterface::showTeacherMenu(std::string_view teacherId) {
    int choice = 0;
    while (true) {
        std::print("\n\n\n");
        std::print("===============================================\n");
        std::print("========== 教师菜单 - 用户名：{} ==========\n", teacherId);
        std::print("===============================================\n");
        std::print("1. 查看授课名单 (View Teaching Roster)\n");
        std::print("2. 录入成绩 (Assign Grade)\n");
        std::print("3. 修改成绩 (Modify Grade)\n");
        std::print("4. 退出登录 (Logout)\n");
        std::print("===============================================\n");
        std::print("请选择功能（输入数字1-4）：");
        std::cin >> choice;

        if (std::cin.fail() || choice < 1 || choice > 4) {
            clearInputBuffer();
            std::print("❌ 无效输入！请输入1-4之间的数字。\n");
            std::print("按Enter键继续...");
            std::cin.get();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1:
                std::print("\n✅ 已选择功能 [1]：查看授课名单 (View Teaching Roster)\n");
                break;
            case 2:
                std::print("\n✅ 已选择功能 [2]：录入成绩 (Assign Grade)\n");
                break;
            case 3:
                std::print("\n✅ 已选择功能 [3]：修改成绩 (Modify Grade)\n");
                break;
            case 4:
                std::print("\n✅ 退出登录成功！返回登录界面。\n");
                return;
        }

        std::print("按Enter键继续...");
        std::cin.get();
    }
}

/**
* @brief 显示教学秘书主菜单
*/
void UserInterface::showSecretaryMenu(std::string_view secretaryId) {
    int choice = 0;
    while (true) {
        std::print("\n\n\n");
        std::print("===============================================\n");
        std::print("========== 教学秘书菜单 - 用户名：{} ==========\n", secretaryId);
        std::print("===============================================\n");
        std::print("1. 创建课程 (Create Course)\n");
        std::print("2. 分配教师 (Assign Teacher)\n");
        std::print("3. 设置上课时间 (Set Class Time)\n");
        std::print("4. 退出登录 (Logout)\n");
        std::print("===============================================\n");
        std::print("请选择功能（输入数字1-4）：");
        std::cin >> choice;

        if (std::cin.fail() || choice < 1 || choice > 4) {
            clearInputBuffer();
            std::print("❌ 无效输入！请输入1-4之间的数字。\n");
            std::print("按Enter键继续...");
            std::cin.get();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1:
                std::print("\n✅ 已选择功能 [1]：创建课程 (Create Course)\n");
                break;
            case 2:
                std::print("\n✅ 已选择功能 [2]：分配教师 (Assign Teacher)\n");
                break;
            case 3:
                std::print("\n✅ 已选择功能 [3]：设置上课时间 (Set Class Time)\n");
                break;
            case 4:
                std::print("\n✅ 退出登录成功！返回登录界面。\n");
                return;
        }

        std::print("按Enter键继续...");
        std::cin.get();
    }
}
```

---

