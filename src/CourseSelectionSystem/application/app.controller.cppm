/**
* @file    src/CourseSelectionSystem/application/app.controller.cppm
* @date    2026-01-07
* @author  GY
* @brief   应用层分区：系统控制器
*
* 负责管理整个选课系统的运行流程
* 维护学生和课程列表，提供选课、退课等核心业务功能
* 协调领域层实体之间的交互
*
* Change Log:
* [v1.0] GY   2026-01-06
* * 初始化项目架构 (基于 C++23 Modules)
* [v1.5] GY   2026-01-07
* * 重构 SystemController，移除内存存储，接入 DBAdapter
* [v2.0] GY   2026-01-07
* * 完成 SystemController 与 DBAdapter 的深度集成
* * 移除内存容器 (std::vector)，全量迁移至 SQL 事务操作
* * 实现基于数据库的实时选课/退课业务逻辑（含并发安全/容量检查）
* [v3.0] GY   2026-01-10
    本次提交完成了核心架构的重构，解决了 Controller 直接依赖数据库的违规问题，并实现了关键业务逻辑。

    [架构重构]
    - Infrastructure: 新增 StudentProxy, CourseProxy, EnrollmentProxy 分区模块。
    - Refactor: 将 SystemController 中的 SQL 语句全部迁移至代理层，初步实现 4 层架构的关注点分离。

    [领域逻辑]
    - Domain: 新增 Timeslot 模块，并在 Student 实体中实现 hasTimeConflict 冲突检测算法。
    - Entity: 扩展 Course 实体，支持学分、教师、时间槽等完整字段。

    [业务功能]
    - Secretary: 完成“创建课程”功能的后端链路与 CLI 对接。
    - Teacher: 完成“录入成绩”功能的数据库交互。

    [构建系统]
    - CMake: 更新配置以支持新增的 Infrastructure 分区文件。
*
*/
export module application;

import domain;
import infrastructure;
import std;

export class SystemController {
public:
    SystemController(); // 构造函数：初始化数据库适配器
    void initialize(); // 系统初始化：建立连接、创建表结构
    void run(); // 启动系统运行逻辑

    // 核心业务功能
    void performEnrollment(std::string sid, std::string cid); // 执行选课业务逻辑
    void performDrop(std::string sid, std::string cid); // 执行退课业务逻辑
    
    // 教学秘书功能
    bool createCourse(std::string id, std::string name, int capacity, double credit, 
                      std::string teacherName, int weekday, int timeslot); // 创建新课程
    
    // 教师功能
    bool updateGrade(std::string sid, std::string cid, int score); // 录入/修改学生成绩

private:
    std::unique_ptr<db::DBAdapter> m_db; // 数据库适配器指针
};

// --- Implementation ---

SystemController::SystemController() : m_db(std::make_unique<db::DBAdapter>()) {}

void SystemController::initialize() {
    std::string conn_str = "dbname=CourseSelectionSystem user=postgres password=123 hostaddr=127.0.0.1 port=5432";
    if (!m_db->connect(conn_str)) {
        std::print("Error: Failed to connect to database.\n");
        return;
    }

    // DDL: 重置数据库结构
    m_db->execute("DROP TABLE IF EXISTS enrollment CASCADE");
    m_db->execute("DROP TABLE IF EXISTS course CASCADE");
    m_db->execute("DROP TABLE IF EXISTS student CASCADE");
    
    m_db->execute(R"(
        CREATE TABLE course (
            id VARCHAR(50) PRIMARY KEY,
            name TEXT NOT NULL,
            capacity INT DEFAULT 120,
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

    // 录入初始数据
    m_db->execute("INSERT INTO student (id, name) VALUES ('2024051604085', 'Gao Yang')");

    // 真实课程数据导入
    struct RawCourse {
        std::string id; std::string name; int cap; double cr; 
        std::string tid; std::string tname; int w; int t;
    };
    std::vector<RawCourse> courses = {
        {"0000002564", "国家安全教育（网络视频课）", 120, 1, "00000000", "网络教师", 0, 0},
        {"0000006001", "军事理论（网络视频课）", 120, 2, "22000708", "张国清", 0, 0},
        {"0000002209", "习近平总书记关于教育的重要论述专题（网络视频课）", 120, 1, "20131591", "毛宇", 0, 0},
        {"003AA8002A", "大学英语I", 120, 3, "20247023", "赖守浪", 1, 3},
        {"003A202027", "军事技能", 120, 2, "20131725", "任文霞", 0, 0},
        {"083BA50005", "计算机导论", 120, 2.5, "20220045", "孙晓宁", 3, 4},
        {"083BA8013A", "C语言程序设计", 120, 4, "20200085", "肖颗", 1, 1},
        {"003AA00001", "形势与政策I", 120, 0.25, "20131381", "黄玲", 2, 4},
        {"003AA600AA", "思想道德与法治", 120, 2.5, "20141113", "伍崇利", 1, 4},
        {"0504321004", "线性代数", 120, 4, "20210031", "莫长鑫", 2, 2},
        {"073BAA0033", "高等数学(1)", 120, 5, "20131925", "赵侯宇", 2, 1},
        {"003AA2003A", "体育I", 120, 1, "20180056", "魏胜辉", 3, 2},
        {"003AA40037", "大学生心理健康教育", 120, 2, "20200047", "彭臻", 2, 5},
        {"083BA70004", "数据结构", 120, 3.5, "20240120", "高丽萍", 1, 1},
        {"083BA70005", "计算机系统基础", 120, 3.5, "20230071", "郭桃林", 1, 2},
        {"083BA8024A", "Linux程序设计", 120, 4, "20170001,20240008", "冯骥,袁晓涵", 2, 2},
        {"083FA10006", "C语言程序设计课程设计", 120, 0.5, "20200085", "肖颗", 0, 0},
        {"083FA20051", "数据结构课程设计", 120, 1, "20240120", "高丽萍", 0, 0},
        {"003AA00002", "形势与政策II", 120, 0.25, "20131675", "黎朝红", 1, 4},
        {"003AA3000A", "中国近现代史纲要", 120, 2.5, "20131930", "李虹辉", 2, 2},
        {"073BAA0034", "高等数学(2)", 120, 5, "20170051,20190041", "许秋菊,郭闪闪", 2, 1},
        {"003AA2004A", "体育II", 120, 1, "20131344", "成平", 3, 2},
        {"003AA8003A", "大学英语II", 120, 3, "20130891", "沈纯", 1, 3},
        {"XSC2013011", "职业生涯规划与就业指导1", 120, 1, "20210030", "李黎", 5, 3},
        {"083CA70013", "数据库原理及应用", 120, 3.5, "20210071", "肖旋", 1, 4},
        {"083EB80135", "软件构建与实现", 120, 4, "20131672", "龚伟", 1, 1},
        {"083FA20056", "软件工程综合实训1-1（C++方向）", 120, 1, "20131672", "龚伟", 0, 0},
        {"113BA7002A", "计算机网络", 120, 3.5, "20200085", "肖颗", 2, 3},
        {"003AA00003", "形势与政策III", 120, 0.25, "20130679", "高德华", 2, 1},
        {"003EA4006A", "马克思主义基本原理", 120, 2.5, "20131977", "陈雪连", 2, 2},
        {"073BAR0034", "概率论与数理统计", 120, 4, "20131918", "吕美英", 1, 2},
        {"04A", "体育Ⅲ（羽毛球）", 120, 1, "20132168", "陈丽", 2, 4},
        {"003AA60026", "大学英语IV拓展课系列- AI辅助专门用途英语翻译", 120, 3, "20130442", "龙涛", 4, 5},
        {"083BA70007", "操作系统原理与实践", 120, 3.5, "20132108", "杜兴", 1, 2},
        {"083BA80006", "大学物理", 120, 4, "20250043", "孙川", 3, 5},
        {"083CA40010", "软件工程导论", 120, 2, "20130951", "魏延", 2, 1},
        {"083EB8040A", "Qt6软件开发", 120, 5, "20131672", "龚伟", 3, 1},
        {"083FB20057", "软件工程综合实训2-1（C++方向）", 120, 1, "20131672", "龚伟", 0, 0},
        {"003AA00004", "形势与政策IV", 120, 0.25, "20131977", "陈雪连", 2, 2},
        {"003AAC002A", "习近平新时代中国特色社会主义思想概论", 120, 3, "20220002", "徐琴", 3, 2},
        {"003EAC002B", "毛泽东思想和中国特色社会主义理论体系概论", 120, 2.5, "20150015", "沈乾飞", 3, 4},
        {"04B", "体育Ⅳ（羽毛球）", 120, 1, "20132168", "陈丽", 2, 4}
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
