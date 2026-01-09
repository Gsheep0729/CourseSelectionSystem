/**
* @file    src/CourseSelectionSystem/domain/domain.cppm
* @date    2026-01-06
* @author  GY
* @brief   领域层主模块接口
*
* 聚合学生（Student）和课程（Course）实体模块
* 补全 Student 对 Course 的调用逻辑（选课、退课）
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
export module domain;
export import :teacher;
export import :student;
export import :course;
export import :timeslot;

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
