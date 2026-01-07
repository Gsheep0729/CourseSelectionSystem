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
* [v1.1] Zhang Tao   2026-01-07
* * 替换硬编码测试逻辑，新增CLI交互测试（调用UserInterface）
*/
import std;
import course_system; // 导入主模块（已包含表现层）

int main() {
    try {
        // 原有测试逻辑注释保留，后续集成时可恢复
        /*
        SystemController app;
        app.initialize();
        app.run();
        */

        // 新增：CLI交互测试（张涛负责的表现层测试）
        std::print("=== 选课系统 CLI 交互测试模式 ===\n");
        UserInterface cli;
        cli.showLoginMenu(); // 启动登录菜单，进入交互流程

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
* [v1.1] Zhang Tao   2026-01-07
* * 导入表现层（presentation）模块，支持CLI交互测试
*/
export module course_system;
export import :domain;
export import :app.controller;
// export import :infrastructure; // 后续添加
export import :presentation; // 新增：导出表现层模块

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

## File: domain/dom.teacher.cppm
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
*/
export module course_system:domain.teacher;

import std;
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

## File: presentation/pres.cli.cppm
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
* * 定义 UserInterface 类及核心菜单接口
* * 实现登录菜单输入处理与角色选择
* * 实现学生、教师、教学秘书菜单循环逻辑
* * 添加输入校验与缓冲区清理，优化交互体验
* * 支持学生、教师、教学秘书三种角色的菜单交互
* [v1.1] Zhang Tao   2026-01-07
* * 优化交互页面空白大小（减少空行数量）
* * 调整菜单选项文字顺序（中文在前、英文在后）
*/
export module course_system:presentation;

import std;

// --- 类声明 ---
export class UserInterface {
public:
    /**
     * @brief 显示登录菜单，处理用户登录流程
     * @return 登录成功的用户ID（非空）
     * @note 仅做模拟登录：ID非空即通过，密码不校验，支持角色选择
     */
    std::string showLoginMenu();

    /**
     * @brief 显示学生主菜单（循环交互）
     * @param studentId 登录学生的ID
     * @note 提供选课、退课、查课表、查成绩功能选项，暂不绑定业务逻辑
     */
    void showStudentMenu(std::string_view studentId);

    /**
     * @brief 显示教师主菜单（循环交互）
     * @param teacherId 登录教师的ID
     * @note 提供查看授课名单、录入成绩、修改成绩功能选项，暂不绑定业务逻辑
     */
    void showTeacherMenu(std::string_view teacherId);

    /**
     * @brief 显示教学秘书主菜单（循环交互）
     * @param secretaryId 登录教学秘书的ID
     * @note 提供创建课程、分配教师、设置上课时间功能选项，暂不绑定业务逻辑
     */
    void showSecretaryMenu(std::string_view secretaryId);

private:
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

/**
* @brief 清除输入缓冲区，避免无效输入导致的交互异常
*/
void UserInterface::clearInputBuffer() const {
    std::cin.clear();                  // 清除错误状态标志
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 忽略缓冲区所有字符
}

/**
* @brief 显示角色选择菜单，返回用户选择的角色类型
* @return 1=学生，2=教师，3=教学秘书
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
* @brief 显示登录菜单，处理用户登录流程
* @return 登录成功的用户ID（非空）
* @note 仅做模拟登录：ID非空即通过，密码不校验，支持角色选择
*/
std::string UserInterface::showLoginMenu() {
    std::string user_id;
    std::string password;

    // 优化：减少空行数量，缩小页面空白
    std::print("\n\n\n");
    std::print("===============================================\n");
    std::print("========== 重庆师范大学选课系统 v1.0 ==========\n");
    std::print("===============================================\n");

    // 输入用户ID（非空校验）
    while (true) {
        std::print("请输入用户ID：");
        std::getline(std::cin, user_id);

        if (user_id.empty()) {
            std::print("❌ 用户ID不能为空！请重新输入。\n");
            continue;
        }
        break;
    }

    // 输入密码（暂不校验，仅做显示）
    // std::print("请输入密码：");
    // std::getline(std::cin, password);
    std::print("✅ 登录验证通过！\n");

    // 角色选择与菜单跳转
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
* @brief 显示学生主菜单（循环交互）
* @param studentId 登录学生的ID
* @note 提供选课、退课、查课表、查成绩功能选项，暂不绑定业务逻辑
*/
void UserInterface::showStudentMenu(std::string_view studentId) {
    int choice = 0;
    while (true) {
        // 优化：减少空行数量，缩小页面空白
        std::print("\n\n\n");
        std::print("===============================================\n");
        std::print("========== 学生菜单 - 用户名：{} ==========\n", studentId);
        std::print("===============================================\n");
        // 调整：中文在前、英文在后
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

        // 功能选中提示（暂不绑定业务逻辑）
        switch (choice) {
            case 1:
                std::print("\n✅ 已选择功能 [1]：选课 (Enroll in Course)\n");
                break;
            case 2:
                std::print("\n✅ 已选择功能 [2]：退课 (Drop Course)\n");
                break;
            case 3:
                std::print("\n✅ 已选择功能 [3]：查看课表 (View Schedule)\n");
                break;
            case 4:
                std::print("\n✅ 已选择功能 [4]：查看成绩 (View Grades)\n");
                break;
            case 5:
                std::print("\n✅ 退出登录成功！返回登录界面。\n");
                return;
        }

        // 停留提示，增强交互体验
        std::print("按Enter键继续...");
        std::cin.get();
    }
}

/**
* @brief 显示教师主菜单（循环交互）
* @param teacherId 登录教师的ID
* @note 提供查看授课名单、录入成绩、修改成绩功能选项，暂不绑定业务逻辑
*/
void UserInterface::showTeacherMenu(std::string_view teacherId) {
    int choice = 0;
    while (true) {
        // 优化：减少空行数量，缩小页面空白
        std::print("\n\n\n");
        std::print("===============================================\n");
        std::print("========== 教师菜单 - 用户名：{} ==========\n", teacherId);
        std::print("===============================================\n");
        // 调整：中文在前、英文在后
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

        // 功能选中提示（暂不绑定业务逻辑）
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

        // 停留提示
        std::print("按Enter键继续...");
        std::cin.get();
    }
}

/**
* @brief 显示教学秘书主菜单（循环交互）
* @param secretaryId 登录教学秘书的ID
* @note 提供创建课程、分配教师、设置上课时间功能选项，暂不绑定业务逻辑
*/
void UserInterface::showSecretaryMenu(std::string_view secretaryId) {
    int choice = 0;
    while (true) {
        // 优化：减少空行数量，缩小页面空白
        std::print("\n\n\n");
        std::print("===============================================\n");
        std::print("========== 教学秘书菜单 - 用户名：{} ==========\n", secretaryId);
        std::print("===============================================\n");
        // 调整：中文在前、英文在后
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

        // 功能选中提示（暂不绑定业务逻辑）
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

        // 停留提示
        std::print("按Enter键继续...");
        std::cin.get();
    }
}

```

---

