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
    // 构造函数
    SystemController();

    // 初始化系统 (连接数据库, 建表, 初始化数据)
    void initialize();

    // 运行系统
    void run();

    // --- 学生功能 ---
    void performEnrollment(std::string sid, std::string cid);
    void performDrop(std::string sid, std::string cid);

    // --- 教师功能 ---
    bool updateStudentScore(std::string sid, std::string cid, int score);

    // --- 教学秘书功能 ---
    bool addNewCourse(const Course& course, int weekday, int timeslot);
    bool setCourseTime(std::string cid, int weekday, int timeslot);

private:
    db::DBAdapter m_db;              // 数据库适配器
    db::StudentProxy m_studentProxy; // 学生数据代理
    db::CourseProxy m_courseProxy;   // 课程数据代理
    db::EnrollmentProxy m_enrollmentProxy; // 选课/成绩代理
};

// --- Implementation ---

SystemController::SystemController() 
    : m_studentProxy(m_db), m_courseProxy(m_db), m_enrollmentProxy(m_db) {
}

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

    // 重置数据库
    m_db.execute("DROP TABLE IF EXISTS student_courses");
    m_db.execute("DROP TABLE IF EXISTS enrollment"); // cleanup old
    m_db.execute("DROP TABLE IF EXISTS courses");
    m_db.execute("DROP TABLE IF EXISTS course"); // cleanup old
    m_db.execute("DROP TABLE IF EXISTS students");
    m_db.execute("DROP TABLE IF EXISTS student"); // cleanup old
    m_db.execute("DROP TABLE IF EXISTS users");
    
    // 创建表
    m_db.execute("CREATE TABLE students (id TEXT PRIMARY KEY, name TEXT, password TEXT DEFAULT '123456')");
    // 包含 credit, teacher_id, weekday (1-5), timeslot (1-4)
    m_db.execute("CREATE TABLE courses (id TEXT PRIMARY KEY, name TEXT, capacity INT DEFAULT 60, credit INT DEFAULT 2, teacher_id TEXT, weekday INT, timeslot INT)");
    // 包含 score
    m_db.execute("CREATE TABLE student_courses (student_id TEXT, course_id TEXT, score INT DEFAULT -1, PRIMARY KEY (student_id, course_id))");
    // 用户表 (角色管理)
    m_db.execute("CREATE TABLE users (id TEXT PRIMARY KEY, password TEXT, role TEXT)");

    std::print("Database initialized.\n");

    // 加载初始数据 - Student
    std::string s_id = "2024051604085";
    std::string s_name = "Gao Yang"; 
    Student s(s_id, s_name);
    m_studentProxy.save(s);

    // 加载初始数据 - Courses
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
    int wd = 1, ts = 1;
    for (const auto& [id, name] : manual_courses) {
        // 简单分配时间、学分、教师
        Course c(id, name, 60, 3, "T001", Timeslot{wd, ts});
        if (m_courseProxy.save(c)) {
            course_count++;
        }
        ts++;
        if (ts > 4) { ts = 1; wd++; }
        if (wd > 5) wd = 1;
    }
    
    // 初始化一些用户
    m_db.execute("INSERT INTO users (id, password, role) VALUES ('admin', 'admin', 'admin')");
    m_db.execute(std::format("INSERT INTO users (id, password, role) VALUES ('{}', '123456', 'student')", s_id));
    
    std::print("Loaded 1 student and {} courses via Proxies.\n", course_count);
}


/**
* @brief 运行系统
*/
void SystemController::run() {
    // 保留为空，逻辑移交 CLI
    std::print("System Controller Ready.\n");
}

// --- 教师功能实现 ---
bool SystemController::updateStudentScore(std::string sid, std::string cid, int score) {
    return m_enrollmentProxy.updateScore(sid, cid, score);
}

// --- 教学秘书功能实现 ---
bool SystemController::addNewCourse(const Course& course, int weekday, int timeslot) {
    return m_courseProxy.addCourse(course, weekday, timeslot);
}

bool SystemController::setCourseTime(std::string cid, int weekday, int timeslot) {
    return m_courseProxy.updateClassTime(cid, weekday, timeslot);
}

/**
* @brief 执行选课操作
*/
void SystemController::performEnrollment(std::string sid, std::string cid) {
    // 1. 加载聚合根 (Student)
    auto student = m_studentProxy.findById(sid);
    if (!student) {
        std::print("Error: Student {} not found.\n", sid);
        return;
    }

    // 2. 加载课程 (Course)
    auto course = m_courseProxy.findById(cid);
    if (!course) {
        std::print("Error: Course {} not found.\n", cid);
        delete student;
        return;
    }

    // 3. 业务检查与操作
    // 检查容量
    if (course->isFull()) {
        std::print("Error: Course {} is full.\n", course->getName());
        delete student; delete course;
        return;
    }

    // 检查重复选课
    bool alreadyEnrolled = false;
    for (auto* c : student->getEnrolledCourses()) {
        if (c->getId() == cid) {
            alreadyEnrolled = true;
            break;
        }
    }
    if (alreadyEnrolled) {
        std::print("Error: Student {} is already enrolled in [Course] {} - {}\n", 
              sid, course->getId(), course->getName());
        delete student; delete course;
        return;
    }

    // 执行选课
    student->enrollIn(course);

    // 4. 持久化
    if (m_studentProxy.save(*student)) {
        // 更新内存中的显示计数
        course->setEnrolled(course->getEnrolled() + 1);
        std::print("Success: Student {} enrolled in [Course] {} - {} ({}/{})\n", 
              sid, course->getId(), course->getName(), course->getEnrolled(), course->getCapacity());
    } else {
        std::print("Error: Database failure during enrollment.\n");
    }

    delete student;
    delete course;
}

/**
* @brief 执行退课操作
*/
void SystemController::performDrop(std::string sid, std::string cid) {
    auto student = m_studentProxy.findById(sid);
    if (!student) {
        std::print("Error: Student {} not found.\n", sid);
        return;
    }

    // 查找学生是否已选该课
    Course* targetCourse = nullptr;
    for (auto* c : student->getEnrolledCourses()) {
        if (c->getId() == cid) {
            targetCourse = c;
            break;
        }
    }

    if (!targetCourse) {
        // 尝试单独加载课程以获取名称
        auto tempCourse = m_courseProxy.findById(cid);
        if (tempCourse) {
            std::print("Error: Student {} is not enrolled in [Course] {} - {}\n", 
                  sid, tempCourse->getId(), tempCourse->getName());
            delete tempCourse;
        } else {
            std::print("Error: Course {} not found.\n", cid);
        }
        delete student;
        return;
    }

    // 3. 业务操作
    student->dropCourse(targetCourse);

    // 4. 持久化
    if (m_studentProxy.save(*student)) {
        std::print("Success: Student {} dropped [Course] {} - {}\n", 
              sid, targetCourse->getId(), targetCourse->getName());
    } else {
        std::print("Error: Database failure during drop.\n");
    }

    delete student;
    delete targetCourse; 
}