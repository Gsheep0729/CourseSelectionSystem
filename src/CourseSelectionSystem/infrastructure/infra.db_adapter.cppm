/**
* @file    src/CourseSelectionSystem/infrastructure/infra.db_adapter.cppm
* @brief   Infrastructure partition: DBAdapter & Proxies
*/
module;
#include <pqxx/pqxx>

export module course_system:infrastructure;

import std;
import :domain.student;
import :domain.course;
import :domain.timeslot;

export namespace db {

using Row = std::vector<std::string>;
using Result = std::vector<Row>;

class DBAdapter {
public:
    DBAdapter() = default;
    void set_credentials(std::string conn_str) { m_conn_str = std::move(conn_str); }
    bool connect(const std::string& conn_str = "") {
        if (!conn_str.empty()) m_conn_str = conn_str;
        try { pqxx::connection C(m_conn_str); return C.is_open(); } catch (...) { return false; }
    }
    bool execute(const std::string& sql) {
        try { pqxx::connection C(m_conn_str); pqxx::work W(C); W.exec(sql); W.commit(); return true; } catch (...) { return false; }
    }
    std::optional<Result> query(const std::string& sql) {
        try {
            pqxx::connection C(m_conn_str); pqxx::nontransaction N(C); pqxx::result R(N.exec(sql));
            Result res;
            for (const auto& row : R) {
                Row r; for (const auto& f : row) r.push_back(f.is_null() ? "" : f.c_str()); res.push_back(std::move(r));
            }
            return res;
        } catch (...) { return std::nullopt; }
    }
private:
    std::string m_conn_str;
};

// --- Proxies ---

class StudentProxy {
private:
    DBAdapter& m_db;
public:
    explicit StudentProxy(DBAdapter& db) : m_db(db) {}

    // 核心任务 1: 复杂查询 (JOIN)
    Student* findById(const std::string& id) {
        // 使用 LEFT JOIN 一次性获取学生信息和所有选课信息
        // 假设表名：students, courses, student_courses
        // students(id, name)
        // courses(id, name, capacity, weekday, timeslot)
        // student_courses(student_id, course_id)
        
        std::string sql = std::format(
            "SELECT s.name, c.id, c.name, c.capacity, c.weekday, c.timeslot "
            "FROM students s "
            "LEFT JOIN student_courses sc ON s.id = sc.student_id "
            "LEFT JOIN courses c ON sc.course_id = c.id "
            "WHERE s.id = '{}'", 
            id
        );

        auto res_opt = m_db.query(sql);
        if (!res_opt || res_opt->empty()) {
            return nullptr;
        }

        const auto& rows = *res_opt;
        std::string name = rows[0][0]; // 第一行第一列是姓名
        
        Student* student = new Student(id, name);

        for (const auto& row : rows) {
            // 如果 course_id (row[1]) 为空，说明没选课
            if (row[1].empty()) continue;

            std::string c_id = row[1];
            std::string c_name = row[2];
            int c_capacity = std::stoi(row[3]);
            int weekday = row[4].empty() ? 0 : std::stoi(row[4]);
            int timeslot = row[5].empty() ? 0 : std::stoi(row[5]);

            // 重建 Course 对象
            Course* course = new Course(c_id, c_name, c_capacity, Timeslot{weekday, timeslot});

            // 恢复状态 (绕过业务检查)
            student->restoreEnrollment(course);
        }

        return student;
    }

    // 核心任务 2: 保存状态
    bool save(const Student& student) {
        // 1. 保存学生基本信息 (Upsert)
        std::string sql_student = std::format(
            "INSERT INTO students (id, name) VALUES ('{}', '{}') "
            "ON CONFLICT (id) DO UPDATE SET name = EXCLUDED.name",
            student.getId(), student.getName()
        );

        if (!m_db.execute(sql_student)) return false;

        // 2. 保存选课关系
        // 先删后插
        std::string sql_del = std::format("DELETE FROM student_courses WHERE student_id = '{}'", student.getId());
        if (!m_db.execute(sql_del)) return false;

        for (const auto* course : student.getEnrolledCourses()) {
            std::string sql_link = std::format(
                "INSERT INTO student_courses (student_id, course_id) VALUES ('{}', '{}')",
                student.getId(), course->getId()
            );
            if (!m_db.execute(sql_link)) return false;
        }

        return true;
    }
};

// Stub for CourseProxy
class CourseProxy {
private:
    DBAdapter& m_db;
public:
    explicit CourseProxy(DBAdapter& db) : m_db(db) {}
    Course* findById(const std::string& id) {
        auto res = m_db.query(std::format("SELECT name, capacity, weekday, timeslot FROM courses WHERE id = '{}'", id));
        if (res && !res->empty()) {
            int wd = (*res)[0][2].empty() ? 0 : std::stoi((*res)[0][2]);
            int ts = (*res)[0][3].empty() ? 0 : std::stoi((*res)[0][3]);
            return new Course(id, (*res)[0][0], std::stoi((*res)[0][1]), Timeslot{wd, ts});
        }
        return nullptr;
    }
    bool save(const Course& c) {
        std::string sql = std::format(
            "INSERT INTO courses (id, name, capacity, weekday, timeslot) VALUES ('{}', '{}', {}, {}, {}) "
            "ON CONFLICT (id) DO NOTHING",
            c.getId(), c.getName(), c.getCapacity(), c.getTimeslot().weekday, c.getTimeslot().timeslot
        );
        return m_db.execute(sql);
    }
    static bool addCourse(const Course&, int, int) { return true; }
    static bool updateClassTime(const std::string&, int, int) { return true; }
};

class EnrollmentProxy {
public:
    static bool updateScore(const std::string&, const std::string&, int) { return true; }
};

} // namespace db