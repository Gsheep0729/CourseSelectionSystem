/**
* @file    src/CourseSelectionSystem/application/app.controller.cppm
* @date    2026-01-09
* @author  GY
* @brief   Application partition: System Controller
*
* app.controller:应用层系统控制器模块
* 负责管理整个选课系统的运行流程
* 维护学生和课程列表，提供选课、退课等核心业务功能
* 协调领域层实体之间的交互
*
* Change Log:
* [v2.5] 2026-01-09
* * 更新表结构以支持真实课程数据的完整字段 (学分, 教师, 时间等)
* * 使用硬编码方式录入 course_data.txt 中的真实数据
*/
export module application;

import domain;
import infrastructure;
import std;

export class SystemController {
public:
    SystemController(); // 添加构造函数声明
    // 初始化系统 (连接数据库, 建表, 录入初始数据)
    void initialize();

    // 运行系统
    void run();

    // 执行选课操作
    void performEnrollment(std::string sid, std::string cid);

    // 执行退课操作
    void performDrop(std::string sid, std::string cid);

private:
    std::unique_ptr<db::DBAdapter> m_db; // 数据库适配器指针

    // 辅助函数：获取课程详细信息
    struct CourseInfo {
        std::string id;
        std::string name;
        int enrolled;
        int capacity;
        double credit;
        std::string teacher_name;
        std::string time_str; // e.g. "周一 第3-4节"
        bool valid;
    };
    CourseInfo getCourseInfo(const std::string& cid);
};

// --- Implementation ---

SystemController::SystemController() : m_db(std::make_unique<db::DBAdapter>()) {}

/**
* @brief 初始化系统
* 连接数据库，重建表结构，并录入真实课程数据
*/
void SystemController::initialize() {
    std::string conn_str = "dbname=CourseSelectionSystem user=postgres password=123 hostaddr=127.0.0.1 port=5432";
    if (!m_db->connect(conn_str)) {
        std::print("Error: Failed to connect to database.\n");
        return;
    }

    // 1. 重置数据库结构
    m_db->execute("DROP TABLE IF EXISTS enrollment CASCADE");
    m_db->execute("DROP TABLE IF EXISTS course CASCADE");
    m_db->execute("DROP TABLE IF EXISTS student CASCADE");
    
    // 2. 创建表 (包含完整字段)
    // weekday: 0=无/网络, 1=周一, ... 7=周日
    // timeslot: 1=第1-2节, 2=第3-4节, ... (根据实际情况定义，这里仅存储整数)
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
            PRIMARY KEY (student_id, course_id)
        )
    )");

    std::print("Database tables initialized.\n");

    // 3. 录入初始学生数据
    m_db->execute("INSERT INTO student (id, name) VALUES ('2024051604085', 'Gao Yang')");

    // 4. 录入真实课程数据 (硬编码)
    struct RawCourse {
        std::string id;
        std::string name;
        int cap;
        double credit;
        std::string tid;
        std::string tname;
        int w;
        int t;
    };

    std::vector<RawCourse> courses = {
        {"0000002564", "国家安全教育（网络视频课）", 40, 1.0, "00000000", "网络教师", 0, 0},
        {"0000006001", "军事理论（网络视频课）", 40, 2.0, "22000708", "张国清", 0, 0},
        {"0000002209", "习近平总书记关于教育的重要论述专题（网络视频课）", 40, 1.0, "20131591", "毛宇", 0, 0},
        {"003AA8002A", "大学英语I", 40, 3.0, "20247023", "赖守浪", 1, 3},
        {"003A202027", "军事技能", 40, 2.0, "20131725", "任文霞", 0, 0},
        {"083BA50005", "计算机导论", 40, 2.5, "20220045", "孙晓宁", 3, 4},
        {"083BA8013A", "C语言程序设计", 40, 4.0, "20200085", "肖颗", 1, 1},
        {"003AA00001", "形势与政策I", 40, 0.25, "20131381", "黄玲", 2, 4},
        {"003AA600AA", "思想道德与法治", 40, 2.5, "20141113", "伍崇利", 1, 4},
        {"0504321004", "线性代数", 40, 4.0, "20210031", "莫长鑫", 2, 2},
        {"073BAA0033", "高等数学(1)", 40, 5.0, "20131925", "赵侯宇", 2, 1},
        {"003AA2003A", "体育I", 40, 1.0, "20180056", "魏胜辉", 3, 2},
        {"003AA40037", "大学生心理健康教育", 40, 2.0, "20200047", "彭臻", 2, 5},
        {"083BA70004", "数据结构", 40, 3.5, "20240120", "高丽萍", 1, 1},
        {"083BA70005", "计算机系统基础", 40, 3.5, "20230071", "郭桃林", 1, 2},
        {"083BA8024A", "Linux程序设计", 40, 4.0, "20170001,20240008", "冯骥,袁晓涵", 2, 2},
        {"083FA10006", "C语言程序设计课程设计", 40, 0.5, "20200085", "肖颗", 0, 0},
        {"083FA20051", "数据结构课程设计", 40, 1.0, "20240120", "高丽萍", 0, 0},
        {"003AA00002", "形势与政策II", 40, 0.25, "20131675", "黎朝红", 1, 4},
        {"003AA3000A", "中国近现代史纲要", 40, 2.5, "20131930", "李虹辉", 2, 2},
        {"073BAA0034", "高等数学(2)", 40, 5.0, "20170051,20190041", "许秋菊,郭闪闪", 2, 1},
        {"003AA2004A", "体育II", 40, 1.0, "20131344", "成平", 3, 2},
        {"003AA8003A", "大学英语II", 40, 3.0, "20130891", "沈纯", 1, 3},
        {"XSC2013011", "职业生涯规划与就业指导1", 40, 1.0, "20210030", "李黎", 5, 3},
        {"083CA70013", "数据库原理及应用", 40, 3.5, "20210071", "肖旋", 1, 4},
        {"083EB80135", "软件构建与实现", 40, 4.0, "20131672", "龚伟", 1, 1},
        {"083FA20056", "软件工程综合实训1-1（C++方向）", 40, 1.0, "20131672", "龚伟", 0, 0},
        {"113BA7002A", "计算机网络", 40, 3.5, "20200085", "肖颗", 2, 3},
        {"003AA00003", "形势与政策III", 40, 0.25, "20130679", "高德华", 2, 1},
        {"003EAC002B", "毛泽东思想和中国特色社会主义理论体系概论", 40, 2.5, "20150015", "沈乾飞", 3, 4},
        {"073BAR0034", "概率论与数理统计", 40, 4.0, "20131918", "吕美英", 1, 2},
        {"04A", "体育Ⅲ（羽毛球）", 40, 1.0, "20132168", "陈丽", 2, 4},
        {"003AA60026", "大学英语IV拓展课系列- AI辅助专门用途英语翻译", 40, 3.0, "20130442", "龙涛", 4, 5},
        {"083BA70007", "操作系统原理与实践", 40, 3.5, "20132108", "杜兴", 1, 2},
        {"083BA80006", "大学物理", 40, 4.0, "20250043", "孙川", 3, 5},
        {"083CA40010", "软件工程导论", 40, 2.0, "20130951", "魏延", 2, 1},
        {"083EB8040A", "Qt6软件开发", 40, 5.0, "20131672", "龚伟", 3, 1},
        {"083FB20057", "软件工程综合实训2-1（C++方向）", 40, 1.0, "20131672", "龚伟", 0, 0},
        {"003AA00004", "形势与政策IV", 40, 0.25, "20131977", "陈雪连", 2, 2},
        {"003AAC002A", "习近平新时代中国特色社会主义思想概论", 40, 3.0, "20220002", "徐琴", 3, 2},
        {"003EAC002B", "毛泽东思想和中国特色社会主义理论体系概论", 40, 2.5, "20150015", "沈乾飞", 3, 4},
        {"04B", "体育Ⅳ（羽毛球）", 40, 1.0, "20132168", "陈丽", 2, 4}
    };

    int inserted_count = 0;
    for (const auto& c : courses) {
        // 使用 std::format 构建 SQL 语句
        // 注意：实际项目中应防止 SQL 注入，这里数据是硬编码的所以安全
        std::string sql = std::format(
            "INSERT INTO course (id, name, capacity, credit, teacher_id, teacher_name, weekday, timeslot) "
            "VALUES ('{}', '{}', {}, {}, '{}', '{}', {}, {}) "
            "ON CONFLICT (id) DO NOTHING",
            c.id, c.name, c.cap, c.credit, c.tid, c.tname, c.w, c.t
        );

        if (m_db->execute(sql)) {
            inserted_count++;
        }
    }
    
    std::print("Successfully loaded {} courses into database.\n", inserted_count);
}

void SystemController::run() {
    std::print("System Controller Ready.\n");
}

SystemController::CourseInfo SystemController::getCourseInfo(const std::string& cid) {
    // 查询包含更多字段
    auto res = m_db->query(std::format(
        "SELECT name, enrolled, capacity, credit, teacher_name, weekday, timeslot "
        "FROM course WHERE id = '{}'", cid));
    
    if (res && !res->empty()) {
        try {
            const auto& row = (*res)[0];
            std::string name = row[0];
            int enrolled = std::stoi(row[1]);
            int capacity = std::stoi(row[2]);
            double credit = std::stod(row[3]);
            std::string tname = row[4];
            int w = std::stoi(row[5]);
            int t = std::stoi(row[6]);

            // 格式化时间字符串
            std::string time_desc;
            if (w == 0) {
                time_desc = "Time: N/A (Online/Practice)";
            } else {
                std::vector<std::string> weeks = {"", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
                time_desc = std::format("Time: {} Slot {}", (w >= 1 && w <= 7 ? weeks[w] : "?"), t);
            }

            return {cid, name, enrolled, capacity, credit, tname, time_desc, true};
        } catch (const std::exception& e) {
            std::print("Error parsing course info: {}\n", e.what());
            return {"", "", 0, 0, 0.0, "", "", false};
        }
    }
    return {"", "", 0, 0, 0.0, "", "", false};
}

void SystemController::performEnrollment(std::string sid, std::string cid) {
    auto info = getCourseInfo(cid);
    if (!info.valid) {
        std::print("Error: Course {} not found.\n", cid);
        return;
    }

    // 1. 检查是否重复选课
    auto check = m_db->query(std::format("SELECT 1 FROM enrollment WHERE student_id='{}' AND course_id='{}'", sid, cid));
    if (check && !check->empty()) {
        std::print("Error: Already enrolled in {}\n", info.name);
        return;
    }

    // 2. 检查容量
    if (info.enrolled >= info.capacity) {
        std::print("Error: Course {} is full ({}/{})\n", info.name, info.enrolled, info.capacity);
        return;
    }

    // 3. 执行选课 (事务：插入记录 + 更新计数)
    bool ok1 = m_db->execute(std::format("INSERT INTO enrollment VALUES ('{}', '{}')", sid, cid));
    bool ok2 = m_db->execute(std::format("UPDATE course SET enrolled = enrolled + 1 WHERE id = '{}'", cid));

    if (ok1 && ok2) {
        info.enrolled++;
        std::print("Success: Enrolled in {} ({})\n", info.name, info.id);
        std::print("  - Teacher: {}\n  - Credit:  {:.1f}\n  - {}\n  - Status:  {}/{}\n", 
            info.teacher_name, info.credit, info.time_str, info.enrolled, info.capacity);
    } else {
        std::print("Error: Database transaction failed.\n");
    }
}

void SystemController::performDrop(std::string sid, std::string cid) {
    auto info = getCourseInfo(cid); // 获取当前信息用于打印
    if (!info.valid) {
        std::print("Error: Course {} not found.\n", cid);
        return;
    }

    // 1. 检查是否已选
    auto check = m_db->query(std::format("SELECT 1 FROM enrollment WHERE student_id='{}' AND course_id='{}'", sid, cid));
    if (!check || check->empty()) {
        std::print("Error: Not enrolled in {}\n", info.name);
        return;
    }

    // 2. 执行退课
    bool ok1 = m_db->execute(std::format("DELETE FROM enrollment WHERE student_id='{}' AND course_id='{}'", sid, cid));
    bool ok2 = m_db->execute(std::format("UPDATE course SET enrolled = enrolled - 1 WHERE id = '{}'", cid));

    if (ok1 && ok2) {
        info.enrolled--;
        std::print("Success: Dropped {} ({})\n", info.name, info.id);
        std::print("  - Capacity freed: {}/{}\n", info.enrolled, info.capacity);
    } else {
        std::print("Error: Database transaction failed.\n");
    }
}