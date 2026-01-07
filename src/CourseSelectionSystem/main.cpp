/**
* @file    src/CourseSelectionSystem/main.cpp
* @brief   Integrated Test: Enrollment System with Database
*          使用真实课程数据进行选课/退课业务流程模拟
*/
import std;
import course_system;

using namespace std;

// 辅助工具：字符串分割
vector<string> split(const string& s, const string& delimiter) {
    vector<string> tokens;
    size_t start = 0, end = 0;
    while ((end = s.find(delimiter, start)) != string::npos) {
        if (end != start) {
            tokens.push_back(s.substr(start, end - start));
        }
        start = end + delimiter.length();
    }
    if (start < s.length()) {
        tokens.push_back(s.substr(start));
    }
    return tokens;
}

// 辅助函数：去除空白字符
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (string::npos == first) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

void print_separator(const string& title) {
    print("\n=== {} ===\n", title);
}

int main() {
    std::ios::sync_with_stdio(false); // 提升性能

    print_separator("System Initialization");

    try {
        db::DBAdapter db;
        // 数据库连接配置
        string conn_str = "dbname=postgres user=postgres password=123 hostaddr=127.0.0.1 port=5432";
        
        if (!db.connect(conn_str)) {
            print("Fatal Error: Database connection failed.\n");
            return 1;
        }

        // 1. 初始化数据库结构 (DDL)
        print("-> Resetting database tables...\n");
        db.execute("DROP TABLE IF EXISTS enrollment");
        db.execute("DROP TABLE IF EXISTS course");
        db.execute("DROP TABLE IF EXISTS student");

        db.execute("CREATE TABLE student (id TEXT PRIMARY KEY, name TEXT)");
        // 增加 enrolled 字段记录已选人数
        db.execute("CREATE TABLE course (id TEXT PRIMARY KEY, name TEXT, capacity INT DEFAULT 60, enrolled INT DEFAULT 0)");
        db.execute("CREATE TABLE enrollment (student_id TEXT, course_id TEXT, PRIMARY KEY (student_id, course_id))");


        // 2. 准备基础数据 (Data Preparation)
        print("-> Loading student data...\n");
        string s_id = "2024051604085";
        string s_name = "GaoYang";
        db.execute(std::format("INSERT INTO student (id, name) VALUES ('{}', '{}')", s_id, s_name));
        print("   Added Student: {} ({})\n", s_name, s_id);

        print("-> Loading course data from 'course_data.txt'...\n");
        std::ifstream file("CourseSelectionSystem/course_data.txt");
        if (!file.is_open()) {
            print("Error: Could not open course_data.txt\n");
            return 1;
        }

        string line;
        int course_count = 0;
        while (std::getline(file, line)) {
            if (trim(line).empty()) continue;
            
            // 简单的解析逻辑：假设 ID 和 Name 之间有空白符
            std::stringstream ss(line);
            string c_id, c_name;
            ss >> c_id;
            // Name 可能是剩下的所有内容（包括中文）
            getline(ss, c_name);
            c_name = trim(c_name);

            if (!c_id.empty() && !c_name.empty()) {
                // 插入课程，默认容量 60
                string sql = std::format("INSERT INTO course (id, name, capacity) VALUES ('{}', '{}', 60)", c_id, c_name);
                if (db.execute(sql)) {
                    course_count++;
                }
            }
        }
        print("   Successfully loaded {} courses.\n", course_count);


        // 3. 测试选课操作 (Test Enrollment)
        print_separator("Test 1: Enrollment Operation");
        
        // 目标课程：C语言程序设计 (C0017) 和 数据结构 (C0008)
        vector<string> target_courses = {"C0017", "C0008"};

        for (const auto& cid : target_courses) {
            auto res = db.query(std::format("SELECT name, capacity, enrolled FROM course WHERE id = '{}'", cid));
            if (res && !res->empty()) {
                string cname = (*res)[0][0];
                print("Attempting to enroll in: {} ({})\n", cname, cid);

                // 简单的事务模拟
                bool ok1 = db.execute(std::format("INSERT INTO enrollment VALUES ('{}', '{}')", s_id, cid));
                bool ok2 = db.execute(std::format("UPDATE course SET enrolled = enrolled + 1 WHERE id = '{}'", cid));

                if (ok1 && ok2) {
                    print("   SUCCESS: Enrolled in {}\n", cname);
                } else {
                    print("   FAILURE: Could not enroll.\n");
                }
            }
        }


        // 4. 验证选课结果 (Verification)
        print_separator("Verification: Check Database Records");
        auto res_verify = db.query(std::format(
            "SELECT c.id, c.name, c.enrolled FROM enrollment e "
            "JOIN course c ON e.course_id = c.id "
            "WHERE e.student_id = '{}' ORDER BY c.id", s_id));
        
        if (res_verify && !res_verify->empty()) {
            print("Student {} has enrolled in {} courses:\n", s_name, res_verify->size());
            for(const auto& row : *res_verify) {
                print(" - [{}] {} (Current Enrolled: {})\n", row[0], row[1], row[2]);
            }
        } else {
            print("No courses found for student.\n");
        }


        // 5. 测试退课操作 (Test Drop)
        print_separator("Test 2: Drop Operation");
        string drop_target = "C0017"; // 退选 C语言
        print("Dropping course: {}\n", drop_target);

        bool d1 = db.execute(std::format("DELETE FROM enrollment WHERE student_id='{}' AND course_id='{}'", s_id, drop_target));
        bool d2 = db.execute(std::format("UPDATE course SET enrolled = enrolled - 1 WHERE id = '{}'", drop_target));

        if (d1 && d2) {
             print("   SUCCESS: Dropped course.\n");
        } else {
             print("   FAILURE: Error during drop.\n");
        }

        // 最终验证
        print("-> Verifying final state...\n");
        auto res_final = db.query(std::format("SELECT count(*) FROM enrollment WHERE student_id='{}'", s_id));
        if (res_final) {
            print("   Final course count: {}\n", (*res_final)[0][0]);
        }

        print_separator("Test Finished");

    } catch (const std::exception& e) {
        print("Exception: {}\n", e.what());
        return 1;
    }

    return 0;
}