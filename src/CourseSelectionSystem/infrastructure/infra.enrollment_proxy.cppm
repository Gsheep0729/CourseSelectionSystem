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
* [v5.0] Zhang Tao 2026-01-17
* * 新增 findGradesByStudent 方法，支持学生查询个人成绩
*/
export module infrastructure:enrollment_proxy;
import :db_adapter;
import :dtos;
import :course_proxy;
import std;
export namespace infra {
class EnrollmentProxy {
public:
    static bool updateScore(db::DBAdapter& db, std::string_view studentId, std::string_view courseId, int score); // 更新学生在某门课的成绩
    static std::optional<db::Result> findGradesByCourse(db::DBAdapter& db, std::string_view courseId); // 获取某门课程的所有选课记录及成绩
    static std::vector<GradeDTO> findGradesByStudent(db::DBAdapter& db, std::string_view studentId); // 获取学生个人所有课程成绩
};
} // namespace infra
// --- Implementation ---
namespace infra {
/**
 * @brief 更新学生在某门课的成绩
 * @param db 数据库适配器引用
 * @param studentId 学生ID
 * @param courseId 课程ID
 * @param score 新的分数
 * @return 操作成功返回 true，否则返回 false
 */
bool EnrollmentProxy::updateScore(db::DBAdapter& db, std::string_view studentId, std::string_view courseId, int score) {
    std::string sql = std::format(
        "UPDATE enrollment SET score = {} WHERE student_id = '{}' AND course_id = '{}'",
        score, studentId, courseId
    );
    return db.execute(sql);
}
/**
 * @brief 获取某门课程的所有选课记录及成绩
 * @param db 数据库适配器引用
 * @param courseId 课程ID
 * @return 包含查询结果的 Result 对象，若查询失败则返回 std::nullopt
 */
std::optional<db::Result> EnrollmentProxy::findGradesByCourse(db::DBAdapter& db, std::string_view courseId) {
    std::string sql = std::format(
        "SELECT student_id, score FROM enrollment WHERE course_id = '{}'",
        courseId
    );
    return db.query(sql);
}
/**
 * @brief 获取学生个人所有课程成绩
 * @param db 数据库适配器引用
 * @param studentId 学生ID
 * @return 包含课程名称、学分、成绩的 GradeDTO 列表
 */
std::vector<GradeDTO> EnrollmentProxy::findGradesByStudent(db::DBAdapter& db, std::string_view studentId) {
    std::vector<GradeDTO> grades;
    // 关联查询 enrollment、course 表，获取成绩及课程信息
    std::string sql = std::format(
        "SELECT e.course_id, c.name, c.credit, e.score "
        "FROM enrollment e "
        "JOIN course c ON e.course_id = c.id "
        "WHERE e.student_id = '{}' "
        "ORDER BY c.credit DESC",
        studentId
    );
    auto res = db.query(sql);
    if (res) {
        for (const auto& row : *res) {
            try {
                GradeDTO dto;
                dto.studentId = std::string(studentId);
                dto.courseId = row[0];
                dto.courseName = row[1];
                dto.credit = std::stod(row[2]);
                // 处理未录入成绩的情况（数据库中为 NULL，对应空字符串）
                dto.score = row[3].empty() ? -1 : std::stoi(row[3]);
                grades.emplace_back(dto);
            } catch (const std::exception& e) {
                std::print("Error parsing grade row for student {}: {}\n", studentId, e.what());
            }
        }
    }
    return grades;
}
} // namespace infra
