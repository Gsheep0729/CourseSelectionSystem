/**
* @file    src/CourseSelectionSystem/infrastructure/infra.dtos.cppm
* @date    2026-01-16
* @author  GY
* @brief   基础设施层分区：数据传输对象 (DTO)
*
* 定义用于在层之间传输数据的结构体，避免将领域实体直接暴露给表现层。
* 兼顾原有数据结构和新增的学分展示、课程-学生关联查询需求。
*
* Change Log:
* [v1.0] GY   2026-01-15
* * 初始版本：定义 CourseDTO, GradeDTO, StudentDTO 用于查询结果封装
* [v2.0] GY   2026-01-16
* * 新增 CourseStudentDTO 适配课程学生列表查询
* * 为 GradeDTO 新增 credit 字段，适配成绩查询时的学分展示需求
* * 恢复 GradeDTO 中 studentId/studentName 核心字段，保证成绩与学生的关联
* * 为所有 DTO 补充格式化输出方法，简化表现层展示逻辑
*/

export module infrastructure:dtos;

import std;

export namespace infra {

/**
 * @brief 课程信息 DTO (基础课程信息传输)
 */
struct CourseDTO {
    std::string id;
    std::string name;
    double credit;
    std::string teacherName;
    std::string timeslot; // 格式化后的时间字符串，如 "Mon Slot 1"
    std::string location; // 教室地点

    // 格式化输出课程信息
    std::string toString() const {
        return std::format("[Course] {} - {} ({:.1f}学分) | 教师: {} | 时间: {}",
            id, name, credit, teacherName, timeslot);
    }
};

/**
 * @brief 成绩信息 DTO (关联学生-课程-成绩-学分)
 */
struct GradeDTO {
    std::string studentId;    // 恢复：学生ID
    std::string studentName;  // 恢复：学生姓名
    std::string courseId;     // 课程ID
    std::string courseName;   // 课程名称
    double credit;            // 新增：课程学分
    int score;                // 成绩，NULL 则为 -1 (未录入)

    // 格式化输出成绩信息
    std::string toString() const {
        std::string scoreStr = (score == -1) ? "未录入" : std::format("{}", score);
        return std::format("[Grade] 学生: {}({}) | 课程: {}({:.1f}学分) | 成绩: {}",
            studentName, studentId, courseName, credit, scoreStr);
    }
};

/**
 * @brief 课程-学生关联DTO (用于查询单门课程下的所有学生及成绩)
 */
struct CourseStudentDTO {
    std::string studentId;
    std::string studentName;
    int score; // 成绩，NULL 则为 -1

    // 格式化输出学生信息+成绩
    std::string toString() const {
        std::string scoreStr = (score == -1) ? "未录入" : std::format("{}", score);
        return std::format("[Student] {} - {} | 成绩: {}", studentId, studentName, scoreStr);
    }
};

/**
 * @brief 学生完整信息 DTO (包含已选课程)
 */
struct StudentDTO {
    std::string id;
    std::string name;
    std::vector<CourseDTO> enrolledCourses;

    // 格式化输出学生及已选课程信息
    std::string toString() const {
        std::string info = std::format("学生: {} - {} | 已选课程数: {}\n", id, name, enrolledCourses.size());
        for (const auto& course : enrolledCourses) {
            info += "  " + course.toString() + "\n";
        }
        return info;
    }
};

} // namespace infra
