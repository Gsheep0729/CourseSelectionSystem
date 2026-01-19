/**
* @file    src/CourseSelectionSystem/domain/dom.course.cppm
* @date    2026-01-06
* @author  GY
* @brief   领域层分区：课程实体
*
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
* [v2.0] GY   2026-01-10
* * 增加字段：credit, teacherId, teacherName, timeslot
* * 更新构造函数以支持完整信息
* [v5.6] GY   2026-01-19
* * 规范封装：移除所有 Getter 方法，实施“告知，而非询问”原则
* * 实现访问者模式 (transferData) 用于数据持久化
* * 分离函数声明与实现
*/
export module domain:course;

import std;
import :timeslot;

// 前向声明，解决循环引用
export class Student;

export class Course {
public:
    // 构造函数
    Course(std::string id, std::string name, int capacity, int enrolled,
           double credit, std::string teacherId, std::string teacherName,
           Timeslot timeslot);

    // 检查课程是否已满
    bool isFull() const;

    // 尝试接受学生报名
    bool acceptEnrollment(Student* s);

    // 移除学生报名信息
    void removeEnrollment(Student* s);

    // Operator ==
    bool operator==(const Course& other) const;

    // 数据传输器 (Visitor Pattern): 允许外部以受控方式访问内部状态
    template<typename Func>
    void transferData(Func&& receiver) const;

    // 业务逻辑查询方法 (替代 Getter)
    bool isTaughtBy(std::string_view teacherId) const;
    
    bool conflictsWith(const Course& other) const;

    // 检查 ID 是否匹配
    bool hasId(std::string_view id) const;

    // 获取课程详细信息字符串
    std::string course_info() const;

private:
    std::string m_id;                 // 课程 ID
    std::string m_name;               // 课程名称
    int m_capacity;                   // 最大容量
    int m_enrolledCount;              // 已报名人数 (从数据库同步)

    // 新增字段
    double m_credit;                  // 学分
    std::string m_teacherId;          // 教师 ID
    std::string m_teacherName;        // 教师姓名 (冗余存储，便于显示)
    Timeslot m_timeslot;              // 时间槽

    std::vector<Student*> m_students; // 已选修该课程的学生列表 (内存中的关联)
};

// --- Implementation ---

Course::Course(std::string id, std::string name, int capacity, int enrolled,
               double credit, std::string teacherId, std::string teacherName,
               Timeslot timeslot)
    : m_id(id), m_name(name), m_capacity(capacity), m_enrolledCount(enrolled),
      m_credit(credit), m_teacherId(teacherId), m_teacherName(teacherName),
      m_timeslot(timeslot) {}



/**
* @brief 检查课程是否已满
* @return true 如果当前选课人数 >= 容量，否则 false
*/
bool Course::isFull() const {
    return m_enrolledCount >= m_capacity;
}



/**
* @brief 尝试接受学生报名
* @param s 申请报名的学生指针
* @return true 报名成功, false 报名失败 (如课程已满)
*/
bool Course::acceptEnrollment(Student* s) {
    if (isFull()) return false;
    m_students.push_back(s);
    m_enrolledCount++; // 同步增加计数
    return true;
}



/**
* @brief 移除学生报名信息
* @param s 要移除的学生指针
*/
void Course::removeEnrollment(Student* s) {
    std::erase(m_students, s);
    if (m_enrolledCount > 0) m_enrolledCount--;
}

/**
 * @brief 重载相等运算符
 */
bool Course::operator==(const Course& other) const {
    return m_id == other.m_id;
}

/**
 * @brief 数据传输器实现
 * 注意：作为模板函数，实现必须位于模块接口文件中
 */
template<typename Func>
void Course::transferData(Func&& receiver) const {
    receiver(m_id, m_name, m_capacity, m_enrolledCount, m_credit, m_teacherId, m_teacherName, m_timeslot);
}

/**
 * @brief 检查是否由指定教师授课
 */
bool Course::isTaughtBy(std::string_view teacherId) const {
    return m_teacherId == teacherId;
}

/**
 * @brief 检查是否与另一门课程时间冲突
 */
bool Course::conflictsWith(const Course& other) const {
    return m_timeslot.overlaps(other.m_timeslot);
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
 * @return 格式化后的课程信息
 */
std::string Course::course_info() const {
    return std::format("[Course] {} - {} ({:.1f}pts) by {} | {} ({}/{})",
        m_id, m_name, m_credit, m_teacherName, m_timeslot.toString(),
        m_enrolledCount, m_capacity);
}
