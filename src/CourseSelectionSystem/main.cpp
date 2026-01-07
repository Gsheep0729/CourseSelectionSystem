/**
* @file    /run/media/root/铠侠D/桌面文件/开源技术群资料/考试/CourseSelectionSystem/CourseSelectionSystem/src/CourseSelectionSystem/main.cpp
* @date    2026-01-07
* @author  GY
* @brief   Integrated Test: Enrollment System (Formatted Output, Hardcoded Data)
*
* Change Log:
* [v1.0] GY   2026-01-06
* * 优化了cmake,实现学生选课系统基础
* [v1.5] GY   2026-01-07
* * 接入 DBAdapter，完成基于真实数据的选课/退课全流程集成测试
*/
import std;
import course_system;

using namespace std;

// 全局 DB 适配器
db::DBAdapter db_adapter;

// 获取课程信息的辅助结构
struct CourseInfo {
    string id;
    string name;
    int enrolled;
    int capacity;
    bool valid;
};

CourseInfo get_course_info(const string& cid) {
    auto res = db_adapter.query(std::format("SELECT name, enrolled, capacity FROM course WHERE id = '{}'", cid));
    if (res && !res->empty()) {
        return {cid, (*res)[0][0], std::stoi((*res)[0][1]), std::stoi((*res)[0][2]), true};
    }
    return {"", "", 0, 0, false};
}

// 模拟选课操作并打印结果
void try_enroll(const string& sid, const string& sname, const string& cid) {
    auto info = get_course_info(cid);
    if (!info.valid) {
        print("Error: Course {} not found.\n", cid);
        return;
    }

    // 检查是否已选
    auto check = db_adapter.query(std::format("SELECT 1 FROM enrollment WHERE student_id='{}' AND course_id='{}'", sid, cid));
    if (check && !check->empty()) {
        print("Error: Student {} is already enrolled in [Course] {} - {} ({}/{})\n", 
              sname, info.id, info.name, info.enrolled, info.capacity);
        return;
    }

    if (info.enrolled >= info.capacity) {
        print("Error: Course {} is full.\n", info.name);
        return;
    }

    // 执行事务
    bool ok1 = db_adapter.execute(std::format("INSERT INTO enrollment VALUES ('{}', '{}')", sid, cid));
    bool ok2 = db_adapter.execute(std::format("UPDATE course SET enrolled = enrolled + 1 WHERE id = '{}'", cid));

    if (ok1 && ok2) {
        // 获取更新后的人数
        info.enrolled++; 
        print("Success: Student {} enrolled in [Course] {} - {} ({}/{})\n", 
              sname, info.id, info.name, info.enrolled, info.capacity);
    } else {
        print("Error: Database failure during enrollment.\n");
    }
}

// 模拟退课操作并打印结果
void try_drop(const string& sid, const string& sname, const string& cid) {
    auto info = get_course_info(cid); // 获取当前信息用于打印
    if (!info.valid) {
        print("Error: Course {} not found.\n", cid);
        return;
    }

    // 检查是否已选
    auto check = db_adapter.query(std::format("SELECT 1 FROM enrollment WHERE student_id='{}' AND course_id='{}'", sid, cid));
    if (!check || check->empty()) {
        print("Error: Student {} is not enrolled in [Course] {} - {} ({}/{})\n", 
              sname, info.id, info.name, info.enrolled, info.capacity);
        return;
    }

    // 执行事务
    bool ok1 = db_adapter.execute(std::format("DELETE FROM enrollment WHERE student_id='{}' AND course_id='{}'", sid, cid));
    bool ok2 = db_adapter.execute(std::format("UPDATE course SET enrolled = enrolled - 1 WHERE id = '{}'", cid));

    if (ok1 && ok2) {
        info.enrolled--;
        print("Success: Student {} dropped [Course] {} - {} ({}/{})\n", 
              sname, info.id, info.name, info.enrolled, info.capacity);
    } else {
        print("Error: Database failure during drop.\n");
    }
}

int main() {
    std::ios::sync_with_stdio(false);

    // --- 1. System Initialization (Silent) ---
    string conn_str = "dbname=postgres user=postgres password=123 hostaddr=127.0.0.1 port=5432";
    if (!db_adapter.connect(conn_str)) {
        return 1;
    }

    // 重置数据库
    db_adapter.execute("DROP TABLE IF EXISTS enrollment");
    db_adapter.execute("DROP TABLE IF EXISTS course");
    db_adapter.execute("DROP TABLE IF EXISTS student");
    db_adapter.execute("CREATE TABLE student (id TEXT PRIMARY KEY, name TEXT)");
    db_adapter.execute("CREATE TABLE course (id TEXT PRIMARY KEY, name TEXT, capacity INT DEFAULT 60, enrolled INT DEFAULT 0)");
    db_adapter.execute("CREATE TABLE enrollment (student_id TEXT, course_id TEXT, PRIMARY KEY (student_id, course_id))");

    // 加载数据
    print("System Initializing Mock Data...\n");
    
    string s_id = "2024051604085";
    string s_name = "Gao Yang"; 
    db_adapter.execute(std::format("INSERT INTO student (id, name) VALUES ('{}', '{}')", s_id, s_name));

    // 手动录入课程数据
    vector<pair<string, string>> manual_courses = {
        {"C0017", "C语言程序设计"},
        {"C0001", "高等数学"},
        {"C0008", "数据结构"},
        {"C0016", "Linux程序设计"},
        {"C0005", "计算机导论"},
        {"C0002", "马克思主义基本原理"},
        {"C0003", "软件构建与实现"},
        {"C0004", "计算机网络"},
        {"C0006", "线性代数"},
        {"C0007", "概率论与数理统计"},
        {"C0009", "数据库原理及应用"},
        {"C0010", "思想道德与法治"},
        {"C0011", "形势与政策"},
        {"C0012", "大学英语"},
        {"C0013", "大学生心理健康教育"},
        {"C0014", "体育"},
        {"C0015", "中国近现代史纲要"}
    };

    int course_count = 0;
    for (const auto& [id, name] : manual_courses) {
        if (db_adapter.execute(std::format("INSERT INTO course (id, name) VALUES ('{}', '{}')", id, name))) {
            course_count++;
        }
    }
    
    print("Loaded 1 students and {} courses.\n", course_count);
    print("System Started.\n");

    // --- 2. Test Cases ---
    
    print("=== Enrollment Tests ===\n");
    try_enroll(s_id, s_name, "C0017"); // C语言
    try_enroll(s_id, s_name, "C0001"); // 高数
    try_enroll(s_id, s_name, "C0008"); // 数据结构

    print("=== Drop Tests ===\n");
    try_drop(s_id, s_name, "C0001");   // 退选高数

    print("=== Re-Enrollment Tests ===\n");
    try_enroll(s_id, s_name, "C0016"); // Linux

    print("=== Invalid Drop Tests ===\n");
    try_drop(s_id, s_name, "C0005");   // 计算机导论 (未选)

    return 0;
}
