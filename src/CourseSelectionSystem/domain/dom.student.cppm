/**
* @file    src/CourseSelectionSystem/domain/dom.student.cppm
* @date    2026-01-06
* @author  GY
* @brief   领域层分区：学生实体
*
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
* [v2.0] GY   2026-01-10
* * 增加 hasTimeConflict 方法
* * 优化 enrollIn/dropCourse 逻辑以配合 Proxy
*/

export module domain:student;

import std;
import :course;

export class Student {
public:
    // 构造函数
    Student(std::string id, std::string name);

    // 选修课程 (仅更新内存状态，不持久化)
    void enrollIn(Course* c);

    // 退选课程 (仅更新内存状态，不持久化)
    void dropCourse(Course* c);

    // 检查是否有时间冲突
    bool hasTimeConflict(const Course* targetCourse) const;

    // 检查是否已选修
    bool isEnrolled(const Course* targetCourse) const;

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
* @brief 检查是否已选修
*/
bool Student::isEnrolled(const Course* targetCourse) const {
    if (!targetCourse) return false;
    // 比较指针或ID
    return std::ranges::any_of(m_courses, [targetCourse](Course* c) {
        return c->getId() == targetCourse->getId();
    });
}

/**
* @brief 检查是否有时间冲突
*/
bool Student::hasTimeConflict(const Course* targetCourse) const {
    if (!targetCourse) return false;

    for (const auto* enrolledCourse : m_courses) {
        if (enrolledCourse->getTimeslot().overlaps(targetCourse->getTimeslot())) {
            std::print("Conflict detected: {} overlaps with \n", 
                targetCourse->course_info(), enrolledCourse->course_info());
            return true;
        }
    }
    return false;
}

/**
* @brief 学生选修课程
*/
void Student::enrollIn(Course* c) {
    if (!c) return;
    if (isEnrolled(c)) return; 
    // 注意：这里不再进行 acceptEnrollment 检查，因为那是 Controller/Proxy 协调的。
    // Student 对象只负责维护自己的关系。
    m_courses.push_back(c);
}

/**
* @brief 学生退选课程
*/
void Student::dropCourse(Course* c) {
    if (!c) return;
    // 使用 ID 匹配移除，防止指针不同但 ID 相同的情况
    std::erase_if(m_courses, [c](Course* enrolled) {
        return enrolled->getId() == c->getId();
    });
}

/**
* @brief 检查学生是否匹配指定 ID
*/
bool Student::hasId(std::string_view id) const {
    return m_id == id;
}

/**
* @brief 获取学生详细信息字符串
*/
std::string Student::student_info() const {
    return std::format("[Student] {} - {} ({} courses)", m_id, m_name, m_courses.size());
}
