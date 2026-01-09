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
        std::string name = rows[0][0]; 
        
        Student* student = new Student(id, name);

        for (const auto& row : rows) {
            if (row[1].empty()) continue;

            std::string c_id = row[1];
            std::string c_name = row[2];
            int c_capacity = std::stoi(row[3]);
            int weekday = row[4].empty() ? 0 : std::stoi(row[4]);
            int timeslot = row[5].empty() ? 0 : std::stoi(row[5]);

            // 重建 Course 对象
            // 注意：这里恢复的 Course 对象只是为了显示学生选了什么课
            // 它的 enrolled 计数可能是不准确的，除非我们再去查询一次
            // 为了性能，这里暂时设为 0 或者不设置，因为 Student::restoreEnrollment 只是建立链接
            // 使用默认学分和教师ID (0, "")
            Course* course = new Course(c_id, c_name, c_capacity, 0, "", Timeslot{weekday, timeslot});
            
            // 简单的恢复已选人数，如果需要的话 (可选优化: join count)
            // 这里为了保持简单，暂不查询该课程的总人数，因为只是查看学生信息

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
        // 先删后插 (简单粗暴但有效)
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

class CourseProxy {
private:
    DBAdapter& m_db;
public:
    explicit CourseProxy(DBAdapter& db) : m_db(db) {}
    
    Course* findById(const std::string& id) {
        // 1. 获取课程基本信息
        auto res = m_db.query(std::format("SELECT name, capacity, weekday, timeslot, credit, teacher_id FROM courses WHERE id = '{}'", id));
        if (!res || res->empty()) {
            return nullptr;
        }
        
        std::string name = (*res)[0][0];
        int capacity = std::stoi((*res)[0][1]);
        int wd = (*res)[0][2].empty() ? 0 : std::stoi((*res)[0][2]);
        int ts = (*res)[0][3].empty() ? 0 : std::stoi((*res)[0][3]);
        int credit = (*res)[0][4].empty() ? 0 : std::stoi((*res)[0][4]);
        std::string tid = (*res)[0][5];

        Course* course = new Course(id, name, capacity, credit, tid, Timeslot{wd, ts});

        // 2. 获取当前已选人数
        auto count_res = m_db.query(std::format("SELECT COUNT(*) FROM student_courses WHERE course_id = '{}'", id));
        if (count_res && !count_res->empty()) {
            int count = std::stoi((*count_res)[0][0]);
            course->setEnrolled(count);
        }

        return course;
    }

    bool save(const Course& c) {
        std::string sql = std::format(
            "INSERT INTO courses (id, name, capacity, weekday, timeslot, credit, teacher_id) VALUES ('{}', '{}', {}, {}, {}, {}, '{}') "
            "ON CONFLICT (id) DO UPDATE SET name=EXCLUDED.name, capacity=EXCLUDED.capacity, weekday=EXCLUDED.weekday, timeslot=EXCLUDED.timeslot, credit=EXCLUDED.credit, teacher_id=EXCLUDED.teacher_id",
            c.getId(), c.getName(), c.getCapacity(), c.getTimeslot().weekday, c.getTimeslot().timeslot, c.getCredit(), c.getTeacherId()
        );
        return m_db.execute(sql);
    }
    
    // 真实实现：添加课程
    bool addCourse(const Course& c, int weekday, int timeslot) {
        // 复用 save，但确保时间被设置
        // 注意：Course 对象本身是 const，不能修改。
        // 但我们在 save 里是读取 c 的字段。传入的 c 可能没有设置时间（如果构造时没传）。
        // 这里我们可以直接构建 SQL。
        std::string sql = std::format(
            "INSERT INTO courses (id, name, capacity, weekday, timeslot, credit, teacher_id) VALUES ('{}', '{}', {}, {}, {}, {}, '{}')",
            c.getId(), c.getName(), c.getCapacity(), weekday, timeslot, c.getCredit(), c.getTeacherId()
        );
        return m_db.execute(sql);
    }

    // 真实实现：更新上课时间
    bool updateClassTime(const std::string& courseId, int weekday, int timeslot) {
        std::string sql = std::format(
            "UPDATE courses SET weekday = {}, timeslot = {} WHERE id = '{}'",
            weekday, timeslot, courseId
        );
        return m_db.execute(sql);
    }
};

class EnrollmentProxy {
private:
    DBAdapter& m_db;
public:
    explicit EnrollmentProxy(DBAdapter& db) : m_db(db) {}

    // 真实实现：更新成绩
    bool updateScore(const std::string& studentId, const std::string& courseId, int score) {
        // 检查记录是否存在
        auto check = m_db.query(std::format(
            "SELECT 1 FROM student_courses WHERE student_id='{}' AND course_id='{}'", 
            studentId, courseId));
        
        if (!check || check->empty()) return false;

        std::string sql = std::format(
            "UPDATE student_courses SET score = {} WHERE student_id = '{}' AND course_id = '{}'",
            score, studentId, courseId
        );
        return m_db.execute(sql);
    }
};

} // namespace db