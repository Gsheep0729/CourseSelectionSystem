/**
* @file    src/CourseSelectionSystem/infrastructure/infra.course_proxy.cppm
* @date    2026-01-08
* @author  Zhang Tao
* @brief   Infrastructure partition: Course Proxy (Secretary Features)
*
* infra.course_proxy:基础设施层课程代理模块
* 封装课程相关数据访问逻辑，为教学秘书模块提供创建课程、设置上课时间的接口
* 严格遵循代理者模式，将SQL语句隔离在基础设施层，不暴露给上层
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
export module course_system:infrastructure.course_proxy;

import :infrastructure;  // 导入DBAdapter
import :domain.course;    // 导入Course领域对象
import std;

export namespace db {

class CourseProxy {
public:
    /**
     * @brief 创建新课程（教学秘书功能）
     * @param course 课程领域对象（包含ID、名称、容量）
     * @param weekday 上课星期（1=周一，5=周五）
     * @param timeslot 上课时段（1=1-2节，2=3-4节...）
     * @return true 创建成功，false 创建失败（如ID重复）
     */
    static bool addCourse(const Course& course, int weekday, int timeslot);

    /**
     * @brief 设置/修改课程上课时间（教学秘书功能）
     * @param courseId 课程ID
     * @param weekday 新上课星期
     * @param timeslot 新上课时段
     * @return true 修改成功，false 修改失败（如课程不存在）
     */
    static bool updateClassTime(const std::string& courseId, int weekday, int timeslot);

private:
    /**
     * @brief 私有辅助方法：验证时间参数合法性
     * @param weekday 上课星期
     * @param timeslot 上课时段
     * @return true 合法，false 非法
     */
    static bool validateTimeParams(int weekday, int timeslot);
};

// --- Implementation ---

bool CourseProxy::validateTimeParams(int weekday, int timeslot) {
    // 星期范围：1-5（周一至周五），时段范围：1-4（假设每天4个时段）
    if (weekday < 1 || weekday > 5 || timeslot < 1 || timeslot > 4) {
        std::print("Error: Invalid time params - weekday({}), timeslot({})\n", weekday, timeslot);
        return false;
    }
    return true;
}

bool CourseProxy::addCourse(const Course& course, int weekday, int timeslot) {
    // 1. 验证参数合法性
    if (!validateTimeParams(weekday, timeslot)) {
        return false;
    }

    // 2. 封装SQL语句（修复格式错误，占位符与参数顺序一致）
    std::string sql = std::format(
        "INSERT INTO course (id, name, capacity, enrolled, weekday, timeslot) "
        "VALUES ('{}', '{}', {}, 0, {}, {})",
        course.getId(),        // 课程ID
        course.getName(),      // 课程名称（已补充接口）
        course.getCapacity(),  // 课程容量（已补充接口）
        weekday,               // 上课星期
        timeslot               // 上课时段
    );

    // 3. 调用DBAdapter执行SQL
    db::DBAdapter db;
    // 复用系统初始化时的连接字符串（保持兼容）
    std::string conn_str = "dbname=postgres user=postgres password=123 hostaddr=127.0.0.1 port=5432";
    if (!db.connect(conn_str)) {
        std::print("Error: Database connection failed when creating course\n");
        return false;
    }

    if (db.execute(sql)) {
        std::print("Success: Course {} - {} created (weekday: {}, timeslot: {})\n",
            course.getId(), course.getName(), weekday, timeslot);
        return true;
    } else {
        std::print("Error: Failed to create course {} (maybe duplicate ID)\n", course.getId());
        return false;
    }
}

bool CourseProxy::updateClassTime(const std::string& courseId, int weekday, int timeslot) {
    // 1. 验证参数合法性
    if (!validateTimeParams(weekday, timeslot)) {
        return false;
    }

    // 2. 封装SQL语句（修复变量名拼写错误：sal→sql）
    std::string sql = std::format(
        "UPDATE course SET weekday = {}, timeslot = {} WHERE id = '{}'",
        weekday, timeslot, courseId
    );

    // 3. 调用DBAdapter执行SQL
    db::DBAdapter db;
    std::string conn_str = "dbname=postgres user=postgres password=123 hostaddr=127.0.0.1 port=5432";
    if (!db.connect(conn_str)) {
        std::print("Error: Database connection failed when updating class time\n");
        return false;
    }

    if (db.execute(sql)) {
        std::print("Success: Course {} class time updated (weekday: {}, timeslot: {})\n",
            courseId, weekday, timeslot);
        return true;
    } else {
        std::print("Error: Failed to update class time for course {} (maybe course not found)\n", courseId);
        return false;
    }
}

} // namespace db
