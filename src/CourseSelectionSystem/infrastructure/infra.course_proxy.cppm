/**
* @file    src/CourseSelectionSystem/infrastructure/infra.course_proxy.cppm
* @date    2026-01-08
* @author  Zhang Tao
* @brief   基础设施层分区：课程数据代理
*
* 负责 Course 对象的持久化操作。
* 将领域对象 Course 与数据库表 course 进行映射。
*
* Change Log:
* [v1.0] Zhang Tao   2026-01-08
* * 初始化课程代理类结构（基于C++23 Modules）
* * 实现addCourse方法：封装课程创建SQL，支持容量、时间槽参数
* * 实现updateClassTime方法：支持修改课程上课时间
* [v1.1] Zhang Tao   2026-01-08
* * 修复Course类接口调用错误（补充getName/getCapacity访问方法）
* * 修正SQL语句格式错误和变量名拼写错误
*/
export module infrastructure:course_proxy;

import domain;
import :db_adapter; // 导入同属于 infrastructure 模块的 db_adapter 分区 (假设 db_adapter 也是 infrastructure 的一部分)

import std;

export namespace infra { // 使用 namespace 区分

class CourseProxy {
public:
    static std::unique_ptr<Course> findCourseById(db::DBAdapter& db, std::string_view id); // 根据 ID 查找课程
    static std::vector<std::unique_ptr<Course>> findAllCourses(db::DBAdapter& db); // 获取所有课程列表
    static bool addCourse(db::DBAdapter& db, const Course& course); // 将新课程持久化到数据库
};

} // namespace infra

// --- Implementation ---

namespace infra {

std::unique_ptr<Course> CourseProxy::findCourseById(db::DBAdapter& db, std::string_view id) {
    std::string sql = std::format(
        "SELECT name, capacity, credit, teacher_id, teacher_name, weekday, timeslot "
        "FROM course WHERE id = '{}'", id);
    
    auto res = db.query(sql);
    if (!res || res->empty()) {
        return nullptr;
    }

    const auto& row = (*res)[0];
    // row: name, capacity, credit, tid, tname, w, t
    std::string name = row[0];
    int cap = std::stoi(row[1]);
    double credit = std::stod(row[2]);
    std::string tid = row[3];
    std::string tname = row[4];
    int w = std::stoi(row[5]);
    int t = std::stoi(row[6]);

    return std::make_unique<Course>(
        std::string(id), name, cap, credit, tid, tname, Timeslot(w, t)
    );
}

std::vector<std::unique_ptr<Course>> CourseProxy::findAllCourses(db::DBAdapter& db) {
    std::string sql = "SELECT id, name, capacity, credit, teacher_id, teacher_name, weekday, timeslot FROM course ORDER BY id";
    auto res = db.query(sql);
    
    std::vector<std::unique_ptr<Course>> courses;
    if (!res) return courses;

    for (const auto& row : *res) {
        std::string id = row[0];
        std::string name = row[1];
        int cap = std::stoi(row[2]);
        double credit = std::stod(row[3]);
        std::string tid = row[4];
        std::string tname = row[5];
        int w = std::stoi(row[6]);
        int t = std::stoi(row[7]);

        courses.push_back(std::make_unique<Course>(
            id, name, cap, credit, tid, tname, Timeslot(w, t)
        ));
    }
    return courses;
}

bool CourseProxy::addCourse(db::DBAdapter& db, const Course& course) {
    const auto& ts = course.getTimeslot();
    std::string sql = std::format(
        "INSERT INTO course (id, name, capacity, enrolled, credit, teacher_id, teacher_name, weekday, timeslot) "
        "VALUES ('{}', '{}', {}, 0, {}, 'TBD', '{}', {}, {})",
        course.getId(), course.getName(), course.getCapacity(), 
        course.getCredit(), course.getTeacherName(), ts.getWeekday(), ts.getPeriod()
    );
    return db.execute(sql);
}

} // namespace infra
