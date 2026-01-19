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
* [v4.5] GY   2026-01-15
* * 实现 findStudentsByCourse 方法，支持关联查询选课学生名单
* * 将内部 StudentDTO 重命名为 CourseStudentDTO 解决命名冲突
* [v5.0.2] Zhang Tao 2026-01-17
* * 新增 updateTeacher 方法，支持更新课程的教师信息
* [v5.5] GY 2026-01-18
* * 实现 deleteCourse 方法，增加针对选课记录的关联检查
* [v5.6] GY   2026-01-19
* * 规范封装：使用 transferData 替代 Getter 进行持久化操作
* [v5.7] GY   2026-01-19
* * 新增 hasTeacherTimeConflict 方法，用于创建课程时的教师时间冲突检测
*/
export module infrastructure:course_proxy;
import domain;
import :db_adapter; // 导入同属于 infrastructure 模块的 db_adapter 分区 (假设 db_adapter 也是 infrastructure 的一部分)
import std;
export namespace infra { // 使用 namespace 区分
struct CourseStudentDTO {
    std::string id;
    std::string name;
    int score; // -1 表示未录入
};
class CourseProxy {
public:
    static std::unique_ptr<Course> findCourseById(db::DBAdapter& db, std::string_view id); // 根据 ID 查找课程
    static std::vector<std::unique_ptr<Course>> findAllCourses(db::DBAdapter& db); // 获取所有课程列表
    static bool addCourse(db::DBAdapter& db, const Course& course); // 将新课程持久化到数据库
    static std::vector<CourseStudentDTO> findStudentsByCourse(db::DBAdapter& db, std::string_view courseId); // 查询某课程的选课学生
    static bool updateTeacher(db::DBAdapter& db, const std::string& courseId, const std::string& teacherId, const std::string& teacherName); // 更新课程的教师信息
    static bool deleteCourse(db::DBAdapter& db, std::string_view courseId); // 删除课程
    static bool hasTeacherTimeConflict(db::DBAdapter& db, std::string_view teacherId, int weekday, int timeslot); // 检查教师时间冲突
};
} // namespace infra
// --- Implementation ---
namespace infra {
/**
 * @brief 检查教师是否存在时间冲突
 * @param db 数据库适配器
 * @param teacherId 教师ID
 * @param weekday 星期
 * @param timeslot 节次
 * @return true 如果存在冲突，否则 false
 */
bool CourseProxy::hasTeacherTimeConflict(db::DBAdapter& db, std::string_view teacherId, int weekday, int timeslot) {
    // 忽略网络课 (weekday=0)
    if (weekday == 0) return false;

    std::string sql = std::format(
        "SELECT COUNT(*) FROM course WHERE teacher_id = '{}' AND weekday = {} AND timeslot = {}",
        teacherId, weekday, timeslot
    );
    
    auto res = db.query(sql);
    if (res && !res->empty()) {
        return std::stoi((*res)[0][0]) > 0;
    }
    return false;
}
/**
 * @brief 删除课程（包含前置检查）
 */
bool CourseProxy::deleteCourse(db::DBAdapter& db, std::string_view courseId) {
    // 1. 检查是否有选课记录（防止级联删除导致数据丢失）
    std::string checkSql = std::format("SELECT COUNT(*) FROM enrollment WHERE course_id = '{}'", courseId);
    auto res = db.query(checkSql);
    if (res && !res->empty()) {
        if (std::stoi((*res)[0][0]) > 0) {
            std::print("❌ 删除失败：课程 '{}' 已有学生选修，请先处理选课记录。\n", courseId);
            return false;
        }
    }
    // 2. 执行删除
    std::string sql = std::format("DELETE FROM course WHERE id = '{}'", courseId);
    return db.execute(sql);
}
std::vector<CourseStudentDTO> CourseProxy::findStudentsByCourse(db::DBAdapter& db, std::string_view courseId) {
    std::vector<CourseStudentDTO> students;
    // 关联查询 enrollment 和 student 表
    std::string sql = std::format(
        "SELECT s.id, s.name, e.score "
        "FROM enrollment e "
        "JOIN student s ON e.student_id = s.id "
        "WHERE e.course_id = '{}' "
        "ORDER BY s.id",
        courseId
    );

    auto res = db.query(sql);
    if (res) {
        for (const auto& row : *res) {
            try {
                std::string id = row[0];
                std::string name = row[1];
                int score = -1; // 默认未录入
                // 检查成绩字段是否为空 (假设空字符串表示 NULL)
                if (!row[2].empty()) {
                    score = std::stoi(row[2]);
                }
                students.emplace_back(id, name, score);
            } catch (const std::exception& e) {
                 std::print("Error parsing student row for course {}: {}\n", courseId, e.what());
            }
        }
    }
    return students;
}
std::unique_ptr<Course> CourseProxy::findCourseById(db::DBAdapter& db, std::string_view id) {
    std::string sql = std::format(
        "SELECT name, capacity, enrolled, credit, teacher_id, teacher_name, weekday, timeslot "
        "FROM course WHERE id = '{}'", id);

    auto res = db.query(sql);
    if (!res || res->empty()) {
        return nullptr;
    }
    const auto& row = (*res)[0];
    // row: name, capacity, enrolled, credit, tid, tname, w, t
    std::string name = row[0];
    int cap = std::stoi(row[1]);
    int enrolled = std::stoi(row[2]);
    double credit = std::stod(row[3]);
    std::string tid = row[4];
    std::string tname = row[5];
    int w = std::stoi(row[6]);
    int t = std::stoi(row[7]);
    return std::make_unique<Course>(
        std::string(id), name, cap, enrolled, credit, tid, tname, Timeslot(w, t)
    );
}
std::vector<std::unique_ptr<Course>> CourseProxy::findAllCourses(db::DBAdapter& db) {
    std::string sql = "SELECT id, name, capacity, enrolled, credit, teacher_id, teacher_name, weekday, timeslot FROM course ORDER BY id";
    auto res = db.query(sql);

    std::vector<std::unique_ptr<Course>> courses;
    if (!res) return courses;
    for (const auto& row : *res) {
        std::string id = row[0];
        std::string name = row[1];
        int cap = std::stoi(row[2]);
        int enrolled = std::stoi(row[3]);
        double credit = std::stod(row[4]);
        std::string tid = row[5];
        std::string tname = row[6];
        int w = std::stoi(row[7]);
        int t = std::stoi(row[8]);
        courses.push_back(std::make_unique<Course>(
            id, name, cap, enrolled, credit, tid, tname, Timeslot(w, t)
        ));
    }
    return courses;
}
bool CourseProxy::addCourse(db::DBAdapter& db, const Course& course) {
    bool success = false;
    course.transferData([&](const auto& id, const auto& name, int cap, int enrolled, double credit, const auto& tid, const auto& tname, const auto& ts) {
        int weekday, period;
        ts.transferData([&](int w, int p) { weekday = w; period = p; });
        
        std::string sql = std::format(
            "INSERT INTO course (id, name, capacity, enrolled, credit, teacher_id, teacher_name, weekday, timeslot) "
            "VALUES ('{}', '{}', {}, 0, {}, '{}', '{}', {}, {})",
            id, name, cap, credit, tid, tname, weekday, period
        );
        success = db.execute(sql);
    });
    return success;
}
/**
 * @brief 更新课程的教师信息
 * @param db 数据库适配器引用
 * @param courseId 课程ID
 * @param teacherId 新教师ID
 * @param teacherName 新教师姓名
 * @return 操作成功返回 true，否则返回 false
 */
bool CourseProxy::updateTeacher(db::DBAdapter& db, const std::string& courseId, const std::string& teacherId, const std::string& teacherName) {
    std::string sql = std::format(
        "UPDATE course SET teacher_id = '{}', teacher_name = '{}' WHERE id = '{}'",
        teacherId, teacherName, courseId
    );
    return db.execute(sql);
}
} // namespace infra
