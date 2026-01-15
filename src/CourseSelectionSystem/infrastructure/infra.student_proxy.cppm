/**
* @file    src/CourseSelectionSystem/infrastructure/infra.student_proxy.cppm
* @date    2026-01-10
* @author  GY
* @brief   基础设施层分区：学生数据代理
*
* 负责学生实体与数据库表之间的映射。
* 提供了学生信息的增删改查以及选课相关的持久化操作。
*
* Change Log:
* [v1.0] GY   2026-01-10
* * 初始版本：实现 StudentProxy 类。
* [v4.5] GY   2026-01-15
* * 实现 findSchedule 方法，通过 JOIN 查询学生课表并封装领域对象
*/

export module infrastructure:student_proxy;

import domain;
import :db_adapter;
import :course_proxy;
import std;

export namespace infra {

class StudentProxy {
public:
    static std::unique_ptr<Student> findStudentById(db::DBAdapter& db, std::string_view id); // 根据 ID 查找学生并加载课程
    static bool saveEnrollment(db::DBAdapter& db, std::string_view studentId, std::string_view courseId); // 保存选课关系
    static bool removeEnrollment(db::DBAdapter& db, std::string_view studentId, std::string_view courseId); // 删除选课关系
    static bool isEnrolled(db::DBAdapter& db, std::string_view studentId, std::string_view courseId); // 检查是否已选课
    static std::vector<Course> findSchedule(db::DBAdapter& db, std::string_view studentId); // 查询学生课表
};

} // namespace infra

// --- Implementation ---

namespace infra {

/**
 * @brief 查询学生的课表
 * @param db 数据库适配器引用
 * @param studentId 学生ID
 * @return 包含课程对象的 vector 列表
 */
std::vector<Course> StudentProxy::findSchedule(db::DBAdapter& db, std::string_view studentId) {
    std::vector<Course> schedule;
    
    // 使用 JOIN 关联 enrollment 和 course 表
    // 注意：假设表名为 course (单数)，与文件中其他方法保持一致
    // 假设 course 表包含: id, name, capacity, enrolled, credit, teacher_id, teacher_name, weekday, timeslot
    std::string sql = std::format(
        "SELECT c.id, c.name, c.capacity, c.enrolled, c.credit, c.teacher_id, c.teacher_name, c.weekday, c.timeslot "
        "FROM enrollment e "
        "JOIN course c ON e.course_id = c.id "
        "WHERE e.student_id = '{}'", 
        studentId
    );
    
    auto res = db.query(sql);
    
    if (res) {
        for (const auto& row : *res) {
            try {
                // 解析结果行
                std::string id = row[0];
                std::string name = row[1];
                int capacity = std::stoi(row[2]);
                int enrolled = std::stoi(row[3]);
                double credit = std::stod(row[4]);
                std::string tid = row[5];
                std::string tname = row[6];
                int weekday = std::stoi(row[7]);
                int period = std::stoi(row[8]);
                
                // 构造 Course 对象并添加到列表
                schedule.emplace_back(id, name, capacity, enrolled, credit, tid, tname, Timeslot(weekday, period));
            } catch (const std::exception& e) {
                std::print("Error parsing schedule row for student {}: {}\n", studentId, e.what());
                // 忽略错误行，继续处理
            }
        }
    }
    
    return schedule;
}

/**
 * @brief 根据 ID 查找学生并加载其已选课程
 * @param db 数据库适配器引用
 * @param id 学生ID
 * @return 指向 Student 对象的 unique_ptr，若未找到则返回 nullptr
 */
std::unique_ptr<Student> StudentProxy::findStudentById(db::DBAdapter& db, std::string_view id) {
    // 1. 加载学生基础信息
    auto res = db.query(std::format("SELECT name FROM student WHERE id = '{}'", id));
    if (!res || res->empty()) {
        return nullptr;
    }
    std::string name = (*res)[0][0];
    auto student = std::make_unique<Student>(std::string(id), name);

    // 2. 加载已选课程 ID 列表
    auto enrollRes = db.query(std::format("SELECT course_id FROM enrollment WHERE student_id = '{}'", id));
    
    if (enrollRes) {
        for (const auto& row : *enrollRes) {
            std::string cid = row[0];
            // 3. 加载课程详情
            auto course = CourseProxy::findCourseById(db, cid);
            if (course) {
                // 将课程对象的所有权转移给学生对象管理
                // TODO: 建立全局 CourseRepository 管理课程生命周期，避免重复创建和潜在内存风险
                student->enrollIn(course.release()); 
            }
        }
    }

    return student;
}


/**
 * @brief 保存选课关系
 * @param db 数据库适配器引用
 * @param studentId 学生ID
 * @param courseId 课程ID
 * @return 操作成功返回 true
 */
bool StudentProxy::saveEnrollment(db::DBAdapter& db, std::string_view studentId, std::string_view courseId) {
    // 事务性操作：插入记录 + 更新计数
    std::string sql1 = std::format("INSERT INTO enrollment (student_id, course_id) VALUES ('{}', '{}')", studentId, courseId);
    if (!db.execute(sql1)) return false;

    std::string sql2 = std::format("UPDATE course SET enrolled = enrolled + 1 WHERE id = '{}'", courseId);
    return db.execute(sql2);
}



/**
 * @brief 移除选课关系
 * @param db 数据库适配器引用
 * @param studentId 学生ID
 * @param courseId 课程ID
 * @return 操作成功返回 true
 */
bool StudentProxy::removeEnrollment(db::DBAdapter& db, std::string_view studentId, std::string_view courseId) {
    std::string sql1 = std::format("DELETE FROM enrollment WHERE student_id='{}' AND course_id='{}'", studentId, courseId);
    if (!db.execute(sql1)) return false;

    std::string sql2 = std::format("UPDATE course SET enrolled = enrolled - 1 WHERE id = '{}'", courseId);
    return db.execute(sql2);
}



/**
 * @brief 检查学生是否已选修某门课
 * @param db 数据库适配器引用
 * @param studentId 学生ID
 * @param courseId 课程ID
 * @return 已选修返回 true，否则返回 false
 */
bool StudentProxy::isEnrolled(db::DBAdapter& db, std::string_view studentId, std::string_view courseId) {
    auto res = db.query(std::format("SELECT 1 FROM enrollment WHERE student_id='{}' AND course_id='{}'", studentId, courseId));
    return (res && !res->empty());
}

} // namespace infra
