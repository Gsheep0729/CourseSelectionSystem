/**
* @file    src/CourseSelectionSystem/infrastructure/infra.dtos.cppm
* @date    2026-01-15
* @author  GY
* @brief   基础设施层分区：数据传输对象 (DTO)
*
* 定义用于在层之间传输数据的结构体。
* 避免将领域实体直接暴露给表现层。
*
* Change Log:
* [v4.5] GY   2026-01-15
* * 初始版本：定义 CourseDTO, GradeDTO, StudentDTO 用于查询结果封装
*/



export module infrastructure:dtos;

import std;

export namespace infra {

/**
 * @brief 课程信息 DTO
 */
struct CourseDTO {
    std::string id;
    std::string name;
    double credit;
    std::string teacherName;
    std::string timeslot; // 格式化后的时间字符串，如 "Mon Slot 1"
    std::string location; // 教室地点
};

/**
 * @brief 成绩信息 DTO
 */
struct GradeDTO {
    std::string studentId;
    std::string studentName;
    std::string courseId;
    std::string courseName;
    int score; // -1 表示暂无成绩
};



/**
 * @brief 学生完整信息 DTO (包含已选课程)
 */
struct StudentDTO {
    std::string id;
    std::string name;
    std::vector<CourseDTO> enrolledCourses;
};

} // namespace infra
