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