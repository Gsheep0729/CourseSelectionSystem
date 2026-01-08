/**
* @file    src/CourseSelectionSystem/infrastructure/infra.enrollment_proxy.cppm
* @date    2026-01-08
* @author  Zhang Tao
* @brief   Infrastructure partition: Enrollment Proxy (Grade Features)
*
* infra.enrollment_proxy:基础设施层选课记录代理模块
* 封装成绩录入/修改的数据访问逻辑，为教师模块提供成绩管理接口
* 严格遵循代理者模式，将SQL语句隔离在基础设施层
*
* Change Log:
* [v1.0] Zhang Tao   2026-01-08
* * 初始化成绩代理类结构（基于C++23 Modules）
* * 实现updateScore方法：支持成绩录入与修改
* * 增加成绩范围校验（0-100分）
*/
export module course_system:infrastructure.enrollment_proxy;

import :infrastructure;  // 导入DBAdapter
import std;

export namespace db {

class EnrollmentProxy {
public:
    /**
     * @brief 录入/修改学生成绩（教师功能）
     * @param studentId 学生ID
     * @param courseId 课程ID
     * @param score 成绩（0-100整数）
     * @return true 操作成功，false 操作失败（成绩无效/记录不存在）
     */
    static bool updateScore(const std::string& studentId, const std::string& courseId, int score);

private:
    /**
     * @brief 私有辅助方法：验证成绩合法性
     * @param score 待验证成绩
     * @return true 合法（0-100），false 非法
     */
    static bool validateScore(int score);
};

// --- Implementation ---

bool EnrollmentProxy::validateScore(int score) {
    if (score < 0 || score > 100) {
        std::print("Error: Invalid score {} (must be 0-100)\n", score);
        return false;
    }
    return true;
}

bool EnrollmentProxy::updateScore(const std::string& studentId, const std::string& courseId, int score) {
    // 1. 验证成绩合法性
    if (!validateScore(score)) {
        return false;
    }

    // 2. 封装SQL语句（确保字段名与表结构一致：enrollment表的score字段）
    std::string sql = std::format(
        "UPDATE enrollment SET score = {} "
        "WHERE student_id = '{}' AND course_id = '{}'",
        score, studentId, courseId
    );

    // 3. 调用DBAdapter执行SQL
    db::DBAdapter db;
    std::string conn_str = "dbname=postgres user=postgres password=123 hostaddr=127.0.0.1 port=5432";
    if (!db.connect(conn_str)) {
        std::print("Error: Database connection failed when updating score\n");
        return false;
    }

    // 4. 执行SQL并检查影响行数
    if (db.execute(sql)) {
        // 查询是否有记录被更新（验证修改是否生效）
        auto res = db.query(std::format(
            "SELECT 1 FROM enrollment WHERE student_id = '{}' AND course_id = '{}' AND score = {}",
            studentId, courseId, score
        ));

        if (res && !res->empty()) {
            std::print("Success: Updated score for Student {} - Course {} to {}\n",
                studentId, courseId, score);
            return true;
        } else {
            std::print("Error: No enrollment record found for Student {} - Course {}\n",
                studentId, courseId);
            return false;
        }
    } else {
        std::print("Error: Failed to update score for Student {} - Course {}\n",
            studentId, courseId);
        return false;
    }
}

} // namespace db
