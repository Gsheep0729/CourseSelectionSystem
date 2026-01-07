# 项目: CourseSelectionSystem

## 项目特征总结





---

## File: main.cpp
```cpp
/**
* @file    /run/media/root/铠侠D/桌面文件/开源技术群资料/考试/CourseSelectionSystem/CourseSelectionSystem/src/CourseSelectionSystem/main.cpp
* @date    2026-01-06
* @author  GY
* @brief   Main entry point
*
* main:程序主入口文件
* 负责创建 SystemController 实例并启动选课系统
* 捕获并处理运行时异常，确保程序异常退出时有错误提示
*
* Change Log:
* [v1.0] GY   2026-01-06
* * 优化了cmake,实现学生选课系统基础
*/
import std;
import course_system; // 导入主模块

int main() {
    try {
        SystemController app;
        app.initialize();
        app.run();
    } catch (const std::exception& e) {
        std::print("Fatal Error: {}\n", e.what());
        return 1;
    }
    return 0;
}


```

---

## File: course_system.cppm
```cppm
/**
* @file    src/CourseSelectionSystem/course_system.cppm
* @date    2026-01-06
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
*/
export module course_system;

export import :domain;
export import :app.controller;
// export import :infrastructure; // 后续添加
// export import :presentation;   // 后续添加

```

---

## File: domain/dom.student.cppm
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

## File: domain/dom.course.cppm
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

## File: domain/domain.cppm
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

## File: application/app.cppm
```cppm

```

---

## File: application/app.controller.cppm
```cppm
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
export module course_system:app.controller;

import :domain;
import std;

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

```

---

