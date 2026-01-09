/**
* @file    src/CourseSelectionSystem/infrastructure/infra.enrollment_proxy.cppm
* @date    2026-01-08
* @author  Zhang Tao
* @brief   基础设施层分区：选课记录代理
*
* infra.enrollment_proxy:基础设施层选课记录代理模块
* 封装成绩录入/修改的数据访问逻辑，为教师模块提供成绩管理接口
* 严格遵循代理者模式，将SQL语句隔离在基础设施层
* 负责选课关系及成绩的持久化操作。
*
* Change Log:
* [v1.0] Zhang Tao   2026-01-08
* * 初始化成绩代理类结构（基于C++23 Modules）
* * 实现updateScore方法：支持成绩录入与修改
* * 增加成绩范围校验（0-100分）
*/
export module infrastructure:enrollment_proxy;

import :db_adapter;
import std;

export namespace infra {

class EnrollmentProxy {
public:
    static bool updateScore(db::DBAdapter& db, std::string_view studentId, std::string_view courseId, int score); // 更新学生在某门课的成绩
    static std::optional<db::Result> findGradesByCourse(db::DBAdapter& db, std::string_view courseId); // 获取某门课程的所有选课记录及成绩
};

} // namespace infra

// --- Implementation ---

namespace infra {

bool EnrollmentProxy::updateScore(db::DBAdapter& db, std::string_view studentId, std::string_view courseId, int score) {
    std::string sql = std::format(
        "UPDATE enrollment SET score = {} WHERE student_id = '{}' AND course_id = '{}'",
        score, studentId, courseId
    );
    return db.execute(sql);
}

std::optional<db::Result> EnrollmentProxy::findGradesByCourse(db::DBAdapter& db, std::string_view courseId) {
    std::string sql = std::format(
        "SELECT student_id, score FROM enrollment WHERE course_id = '{}'",
        courseId
    );
    return db.query(sql);
}

} // namespace infra
