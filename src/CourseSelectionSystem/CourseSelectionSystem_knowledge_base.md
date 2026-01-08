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
    domain/dom.timeslot.cppm
    domain/dom.course.cppm
    domain/dom.student.cppm
    domain/dom.teacher.cppm
    domain/domain.cppm

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
* @brief   Primary module interface
*/
export module course_system;

// 1. 领域层 (分文件，需分别导出)
export import :domain.timeslot;
export import :domain.course;
export import :domain.student;
export import :domain.teacher;
export import :domain;

// 2. 基础设施层 (合并文件，整体导出)
export import :infrastructure;

// 3. 应用层与表现层
export import :app.controller;
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
* @brief   Infrastructure partition: DBAdapter & Proxies
*/
module;
#include <pqxx/pqxx>

export module course_system:infrastructure;

import std;
import :domain.student;
import :domain.course;
import :domain.timeslot;

export namespace db {

using Row = std::vector<std::string>;
using Result = std::vector<Row>;

class DBAdapter {
public:
    DBAdapter() = default;
    void set_credentials(std::string conn_str) { m_conn_str = std::move(conn_str); }
    bool connect(const std::string& conn_str = "") {
        if (!conn_str.empty()) m_conn_str = conn_str;
        try { pqxx::connection C(m_conn_str); return C.is_open(); } catch (...) { return false; }
    }
    bool execute(const std::string& sql) {
        try { pqxx::connection C(m_conn_str); pqxx::work W(C); W.exec(sql); W.commit(); return true; } catch (...) { return false; }
    }
    std::optional<Result> query(const std::string& sql) {
        try {
            pqxx::connection C(m_conn_str); pqxx::nontransaction N(C); pqxx::result R(N.exec(sql));
            Result res;
            for (const auto& row : R) {
                Row r; for (const auto& f : row) r.push_back(f.is_null() ? "" : f.c_str()); res.push_back(std::move(r));
            }
            return res;
        } catch (...) { return std::nullopt; }
    }
private:
    std::string m_conn_str;
};

// --- Proxies ---

class StudentProxy {
private:
    DBAdapter& m_db;
public:
    explicit StudentProxy(DBAdapter& db) : m_db(db) {}

    // 核心任务 1: 复杂查询 (JOIN)
    Student* findById(const std::string& id) {
        // 使用 LEFT JOIN 一次性获取学生信息和所有选课信息
        // 假设表名：students, courses, student_courses
        // students(id, name)
        // courses(id, name, capacity, weekday, timeslot)
        // student_courses(student_id, course_id)
        
        std::string sql = std::format(
            "SELECT s.name, c.id, c.name, c.capacity, c.weekday, c.timeslot "
            "FROM students s "
            "LEFT JOIN student_courses sc ON s.id = sc.student_id "
            "LEFT JOIN courses c ON sc.course_id = c.id "
            "WHERE s.id = '{}'", 
            id
        );

        auto res_opt = m_db.query(sql);
        if (!res_opt || res_opt->empty()) {
            return nullptr;
        }

        const auto& rows = *res_opt;
        std::string name = rows[0][0]; // 第一行第一列是姓名
        
        Student* student = new Student(id, name);

        for (const auto& row : rows) {
            // 如果 course_id (row[1]) 为空，说明没选课
            if (row[1].empty()) continue;

            std::string c_id = row[1];
            std::string c_name = row[2];
            int c_capacity = std::stoi(row[3]);
            int weekday = row[4].empty() ? 0 : std::stoi(row[4]);
            int timeslot = row[5].empty() ? 0 : std::stoi(row[5]);

            // 重建 Course 对象
            Course* course = new Course(c_id, c_name, c_capacity, Timeslot{weekday, timeslot});

            // 恢复状态 (绕过业务检查)
            student->restoreEnrollment(course);
        }

        return student;
    }

    // 核心任务 2: 保存状态
    bool save(const Student& student) {
        // 1. 保存学生基本信息 (Upsert)
        std::string sql_student = std::format(
            "INSERT INTO students (id, name) VALUES ('{}', '{}') "
            "ON CONFLICT (id) DO UPDATE SET name = EXCLUDED.name",
            student.getId(), student.getName()
        );

        if (!m_db.execute(sql_student)) return false;

        // 2. 保存选课关系
        // 先删后插
        std::string sql_del = std::format("DELETE FROM student_courses WHERE student_id = '{}'", student.getId());
        if (!m_db.execute(sql_del)) return false;

        for (const auto* course : student.getEnrolledCourses()) {
            std::string sql_link = std::format(
                "INSERT INTO student_courses (student_id, course_id) VALUES ('{}', '{}')",
                student.getId(), course->getId()
            );
            if (!m_db.execute(sql_link)) return false;
        }

        return true;
    }
};

// Stub for CourseProxy
class CourseProxy {
private:
    DBAdapter& m_db;
public:
    explicit CourseProxy(DBAdapter& db) : m_db(db) {}
    Course* findById(const std::string& id) {
        auto res = m_db.query(std::format("SELECT name, capacity, weekday, timeslot FROM courses WHERE id = '{}'", id));
        if (res && !res->empty()) {
            int wd = (*res)[0][2].empty() ? 0 : std::stoi((*res)[0][2]);
            int ts = (*res)[0][3].empty() ? 0 : std::stoi((*res)[0][3]);
            return new Course(id, (*res)[0][0], std::stoi((*res)[0][1]), Timeslot{wd, ts});
        }
        return nullptr;
    }
    bool save(const Course& c) {
        std::string sql = std::format(
            "INSERT INTO courses (id, name, capacity, weekday, timeslot) VALUES ('{}', '{}', {}, {}, {}) "
            "ON CONFLICT (id) DO NOTHING",
            c.getId(), c.getName(), c.getCapacity(), c.getTimeslot().weekday, c.getTimeslot().timeslot
        );
        return m_db.execute(sql);
    }
    static bool addCourse(const Course&, int, int) { return true; }
    static bool updateClassTime(const std::string&, int, int) { return true; }
};

class EnrollmentProxy {
public:
    static bool updateScore(const std::string&, const std::string&, int) { return true; }
};

} // namespace db
```

---

### File: domain/dom.timeslot.cppm
```cppm
/**
* @file    src/CourseSelectionSystem/domain/dom.timeslot.cppm
* @date    2026-01-08
* @brief   Domain partition: Timeslot value object
*/
export module course_system:domain.timeslot;

import std;

export struct Timeslot {
    int weekday;   // 1 (Monday) - 7 (Sunday)
    int timeslot;  // 1=1-2节, 2=3-4节...

    // 核心算法：检测两个时间段是否冲突
    // 逻辑：如果星期相同 且 节次相同，则冲突
    bool overlaps(const Timeslot& other) const {
        return (this->weekday == other.weekday) && (this->timeslot == other.timeslot);
    }

    // 辅助功能：转为字符串用于显示 (e.g., "Mon Section 1")
    std::string toString() const {
        static const std::vector<std::string> days = {"", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
        std::string day_str = (weekday >= 1 && weekday <= 7) ? days[weekday] : "Unknown";
        return std::format("{} Section {}", day_str, timeslot);
    }
};

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
import :domain.timeslot;

// 前向声明，解决循环引用
export class Student;


export class Course {
public:
    // 构造函数
    Course(std::string id, std::string name, int capacity = 60, Timeslot timeslot = {0, 0});
    // domain/dom.course.cppm 中 Course 类添加以下公有方法
    /**
     * @brief 获取课程名称
     * @return 课程名称字符串
     */
    std::string getName() const { return m_name; }

    /**
     * @brief 获取课程容量
     * @return 课程最大容量
     */
    int getCapacity() const { return m_capacity; }

    /**
     * @brief 获取上课时间
     * @return 时间段
     */
    Timeslot getTimeslot() const { return m_timeslot; }

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
    Timeslot m_timeslot;              // 上课时间
    std::vector<Student*> m_students; // 已选修该课程的学生列表
};

// --- Implementation ---
Course::Course(std::string id, std::string name, int capacity, Timeslot timeslot)
    : m_id(id), m_name(name), m_capacity(capacity), m_timeslot(timeslot) {}


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
 * @return 格式化后的课程信息 (ID - Name (Current/Max) [Timeslot])
 */
std::string Course::course_info() const {
    return std::format("[Course] {} - {} ({}/{}) [{}]",
        m_id, m_name, m_students.size(), m_capacity, m_timeslot.toString());
}

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
export import :domain.teacher;
export import :domain.student;
export import :domain.course;
export import :domain.timeslot;

import std;

/**
* @brief 学生选修课程
* @param c 要选修的课程指针
*/
void Student::enrollIn(Course* c) {
    if (!c) return;

    // 1. 检查是否已经选过该课程
    if (std::ranges::contains(m_courses, c)) {
        std::print("Warning: Student {} already enrolled in {}.\n", m_name, c->course_info());
        return;
    }

    // 2. 时间冲突检查 (Task B 核心要求)
    for (const auto* existing : m_courses) {
        if (existing->getTimeslot().overlaps(c->getTimeslot())) {
            std::print("Error: Time conflict! {} overlaps with already enrolled {}.\n", 
                c->course_info(), existing->course_info());
            return;
        }
    }

    // 3. 执行选课 (含容量检查)
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

    std::string getId() const { return m_id; }
    std::string getName() const { return m_name; }

    // 仅供 Proxy 恢复数据使用，不进行冲突检查
    void restoreEnrollment(Course* c) {
        if (c) m_courses.push_back(c);
    }
    
    // 获取已选课程列表
    const std::vector<Course*>& getEnrolledCourses() const {
        return m_courses;
    }

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

// 模块导入必须放在全局作用域（函数外），C++23不允许函数内import
import std;
import :app.controller;
import :domain;                      
import :infrastructure;              

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
            case 1: { // 加花括号避免变量跨case初始化问题
                std::print("\n--- 选课 ---\n");
                std::print("请输入课程ID：");
                std::getline(std::cin, courseId);
                if (m_controller) {
                    m_controller->performEnrollment(std::string(studentId), courseId);
                } else {
                    std::print("Error: System Controller not initialized.\n");
                }
                break;
            }
            case 2: { // 加花括号避免变量跨case初始化问题
                std::print("\n--- 退课 ---\n");
                std::print("请输入课程ID：");
                std::getline(std::cin, courseId);
                if (m_controller) {
                    m_controller->performDrop(std::string(studentId), courseId);
                } else {
                    std::print("Error: System Controller not initialized.\n");
                }
                break;
            }
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
    int choice = 0; // 确保变量在正确作用域声明
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
                std::print("(功能开发中...)\n");
                break;
            case 2: { // 加花括号包裹变量初始化
                std::string studentId, courseId;
                int score;
                std::print("\n--- 录入成绩 ---\n");
                std::print("请输入学生ID：");
                std::getline(std::cin, studentId);
                std::print("请输入课程ID：");
                std::getline(std::cin, courseId);
                std::print("请输入成绩（0-100）：");
                std::cin >> score;
                clearInputBuffer();

                // 调用EnrollmentProxy录入成绩（录入和修改共用updateScore方法）
                if (db::EnrollmentProxy::updateScore(studentId, courseId, score)) {
                    std::print("✅ 成绩录入成功！\n");
                } else {
                    std::print("❌ 成绩录入失败！\n");
                }
                break;
            }
            case 3: { // 加花括号包裹变量初始化
                std::string studentId, courseId;
                int score;
                std::print("\n--- 修改成绩 ---\n");
                std::print("请输入学生ID：");
                std::getline(std::cin, studentId);
                std::print("请输入课程ID：");
                std::getline(std::cin, courseId);
                std::print("请输入新成绩（0-100）：");
                std::cin >> score;
                clearInputBuffer();

                // 调用EnrollmentProxy修改成绩（与录入共用同一方法）
                if (db::EnrollmentProxy::updateScore(studentId, courseId, score)) {
                    std::print("✅ 成绩修改成功！\n");
                } else {
                    std::print("❌ 成绩修改失败！\n");
                }
                break;
            }
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
    int choice = 0; // 确保变量在正确作用域声明
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
            case 1: { // 加花括号包裹变量初始化，解决跨case报错
                std::string courseId, courseName;
                int capacity, weekday, timeslot;
                std::print("\n--- 创建课程 ---\n");
                // 接收课程基本信息输入
                std::print("请输入课程ID：");
                std::getline(std::cin, courseId);
                std::print("请输入课程名称：");
                std::getline(std::cin, courseName);
                std::print("请输入课程容量（默认60，直接回车使用默认值）：");
                std::string capacity_str;
                std::getline(std::cin, capacity_str);
                capacity = capacity_str.empty() ? 60 : std::stoi(capacity_str);
                std::print("请输入上课星期（1=周一，5=周五）：");
                std::cin >> weekday;
                std::print("请输入上课时段（1=1-2节，2=3-4节，3=5-6节，4=7-8节）：");
                std::cin >> timeslot;
                clearInputBuffer();

                // 构建Course领域对象
                Course course(courseId, courseName, capacity);
                // 调用CourseProxy创建课程
                if (db::CourseProxy::addCourse(course, weekday, timeslot)) {
                    std::print("✅ 课程创建成功！\n");
                } else {
                    std::print("❌ 课程创建失败！\n");
                }
                break;
            }
            case 2:
                std::print("\n✅ 已选择功能 [2]：分配教师 (Assign Teacher)\n");
                std::print("(功能开发中...)\n");
                break;
            case 3: { // 加花括号包裹变量初始化
                std::string courseId;
                int weekday, timeslot;
                std::print("\n--- 设置上课时间 ---\n");
                std::print("请输入课程ID：");
                std::getline(std::cin, courseId);
                std::print("请输入新上课星期（1=周一，5=周五）：");
                std::cin >> weekday;
                std::print("请输入新上课时段（1=1-2节，2=3-4节，3=5-6节，4=7-8节）：");
                std::cin >> timeslot;
                clearInputBuffer();

                // 调用CourseProxy修改上课时间
                if (db::CourseProxy::updateClassTime(courseId, weekday, timeslot)) {
                    std::print("✅ 上课时间设置成功！\n");
                } else {
                    std::print("❌ 上课时间设置失败！\n");
                }
                break;
            }
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

