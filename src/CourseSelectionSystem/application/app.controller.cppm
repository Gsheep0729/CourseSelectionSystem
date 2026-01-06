/**
* @file    src/CourseSelectionSystem/application/app.controller.cppm
* @date    2026-01-06
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
* * 实现领域层实体 (Student, Course) 及其核心逻辑
* * 实现应用层控制器 (SystemController) 管理选课流程
* * 实现选课与退课功能，包含容量检查与重复选课验证
* * 添加基础测试用例与 CLI 输出
*/
module;
#include <vector>
#include <string>
#include <print>
export module course_system:app.controller;

import :domain;
// import std;

export class SystemController {
public:
    // 初始化系统
    void initialize();

    // 运行系统
    void run();

    // 执行选课操作
    void performEnrollment(std::string sid, std::string cid);

    // 执行退课操作
    void performDrop(std::string sid, std::string cid);

    ~SystemController() {
        std::print("~SystemController\n");
        for (auto s : m_students) delete s;
        for (auto c : m_courses) delete c;
    }

private:
    std::vector<Student*> m_students; // 学生列表
    std::vector<Course*> m_courses;   // 课程列表

    // 根据ID查找学生
    Student* findStudent(std::string_view id);

    // 根据ID查找课程
    Course* findCourse(std::string_view id);
};

// --- Implementation ---

/**
* @brief 初始化系统
* 创建初始的学生和课程数据
*/
void SystemController::initialize() {
    std::print("System Initializing Mock Data...\n");

    // 1. 初始化学生 (Real Data)
    m_students.push_back(new Student("2024051604085", "Gao Yang"));

    // 2. 初始化课程 (Real Data from course_data.dat)
    m_courses.push_back(new Course("C0001", "高等数学"));
    m_courses.push_back(new Course("C0002", "马克思主义基本原理"));
    m_courses.push_back(new Course("C0003", "软件构建与实现"));
    m_courses.push_back(new Course("C0004", "计算机网络"));
    m_courses.push_back(new Course("C0005", "计算机导论"));
    m_courses.push_back(new Course("C0006", "线性代数"));
    m_courses.push_back(new Course("C0007", "概率论与数理统计"));
    m_courses.push_back(new Course("C0008", "数据结构"));
    m_courses.push_back(new Course("C0009", "数据库原理及应用"));
    m_courses.push_back(new Course("C0010", "思想道德与法治"));
    m_courses.push_back(new Course("C0011", "形势与政策"));
    m_courses.push_back(new Course("C0012", "大学英语"));
    m_courses.push_back(new Course("C0013", "大学生心理健康教育"));
    m_courses.push_back(new Course("C0014", "体育"));
    m_courses.push_back(new Course("C0015", "中国近现代史纲要"));
    m_courses.push_back(new Course("C0016", "Linux程序设计"));
    m_courses.push_back(new Course("C0017", "C语言程序设计"));

    std::print("Loaded {} students and {} courses.\n", m_students.size(), m_courses.size());
}


/**
* @brief 运行系统
* 启动系统并执行选课操作
*/
void SystemController::run() {
    std::print("System Started.\n");
    std::string sid = "2024051604085";

    // 1. 初始选课
    std::print("\n=== Enrollment Tests ===\n");
    performEnrollment(sid, "C0017"); // C语言
    performEnrollment(sid, "C0001"); // 高数
    performEnrollment(sid, "C0008"); // 数据结构

    // 2. 尝试退课
    std::print("\n=== Drop Tests ===\n");
    performDrop(sid, "C0001"); // 退选高数

    // 3. 再次选课
    std::print("\n=== Re-Enrollment Tests ===\n");
    performEnrollment(sid, "C0016"); // Linux程序设计

    // 4. 尝试退掉未选的课
    std::print("\n=== Invalid Drop Tests ===\n");
    performDrop(sid, "C0005"); // 计算机导论 (未选)
}


/**
* @brief 执行选课操作
* @param sid 学生ID
* @param cid 课程ID
*/
void SystemController::performEnrollment(std::string sid, std::string cid) {
    auto s = findStudent(sid);
    auto c = findCourse(cid);

    if (s && c) {
        s->enrollIn(c);
    } else {
        std::print("Error: Student ({}) or Course ({}) not found.\n", sid, cid);
    }
}

/**
* @brief 执行退课操作
* @param sid 学生ID
* @param cid 课程ID
*/
void SystemController::performDrop(std::string sid, std::string cid) {
    auto s = findStudent(sid);
    auto c = findCourse(cid);

    if (s && c) {
        s->dropCourse(c);
    } else {
        std::print("Error: Student ({}) or Course ({}) not found.\n", sid, cid);
    }
}


/**
* @brief 根据ID查找学生
* @param id 学生ID
* @return 找到的学生指针，未找到返回 nullptr
*/
Student* SystemController::findStudent(std::string_view id) {
    for (auto s : m_students) {
        if (s->hasId(id)) return s;
    }
    return nullptr;
}


/**
* @brief 根据ID查找课程
* @param id 课程ID
* @return 找到的课程指针，未找到返回 nullptr
*/
Course* SystemController::findCourse(std::string_view id) {
    for (auto c : m_courses) {
        if (c->hasId(id)) return c;
    }
    return nullptr;
}
