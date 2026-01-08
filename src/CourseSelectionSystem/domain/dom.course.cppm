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
    Course(std::string id, std::string name, int capacity = 60, int credit = 2, std::string teacherId = "", Timeslot timeslot = {0, 0});

    // 获取课程名称
    std::string getName() const { return m_name; }

    // 获取课程容量
    int getCapacity() const { return m_capacity; }

    // 获取学分
    int getCredit() const { return m_credit; }

    // 获取教师ID
    std::string getTeacherId() const { return m_teacher_id; }

    // 获取上课时间
    Timeslot getTimeslot() const { return m_timeslot; }

    // 检查课程是否已满
    bool isFull() const;

    // 尝试接受学生报名
    bool acceptEnrollment(Student* s);

    // 移除学生报名信息
    void removeEnrollment(Student* s);

    // 设置当前已选人数 (供持久化层使用)
    void setEnrolled(int count) { m_currentEnrolled = count; }
    
    // 获取当前已选人数
    int getEnrolled() const { return m_currentEnrolled; }

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
    int m_credit;                     // 学分
    std::string m_teacher_id;         // 教师 ID
    int m_currentEnrolled = 0;        // 当前已选人数 (来自数据库或内存计数)
    Timeslot m_timeslot;              // 上课时间
    std::vector<Student*> m_students; // 已选修该课程的学生列表
};

// --- Implementation ---
Course::Course(std::string id, std::string name, int capacity, int credit, std::string teacherId, Timeslot timeslot)
    : m_id(id), m_name(name), m_capacity(capacity), m_credit(credit), m_teacher_id(teacherId), m_timeslot(timeslot), m_currentEnrolled(0) {}


/**
* @brief 检查课程是否已满
* @return true 如果当前选课人数 >= 容量，否则 false
*/
bool Course::isFull() const {
    // 考虑内存中的新增学生（这里简化处理，假设 m_currentEnrolled 已经是最新的，或者需要同步）
    // 为了简单起见，如果 m_students 不为空，我们假设它是内存中的权威列表（但这在 Proxy 加载模式下不一定对）
    // 稳妥的做法：max(m_students.size(), m_currentEnrolled) 
    // 但因为 m_students 可能只包含部分学生（如果只加载了一个学生），所以 m_currentEnrolled 应该是基准
    return m_currentEnrolled >= m_capacity;
}


/**
* @brief 尝试接受学生报名
* @param s 申请报名的学生指针
* @return true 报名成功, false 报名失败 (如课程已满)
*/
bool Course::acceptEnrollment(Student* s) {
    if (isFull()) return false;
    m_students.push_back(s);
    m_currentEnrolled++; // 更新计数
    return true;
}


/**
* @brief 移除学生报名信息
* @param s 要移除的学生指针
*/
void Course::removeEnrollment(Student* s) {
    auto it = std::find(m_students.begin(), m_students.end(), s);
    if (it != m_students.end()) {
        m_students.erase(it);
    }
    if (m_currentEnrolled > 0) m_currentEnrolled--;
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
