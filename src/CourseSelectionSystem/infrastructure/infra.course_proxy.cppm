/**
* @file    src/CourseSelectionSystem/infrastructure/infra.course_proxy.cppm
* @date    2026-01-10
* @author  GY
* @brief   Infrastructure partition: Course Proxy
*
* 负责 Course 对象的持久化操作。
* 将领域对象 Course 与数据库表 course 进行映射。
*/
export module infrastructure:course_proxy;

import domain;
import :db_adapter; // 导入同属于 infrastructure 模块的 db_adapter 分区 (假设 db_adapter 也是 infrastructure 的一部分)
// 注意：之前 infra.db_adapter.cppm 是 'export module infrastructure'，没有分区名。
// 如果它是主模块接口，这里应该是 'import infrastructure;' 但这会导致循环依赖如果我也属于 infrastructure。
// 修正：infra.db_adapter.cppm 应该被定义为 'export module infrastructure;' 
// 而我是 'export module infrastructure:course_proxy;' 
// 这样我是主模块的一个分区。主模块接口需要 export import :course_proxy;
// 
// 但目前的 infra.db_adapter.cppm 已经占用了 'infrastructure' 这个名字作为主模块。
// 让我们查看一下 infra.db_adapter.cppm 的定义。
// 它定义了 export namespace db { class DBAdapter ... }
// 
// 方案：
// 我将作为 infrastructure 的一个分区存在。
// 并在 infra.db_adapter.cppm 中 export import 我（如果 adapter 是主接口文件）。
// 或者，我只是一个独立的模块 'course_system:infrastructure.course_proxy' ?
// 按照 [v3.0] 分工，建议的是 'infrastructure/infra.course_proxy.cppm'。
// 
// 为了简单起见，且遵循 C++ Modules 最佳实践：
// 让 'infrastructure' 成为一个包模块。
// 修改 infra.db_adapter.cppm 为 infrastructure:db_adapter 分区
// 创建一个新的 infrastructure.cppm 主接口文件来聚合它们。
// 
// 但这改动有点大。
// 
// 替代方案：
// 保持 infra.db_adapter.cppm 为 'module infrastructure' (主模块)
// 新增的 Proxy 作为 'module infrastructure.course_proxy' (独立模块，非分区)
// 或者作为分区，然后在主模块中重新导出。
// 
// 让我们采用最稳妥的方式：
// Proxy 作为一个独立模块 'infrastructure_layer' 的一部分？
// 不，还是让它作为 'infrastructure' 模块的分区比较好。
// 
// 假设 'infrastructure' 模块定义在 infra.db_adapter.cppm 中。
// 我不能在那里 import 我自己。
// 
// 让我们先看 infra.db_adapter.cppm 的内容。
// 它是 'export module infrastructure;'
// 这意味着它是主模块单元。
// 如果我写 'export module infrastructure:course_proxy;'
// 那么我必须被主模块文件 'export import' 或者至少被编译。
// 
// 既然目前 infra.db_adapter.cppm 已经存在且被 Application 使用。
// 我将新建文件，并定义为 `export module infrastructure_params;` 或者类似的，或者直接修改 infra.db_adapter.cppm 把它变成一个分区，然后创建一个新的主 infrastructure.cppm。
// 
// 让我们执行“创建新的主 infrastructure.cppm” 方案，这是最清晰的。
// 
// 步骤：
// 1. 重命名 infra.db_adapter.cppm 中的 module 定义为 'export module infrastructure:db_adapter;'
// 2. 创建 infra.course_proxy.cppm 定义为 'export module infrastructure:course_proxy;'
// 3. 创建 infra.student_proxy.cppm 定义为 'export module infrastructure:student_proxy;'
// 4. 创建 infrastructure.cppm 定义为 'export module infrastructure;' 并 export import 各个分区。
// 
// 这样 Application 层只需要 import infrastructure; 就能用到所有的类。

import std;

export namespace infra { // 使用 namespace 区分

class CourseProxy {
public:
    // 根据 ID 查找课程
    static std::unique_ptr<Course> findCourseById(db::DBAdapter& db, std::string_view id);

    // 获取所有课程
    static std::vector<std::unique_ptr<Course>> findAllCourses(db::DBAdapter& db);

    // 保存新课程
    static bool addCourse(db::DBAdapter& db, const Course& course);
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
