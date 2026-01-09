/**
* @file    src/CourseSelectionSystem/application/app.controller.cppm
* @date    2026-01-10
* @author  GY
* @brief   Application partition: System Controller
*
* 负责管理整个选课系统的运行流程
* 重构：移除 SQL，使用 Infrastructure 层的 Proxy 进行数据访问。
*/
export module application;

import domain;
import infrastructure;
import std;

export class SystemController {
public:
    SystemController();
    void initialize();
    void run();

    // 核心业务功能
    void performEnrollment(std::string sid, std::string cid);
    void performDrop(std::string sid, std::string cid);
    
    // 教学秘书功能
    bool createCourse(std::string id, std::string name, int capacity, double credit, 
                      std::string teacherName, int weekday, int timeslot);
    
    // 教师功能
    bool updateGrade(std::string sid, std::string cid, int score);

private:
    std::unique_ptr<db::DBAdapter> m_db; 
};

// --- Implementation ---

SystemController::SystemController() : m_db(std::make_unique<db::DBAdapter>()) {}

void SystemController::initialize() {
    std::string conn_str = "dbname=CourseSelectionSystem user=postgres password=123 hostaddr=127.0.0.1 port=5432";
    if (!m_db->connect(conn_str)) {
        std::print("Error: Failed to connect to database.\n");
        return;
    }

    // DDL: 重置数据库结构 (仍然保留在这里，或者移动到专门的 Setup 模块)
    m_db->execute("DROP TABLE IF EXISTS enrollment CASCADE");
    m_db->execute("DROP TABLE IF EXISTS course CASCADE");
    m_db->execute("DROP TABLE IF EXISTS student CASCADE");
    
    m_db->execute(R"(
        CREATE TABLE course (
            id VARCHAR(50) PRIMARY KEY,
            name TEXT NOT NULL,
            capacity INT DEFAULT 40,
            enrolled INT DEFAULT 0,
            credit REAL DEFAULT 0.0,
            teacher_id TEXT,
            teacher_name TEXT,
            weekday INT,
            timeslot INT
        )
    )");

    m_db->execute("CREATE TABLE student (id VARCHAR(50) PRIMARY KEY, name TEXT)");
    
    m_db->execute(R"(
        CREATE TABLE enrollment (
            student_id VARCHAR(50) REFERENCES student(id),
            course_id VARCHAR(50) REFERENCES course(id),
            score INTEGER DEFAULT NULL,
            PRIMARY KEY (student_id, course_id)
        )
    )");

    std::print("Database initialized.\n");

    // 录入初始数据 (保持原样，增加更多数据以测试冲突)
    m_db->execute("INSERT INTO student (id, name) VALUES ('2024051604085', 'Gao Yang')");

    // 示例课程数据
    // 假设：Timeslot 1=1-2节, 2=3-4节
    struct RawCourse {
        std::string id; std::string name; int cap; double cr; 
        std::string tid; std::string tname; int w; int t;
    };
    std::vector<RawCourse> courses = {
        {"CS101", "C++ Programming", 60, 4.0, "T001", "Dr. Stroustrup", 1, 1}, // 周一 1-2节
        {"CS102", "Data Structures", 60, 3.5, "T002", "Prof. Knuth", 1, 1},    // 周一 1-2节 (与 CS101 冲突)
        {"MA101", "Calculus I",      80, 5.0, "T003", "Prof. Newton", 2, 2}    // 周二 3-4节
    };

    for (const auto& c : courses) {
        std::string sql = std::format(
            "INSERT INTO course VALUES ('{}', '{}', {}, 0, {}, '{}', '{}', {}, {})",
            c.id, c.name, c.cap, c.cr, c.tid, c.tname, c.w, c.t
        );
        m_db->execute(sql);
    }
    std::print("Initial data loaded.\n");
}

void SystemController::run() {
    std::print("System Controller Ready.\n");
}

void SystemController::performEnrollment(std::string sid, std::string cid) {
    // 1. 获取课程对象
    auto course = infra::CourseProxy::findCourseById(*m_db, cid);
    if (!course) {
        std::print("Error: Course {} not found.\n", cid);
        return;
    }

    // 2. 获取学生对象 (含已选课程)
    auto student = infra::StudentProxy::findStudentById(*m_db, sid);
    if (!student) {
        std::print("Error: Student {} not found.\n", sid);
        return;
    }

    // 3. 执行业务规则校验
    
    // 3.1 检查是否已选
    if (student->isEnrolled(course.get())) {
        std::print("Error: Already enrolled in %s\n", course->getName());
        return;
    }

    // 3.2 检查容量
    if (course->isFull()) {
        std::print("Error: Course {} is full.\n", course->getName());
        return;
    }

    // 3.3 检查时间冲突
    if (student->hasTimeConflict(course.get())) {
        std::print("Error: Time conflict detected for course %s\n", course->getName());
        return;
    }

    // 4. 持久化 (通过 Proxy)
    if (infra::StudentProxy::saveEnrollment(*m_db, sid, cid)) {
        std::print("Success: Enrolled in %s\n", course->course_info());
    } else {
        std::print("Error: Database operation failed.\n");
    }
}

void SystemController::performDrop(std::string sid, std::string cid) {
    // 1. 简单校验
    if (!infra::StudentProxy::isEnrolled(*m_db, sid, cid)) {
        std::print("Error: Not enrolled in course %s\n", cid);
        return;
    }

    // 2. 执行退课
    if (infra::StudentProxy::removeEnrollment(*m_db, sid, cid)) {
        std::print("Success: Dropped course {}", cid);
    } else {
        std::print("Error: Database operation failed.\n");
    }
}

bool SystemController::createCourse(std::string id, std::string name, int capacity, double credit, 
                                  std::string teacherName, int weekday, int timeslot) {
    // 1. 构建领域对象 (Value Objects & Entities)
    Timeslot ts(weekday, timeslot);
    Course newCourse(id, name, capacity, credit, "T000", teacherName, ts);

    // 2. 调用 Proxy 持久化
    if (infra::CourseProxy::addCourse(*m_db, newCourse)) {
        std::print("Success: Course '{}' created successfully.\n", name);
        return true;
    } else {
        std::print("Error: Failed to create course in database.\n");
        return false;
    }
}

bool SystemController::updateGrade(std::string sid, std::string cid, int score) {
    // 1. 业务校验 (例如成绩范围)
    if (score < 0 || score > 100) {
        std::print("Error: Invalid score {}. Must be between 0 and 100.\n", score);
        return false;
    }

    // 2. 持久化
    if (infra::EnrollmentProxy::updateScore(*m_db, sid, cid, score)) {
        std::print("Success: Updated grade for student {} in course {} to {}.\n", sid, cid, score);
        return true;
    } else {
        std::print("Error: Failed to update grade in database.\n");
        return false;
    }
}
