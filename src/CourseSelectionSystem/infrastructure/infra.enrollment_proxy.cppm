/**
* @file    src/CourseSelectionSystem/infrastructure/infra.enrollment_proxy.cppm
* @date    2026-01-10
* @author  GY
* @brief   Infrastructure partition: Enrollment Proxy
*
* 负责选课关系及成绩的持久化操作。
*/
export module infrastructure:enrollment_proxy;

import :db_adapter;
import std;

export namespace infra {

class EnrollmentProxy {
public:
    /**
     * @brief 更新学生在某门课的成绩
     * @param db 数据库适配器
     * @param studentId 学生 ID
     * @param courseId 课程 ID
     * @param score 成绩 (0-100)
     * @return true 成功, false 失败
     */
    static bool updateScore(db::DBAdapter& db, std::string_view studentId, std::string_view courseId, int score);

    /**
     * @brief 获取某门课程的所有选课记录及成绩
     * @return 包含学生ID和成绩的列表
     */
    static std::optional<db::Result> findGradesByCourse(db::DBAdapter& db, std::string_view courseId);
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
