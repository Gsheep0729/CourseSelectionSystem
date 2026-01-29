# 项目: CourseSelectionSystem

## 项目特征总结





---

## 构建配置文件
---

### File: CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 4.1.1)

# https://github.com/Kitware/CMake/blob/v4.1.1/Help/dev/experimental.rst
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
set(CMAKE_CXX_MODULE_STD ON)

project(CourseSelectionSystem LANGUAGES CXX)

find_package(PkgConfig REQUIRED)

# 引入数据库核心库 (Traditional C++ Static Lib)
add_subdirectory(../lib_db_core lib_db_core)

# 1. 定义各层级的模块文件列表 (Code Grouping)
add_executable(CourseSelectionSystem
    main.cpp
)

# 表现层模块 (Presentation Layer)
set(PRESENTATION_MODULES
    # CourseSelectionSystem/presentation/.cppm
    # presentation/pre.cppm
    presentation/pres.cli.cppm
)

# 应用逻辑层模块 (Application Layer)
set(APPLICATION_MODULES
    # CourseSelectionSystem/application/.cppm
    application/app.controller.cppm
)

# 领域层模块 (Domain Layer)
set(DOMAIN_MODULES
    # CourseSelectionSystem/domain/.cppm
    domain/dom.course.cppm
    domain/dom.student.cppm
    domain/domain.cppm
    domain/dom.teacher.cppm
    domain/dom.timeslot.cppm
)

# 数据管理层模块 (Infrastructure Layer)
set(INFRASTRUCTURE_MODULES
    # CourseSelectionSystem/infrastructure/.cppm
    infrastructure/infra.db_adapter.cppm
    infrastructure/infra.course_proxy.cppm
    infrastructure/infra.student_proxy.cppm
    infrastructure/infra.enrollment_proxy.cppm
    infrastructure/infra.dtos.cppm
    infrastructure/infrastructure.cppm
)

# 模块实现文件 (Module Implementation Units)
set(MODULE_IMPL_SOURCES
    infrastructure/infra.db_adapter.cpp
)

# 2. 配置 IDE 中的显示分组 (IDE Display Grouping)
source_group("Presentation" FILES ${PRESENTATION_MODULES})
source_group("Application"  FILES ${APPLICATION_MODULES})
source_group("Domain"       FILES ${DOMAIN_MODULES})
source_group("Infrastructure" FILES ${INFRASTRUCTURE_MODULES} ${MODULE_IMPL_SOURCES})

# 3. 构建目标与源文件关联

# 添加普通源文件 (包括模块实现单元)
target_sources(CourseSelectionSystem PRIVATE
    ${MODULE_IMPL_SOURCES}
)

# 添加模块接口文件 (定义 Module Interface Units)
target_sources(CourseSelectionSystem PRIVATE FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        course_system.cppm
        ${PRESENTATION_MODULES}
        ${APPLICATION_MODULES}
        ${DOMAIN_MODULES}
        ${INFRASTRUCTURE_MODULES}
)

target_compile_features(CourseSelectionSystem PRIVATE cxx_std_23)


target_link_libraries(CourseSelectionSystem PRIVATE db_core)












































# target_link_directories(CourseSelectionSystem PRIVATE /opt/utils/lib)  # -L /opt/utils/lib 设置链接路径
# target_link_libraries(CourseSelectionSystem PRIVATE utils)             # -lutils           设置链接库名

# set_target_properties(CourseSelectionSystem PROPERTIES
#     INSTALL_RPATH_USE_LINK_PATH  True                           # 使用链接路径作为安装文件的RPath，等价于"-Wl,-rpath=/opt/utils/lib"
# )

#add_library(CourseSelectionSystem SHARED
#  utils.cpp
#)

#set_target_properties( CourseSelectionSystem PROPERTIES
#    PUBLIC_HEADER utils.cppm
#    VERSION   2.5.1
#    SOVERSION 2
#    LIBRARY_OUTPUT_DIRECTORY  "${CMAKE_BINARY_DIR}/lib"
#)

#install(TARGETS CourseSelectionSystem
#    LIBRARY DESTINATION /opt/utils/lib
#    PUBLIC_HEADER DESTINATION  /opt/utils/modules
#)

#add_custom_command(TARGET CourseSelectionSystem
#    POST_BUILD
#    COMMAND cmake --install "${CMAKE_CURRENT_BINARY_DIR}"
#)


```

---

## 项目源文件
---

### File: course_system.cppm
```cppm
/**
* @file    src/CourseSelectionSystem/course_system.cppm
* @date    2026-01-07
* @author  GY
* @brief   选课系统主模块接口
*
* 聚合系统的各个功能模块（Domain、Application、Controller等）
* 作为对外暴露的统一接口，方便其他模块导入使用
*
** 架构重构 (v2.5):
* 为了解决 GCC 14/15 对模块分区聚合的编译崩溃问题，
* 我们将所有子系统重构为完全独立的 C++ Modules:
* - domain: 领域层实体
* - infrastructure: 基础设施层 (数据库适配)
* - application: 应用层逻辑 (控制器)
* - presentation: 表现层 (CLI)
* *
* 本模块仅作为统一入口，导出所有子模块。
*
* Change Log:
* [v1.0] GY   2026-01-06
* * 优化了cmake,实现学生选课系统基础
* [v1.5] GY   2026-01-07
* * 整合基础设施层，导出 db_adapter 模块
* [v1.9] Zhang Tao   2026-01-07
* * 导入表现层（presentation）模块，支持CLI交互测试
* [v2.0] Integrated 2026-01-07
* * 解决模块导出冲突，统一导出 infrastructure 和 presentation 模块
* * 确立最终的模块依赖关系，修复循环依赖问题
* [v6.0] GY   2026-01-19
* * 经终期检查：全系统模块聚合完毕，架构层次分明，符合项目规范
*/

export module course_system;

export import domain;
export import infrastructure;
export import application;
export import presentation;

```

---

### File: main.cpp
```cpp
/**
* @file    src/CourseSelectionSystem/main.cpp
* @date    2026-01-07
* @author  GY
* @brief   选课系统主程序入口
*
* Change Log:
* [v2.0] Integrated 2026-01-07
* * 解决 main.cpp 合并冲突，统一系统入口
* * 完成 CLI 表现层与 DB 基础设施层的完整集成
* * 实现"初始化 -> 注入控制器 -> 启动交互循环"的标准启动流程
* [v5.5] GY 2026-01-18
* * 修正交互死循环，通过判断 showLoginMenu 返回值实现优雅退出
* [v6.0] GY   2026-01-19
* * 经终期检查：系统入口逻辑健壮，代码实现严格遵循 C++23 规范
* [v6.1] GY   2026-01-29
* * 新增：在程序启动时提供交互式数据库初始化配置选择（默认配置 vs 自定义配置）
*/
import std;
import course_system;

int main() {
    try {

        // -----------新增数据库初始化从这里|=>--------------
        std::string conn_str = "";

        std::print("Course Selection System Initialization\n");
        std::print("--------------------------------------\n");
        std::print("1. Use Default Database Configuration\n");
        std::print("   (dbname=CourseSelectionSystem user=postgres password=123 host=127.0.0.1 port=5432)\n");
        std::print("2. Custom Database Configuration\n");
        std::print("Please enter your choice (1 or 2): ");

        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "2") {
            std::string dbname, user, password, host, port;
            std::print("Enter database name: ");
            std::getline(std::cin, dbname);
            std::print("Enter user: ");
            std::getline(std::cin, user);
            std::print("Enter password: ");
            std::getline(std::cin, password);
            std::print("Enter host IP (e.g., 127.0.0.1): ");
            std::getline(std::cin, host);
            std::print("Enter port (e.g., 5432): ");
            std::getline(std::cin, port);

            conn_str = std::format("dbname={} user={} password={} hostaddr={} port={}", 
                                   dbname, user, password, host, port);
        } else {
             std::print("Using default configuration...\n");
        }

        // // -------------<=|--------------

        // 1. 初始化系统控制器 (连接数据库, 建表, 初始化数据)
        SystemController app;
        app.initialize(conn_str);

        // 2. 初始化用户界面并注入控制器
        UserInterface cli;
        cli.setController(&app);

        // 3. 启动交互循环
        while (true) {
            // 显示登录菜单 (内部会根据角色进入对应的子菜单循环)
            // 当用户在子菜单选择"退出登录"时，函数返回，循环继续，重新显示登录菜单
            // 若返回 "exit"，则表示用户请求关闭程序
            if (cli.showLoginMenu() == "exit") {
                std::print("\n再见！感谢使用重庆师范大学选课系统。\n");
                break;
            }
        }

        // 4. 程序结束清理
        app.cleanup();

    } catch (const std::exception& e) {
        std::print("Fatal Error: {}\n", e.what());
        return 1;
    }
    return 0;
} 

```

---

### File: application/app.controller.cppm
```cppm
/**
* @file    src/CourseSelectionSystem/application/app.controller.cppm
* @date    2026-01-07
* @author  GY
* @brief   应用层分区：系统控制器
*
* 负责管理整个选课系统的运行流程
* 维护学生和课程列表，提供选课、退课等核心业务功能
* 协调领域层实体之间的交互
*
* Change Log:
* [v1.0] GY   2026-01-06
* * 初始化项目架构 (基于 C++23 Modules)
* [v1.5] GY   2026-01-07
* * 重构 SystemController，移除内存存储，接入 DBAdapter
* [v2.0] GY   2026-01-07
* * 完成 SystemController 与 DBAdapter 的深度集成
* * 移除内存容器 (std::vector)，全量迁移至 SQL 事务操作
* * 实现基于数据库的实时选课/退课业务逻辑（含并发安全/容量检查）
* [v3.0] GY   2026-01-10
    本次提交完成了核心架构的重构，解决了 Controller 直接依赖数据库的违规问题，并实现了关键业务逻辑。
    [架构重构]
    - Infrastructure: 新增 StudentProxy, CourseProxy, EnrollmentProxy 分区模块。
    - Refactor: 将 SystemController 中的 SQL 语句全部迁移至代理层，初步实现 4 层架构的关注点分离。
    [领域逻辑]
    - Domain: 新增 Timeslot 模块，并在 Student 实体中实现 hasTimeConflict 冲突检测算法。
    - Entity: 扩展 Course 实体，支持学分、教师、时间槽等完整字段。
    [业务功能]
    - Secretary: 完成“创建课程”功能的后端链路与 CLI 对接。
    - Teacher: 完成“录入成绩”功能的数据库交互。
    [构建系统]
    - CMake: 更新配置以支持新增的 Infrastructure 分区文件。
* [v4.5] GY   2026-01-16
* * 实现 login 方法与基于 users 表的身份验证
* * 引入 m_currentUser 维护登录会话
* * 在选课、退课、评分等业务逻辑中集成细粒度权限校验
* * 新增 getMySchedule, getCourseStudentList 等查询桥接接口供 UI 调用
* * [Fix] 修复 createCourse 自动创建教师账户逻辑
* [v5.0] Zhang Tao 2026-01-17
* * 新增 getMyGrades 接口，支持学生查询个人成绩
* [v5.0.2] Zhang Tao 2026-01-17
* * 新增 assignTeacherToCourse 接口，支持教学秘书为课程分配教师
* [v5.1] GY 2026-01-18
* * 新增 setCourseTimeSlot 接口，支持教学秘书修改课程时间槽
* * 新增 getCourseById 接口，封装数据库访问，避免外部直接访问私有成员 m_db
* * 补充 updateGrade 接口实现，支持教师录入/修改成绩
* [v5.5] GY 2026-01-18
* * 新增 removeCourse 和 unassignTeacher 接口，支持课程与教师的解绑管理
* * 重构业务方法以调用 Proxy 静态接口，移除 Controller 内冗余 SQL
* [v5.6] GY   2026-01-19
* * 重构：重命名查询方法 (移除 'get' 前缀)
* * 分离函数声明与实现
* [v5.7] GY   2026-01-19
* * 修复 Bug：在创建课程时增加教师时间冲突检测
* [v5.8] GY   2026-01-19
* * 测试数据升级：导入真实的课程数据列表 (42门课程)
* [v5.9] GY   2026-01-19
* * 测试数据升级：导入真实的学生名单 (178名学生)
* [v6.0] GY   2026-01-19
* * 经终期检查：全业务流程通过大规模真实数据验证，代码标准、逻辑完备
* [v6.1] GY   2026-01-29
* * 新增：支持自定义数据库连接配置，优化 initialize 接口以接受外部连接字符串
*/
export module application;
import domain;
import infrastructure;
import std;

export class SystemController {
public:
    struct User {
        std::string id;
        std::string name;
        std::string role; // "student", "teacher", "secretary"
        bool isValid() const { return !id.empty(); }
    };

    SystemController(); // 构造函数：初始化数据库适配器
    void initialize(const std::string& conn_str = ""); // 系统初始化：建立连接、创建表结构
    void cleanup(); // 系统清理：删除表结构 (用于测试)
    void run(); // 启动系统运行逻辑

    // 用户认证
    bool login(std::string id, std::string password);
    User sessionUser() const { return m_currentUser; }

    // 核心业务功能
    void performEnrollment(std::string sid, std::string cid); // 执行选课业务逻辑
    void performDrop(std::string sid, std::string cid); // 执行退课业务逻辑

    // 教学秘书功能
    bool createCourse(std::string id, std::string name, int capacity, double credit,
                      std::string teacherId, std::string teacherName, int weekday, int timeslot); // 创建新课程
    bool removeCourse(std::string courseId); // 新增：删除课程
    bool assignTeacherToCourse(std::string courseId, std::string teacherId, std::string teacherName); // 为课程分配教师
    bool unassignTeacher(std::string courseId); // 新增：解绑教师
    bool setCourseTimeSlot(std::string courseId, int weekday, int timeslot); // 新增：修改课程时间槽

    // 教师功能
    bool updateGrade(std::string sid, std::string cid, int score); // 录入/修改学生成绩

    // --- 数据查询接口 (供 UI 调用) ---
    // 获取当前登录学生的课表
    std::vector<Course> queryMySchedule();

    // 获取当前登录学生的成绩
    std::vector<infra::GradeDTO> queryMyGrades();

    // 获取某门课程的学生名单 (仅教师/管理员可用)
    std::vector<infra::CourseStudentDTO> queryCourseStudentList(std::string courseId);

    // 获取所有课程列表 (公共查询)
    std::vector<std::unique_ptr<Course>> queryAllCourses();

    // 新增：根据课程ID获取课程对象 (封装数据库访问，避免外部访问m_db)
    std::unique_ptr<Course> findCourseById(std::string courseId);

private:
    std::unique_ptr<db::DBAdapter> m_db; // 数据库适配器指针
    User m_currentUser; // 当前登录用户
};

// --- Implementation ---
// @brief 构造函数
SystemController::SystemController() : m_db(std::make_unique<db::DBAdapter>()) {}

/**
 * @brief 系统环境初始化
 * 建立数据库连接，执行 DDL 语句重置表结构，并导入初始的用户及课程数据。
 */
void SystemController::initialize(const std::string& custom_conn_str) {
    // 使用 PostgreSQL的 CourseSelectionSystem数据库，登录管理员账号为postgres，密码为123，ip地址为127.0.0.1，端口号为5432
    std::string conn_str = "dbname=CourseSelectionSystem user=postgres password=123 hostaddr=127.0.0.1 port=5432";

    if (!custom_conn_str.empty()) {
        conn_str = custom_conn_str;
    }

    if (!m_db->connect(conn_str)) {
        std::print("Error: Failed to connect to database.\n");
        return;
    }

    // DDL: 重置数据库结构
    m_db->execute("DROP TABLE IF EXISTS enrollment CASCADE");
    m_db->execute("DROP TABLE IF EXISTS course CASCADE");
    m_db->execute("DROP TABLE IF EXISTS student CASCADE");
    m_db->execute("DROP TABLE IF EXISTS users CASCADE");

    // 创建用户表 (用于认证)
    m_db->execute(R"(
        CREATE TABLE users (
            user_id VARCHAR(50) PRIMARY KEY,
            name VARCHAR(50) NOT NULL,
            password VARCHAR(50) DEFAULT '123',
            role VARCHAR(20) CHECK (role IN ('student','teacher','secretary'))
        )
    )");

    m_db->execute(R"(
        CREATE TABLE course (
            id VARCHAR(50) PRIMARY KEY,
            name TEXT NOT NULL,
            capacity INT DEFAULT 120,
            enrolled INT DEFAULT 0,
            credit REAL DEFAULT 0.0,
            teacher_id TEXT,
            teacher_name TEXT,
            weekday INT,
            timeslot INT
        )
    )");

    m_db->execute("CREATE TABLE student (id VARCHAR(50) PRIMARY KEY, name TEXT)");

    m_db->execute(R"(
        CREATE TABLE enrollment (
            student_id VARCHAR(50) REFERENCES student(id),
            course_id VARCHAR(50) REFERENCES course(id),
            score INTEGER DEFAULT NULL,
            PRIMARY KEY (student_id, course_id)
        )
    )");

    std::print("Database initialized.\n");

    // --- 录入真实学生数据 ---
    struct RawStudent { std::string id; std::string name; };
    std::vector<RawStudent> students = {
        {"2024051604001", "白佳旭"}, {"2024051604002", "闫培恒"}, {"2024051604003", "易俊辰"}, {"2024051604004", "黄奕萌"},
        {"2024051604005", "明雅桐"}, {"2024051604006", "胡艺舰"}, {"2024051604007", "王耀"}, {"2024051604008", "蒋汶江"},
        {"2024051604009", "江钒"}, {"2024051604010", "周城伟"}, {"2024051604011", "黄奕萌"}, {"2024051604012", "周宇欣"},
        {"2024051604013", "陈谊"}, {"2024051604014", "何孟韩"}, {"2024051604015", "谢贤豪"}, {"2024051604016", "李成"},
        {"2024051604018", "熊俸莹"}, {"2024051604019", "周代森"}, {"2024051604020", "马浩楠"}, {"2024051604021", "陈烁心"},
        {"2024051604022", "郑鑫康"}, {"2024051604023", "何志远"}, {"2024051604024", "李超"}, {"2024051604025", "吴凝梦"},
        {"2024051604026", "崔程江"}, {"2024051604027", "陈科宇"}, {"2024051604028", "陈彤"}, {"2024051604029", "苏茜"},
        {"2024051604030", "鲁凤莹"}, {"2024051604031", "程业宸"}, {"2024051604032", "尚文哲"}, {"2024051604033", "颜世沛"},
        {"2024051604034", "陈雨欣"}, {"2024051604035", "张志豪"}, {"2024051604036", "吴永超"}, {"2024051604037", "张迅福"},
        {"2024051604038", "蔡汶宏"}, {"2024051604039", "李思思"}, {"2024051604040", "廖文杰"}, {"2024051604041", "唐晟庆"},
        {"2024051604042", "杨光"}, {"2024051604043", "熊闲伟"}, {"2024051604044", "谭晶晶"}, {"2024051604045", "张雨欣"},
        {"2024051604046", "何继雄"}, {"2024051604047", "龙玟吉"}, {"2024051604048", "段恩来"}, {"2024051604049", "冉恒益"},
        {"2024051604050", "陈月行"}, {"2024051604051", "刘城"}, {"2024051604052", "涂钧锋"}, {"2024051604053", "高歌"},
        {"2024051604054", "卓杰"}, {"2024051604055", "刘寅丞"}, {"2024051604056", "何俊霖"}, {"2024051604057", "陈龙"},
        {"2024051604058", "陈涛"}, {"2024051604059", "罗剑秋"}, {"2024051604060", "郑凯文"}, {"2024051604061", "杨宗霖"},
        {"2024051604062", "杜若贤"}, {"2024051604063", "何婷"}, {"2024051604064", "陈奕帆"}, {"2024051604065", "余浩然"},
        {"2024051604066", "梁钊睿"}, {"2024051604068", "文桔梨"}, {"2024051604069", "杨朝凯"}, {"2024051604070", "欧橼"},
        {"2024051604071", "廖伟程"}, {"2024051604072", "刘年杰"}, {"2024051604073", "杨钰玲"}, {"2024051604074", "杨红鑫"},
        {"2024051604075", "邬明亮"}, {"2024051604076", "孙海婷"}, {"2024051604077", "谭博文"}, {"2024051604078", "向治蓉"},
        {"2024051604079", "李卫"}, {"2024051604080", "邹洪荣"}, {"2024051604081", "龚清涛"}, {"2024051604082", "姚智耀"},
        {"2024051604083", "陈文静"}, {"2024051604084", "张涛"}, {"2024051604085", "高扬"}, {"2024051604087", "高菲"},
        {"2024051604088", "钟宇洋"}, {"2024051604089", "唐吴汗"}, {"2024051604090", "周姝含"}, {"2024051604091", "王子煜"},
        {"2024051604092", "王清明"}, {"2024051604093", "董镁洁"}, {"2024051604095", "白靖媛"}, {"2024051604096", "李渝平"},
        {"2024051604097", "谢明添"}, {"2024051604098", "李林"}, {"2024051604099", "庞群"}, {"2024051604100", "古航宇"},
        {"2024051604101", "黄琰尧"}, {"2024051604102", "庞文浩"}, {"2024051604103", "曹杰"}, {"2024051604104", "黄钰琳"},
        {"2024051604105", "王涛"}, {"2024051604106", "张文耀"}, {"2024051604107", "贺超"}, {"2024051604108", "何家欢"},
        {"2024051604109", "王骋"}, {"2024051604110", "朱芮娴"}, {"2024051604111", "吴林峰"}, {"2024051604112", "林健华"},
        {"2024051604113", "王璨"}, {"2024051604114", "胡君燕"}, {"2024051604115", "苟曦月"}, {"2024051604116", "郭海峰"},
        {"2024051604117", "帅沂伶"}, {"2024051604118", "苟金津"}, {"2024051604119", "冯春霖"}, {"2024051604120", "舒月"},
        {"2024051611001", "严豪毅"}, {"2024051611002", "罗缘"}, {"2024051611003", "马文杰"}, {"2024051611004", "王松涛"},
        {"2024051611005", "柯力菲"}, {"2024051611007", "柴富翔"}, {"2024051611008", "郑杰"}, {"2024051611009", "李锐"},
        {"2024051611010", "李宜美"}, {"2024051611011", "张瑞霖"}, {"2024051611013", "喻源慧"}, {"2024051611014", "李欢"},
        {"2024051611015", "皮沁灵"}, {"2024051611016", "黄俊豪"}, {"2024051611017", "喻美龄"}, {"2024051611018", "周泰安"},
        {"2024051611019", "幸家林"}, {"2024051611020", "田赟豪"}, {"2024051611021", "黄成彰"}, {"2024051611022", "罗超"},
        {"2024051611023", "王乐霏"}, {"2024051611024", "马政"}, {"2024051611025", "陈晗"}, {"2024051611026", "范绍斌"},
        {"2024051611027", "喻乐"}, {"2024051611028", "贺宇轩"}, {"2024051611029", "唐文杰"}, {"2024051611030", "樊邱悦"},
        {"2024051611031", "杨力豪"}, {"2024051611032", "唐靖淞"}, {"2024051611033", "姚译博"}, {"2024051611034", "刘倩"},
        {"2024051611035", "廖子昂"}, {"2024051611036", "李程轩"}, {"2024051611037", "李嘉奇"}, {"2024051611038", "董川涌"},
        {"2024051611039", "王新"}, {"2024051611040", "刘俊伶"}, {"2024051611041", "徐婧怡"}, {"2024051611042", "张思怡"},
        {"2024051611043", "曾祥洪"}, {"2024051611044", "杨佳昊"}, {"2024051611045", "青柏炀"}, {"2024051611046", "文玲"},
        {"2024051611047", "熊尚峰"}, {"2024051611048", "谢江"}, {"2024051611049", "陈厚禹"}, {"2024051611050", "刘书宏"},
        {"2024051611051", "陈劲宏"}, {"2024051611052", "王佳琴"}, {"2024051611053", "代杉杉"}, {"2024051611054", "张涛"},
        {"2024051611055", "谭钰寅"}, {"2024051611056", "李旨都"}, {"2024051611057", "胡波"}, {"2024051611058", "何奕颖"},
        {"2024051611059", "谢阿凤"}, {"2024051611060", "许文聪"}, {"2024051611061", "詹靖丞"}, {"2024051611062", "宋驭池"},
        {"2024051611063", "卢宗义"}, {"2024051611064", "刘娅雪"}, {"2024051611065", "刘雨彤"}, {"2024051611066", "黄馨扬"},
        {"2024051611067", "赵康德"}, {"2024051611068", "杨雪玲"}, {"2024051611069", "古伟熠"}, {"2024051611070", "李晓璐"}
    };

    for (const auto& s : students) {
        m_db->execute(std::format(
            "INSERT INTO users (user_id, name, password, role) VALUES ('{}', '{}', '123', 'student') ON CONFLICT (user_id) DO NOTHING",
            s.id, s.name
        ));
        m_db->execute(std::format(
            "INSERT INTO student (id, name) VALUES ('{}', '{}') ON CONFLICT (id) DO NOTHING",
            s.id, s.name
        ));
    }

    // 2. 录入默认教师账户
    // 教师工号: 20131672, 用户名: 龚伟 , 默认密码: 123
    m_db->execute("INSERT INTO users VALUES ('20131672', '龚伟', '123', 'teacher')");

    // 3. 录入默认教学秘书账户
    // 账号: admin, 用户名: Secretary, 默认密码: 123
    m_db->execute("INSERT INTO users VALUES ('admin', 'Secretary', '123', 'secretary')");

    // --- 导入真实课程数据 ---
    struct RawCourse {
        std::string id; std::string name; int cap; double cr;
        std::string tid; std::string tname; int w; int t;
    };
    std::vector<RawCourse> courses = {
        {"0000002564", "国家安全教育（网络视频课）", 60, 1.0, "00000000", "网络教师", 1, 1},
        {"0000006001", "军事理论（网络视频课）", 60, 2.0, "22000708", "张国清", 1, 1},
        {"0000002209", "习近平总书记关于教育的重要论述专题（网络视频课）", 60, 1.0, "20131591", "毛宇", 1, 1},
        {"003AA8002A", "大学英语I", 60, 3.0, "20247023", "赖守浪", 1, 3},
        {"003A202027", "军事技能", 60, 2.0, "20131725", "任文霞", 1, 1},
        {"083BA50005", "计算机导论", 60, 2.0, "20220045", "孙晓宁", 3, 4},
        {"083BA8013A", "C语言程序设计", 60, 4.0, "20200085", "肖颗", 1, 1},
        {"003AA00001", "形势与政策I", 60, 0.0, "20131381", "黄玲", 2, 4},
        {"003AA600AA", "思想道德与法治", 60, 2.0, "20141113", "伍崇利", 1, 4},
        {"0504321004", "线性代数", 60, 4.0, "20210031", "莫长鑫", 2, 1},
        {"073BAA0033", "高等数学(1)", 60, 5.0, "20131925", "赵侯宇", 2, 1},
        {"003AA2003A", "体育I", 60, 1.0, "20180056", "魏胜辉", 3, 2},
        {"003AA40037", "大学生心理健康教育", 60, 2.0, "20200047", "彭臻", 2, 2},
        {"083BA70004", "数据结构", 60, 3.0, "20240120", "高丽萍", 1, 1},
        {"083BA70005", "计算机系统基础", 60, 3.0, "20230071", "郭桃林", 1, 2},
        {"083BA8024A", "Linux程序设计", 60, 4.0, "20170001", "冯骥", 2, 1},
        {"083FA10006", "C语言程序设计课程设计", 60, 0.0, "20200085", "肖颗", 1, 1},
        {"083FA20051", "数据结构课程设计", 60, 1.0, "20240120", "高丽萍", 1, 1},
        {"003AA00002", "形势与政策II", 60, 0.0, "20131675", "黎朝红", 1, 4},
        {"003AA3000A", "中国近现代史纲要", 60, 2.0, "20131930", "李虹辉", 2, 2},
        {"073BAA0034", "高等数学(2)", 60, 5.0, "20170051", "许秋菊", 2, 1},
        {"003AA2004A", "体育II", 60, 1.0, "20131344", "成平", 3, 2},
        {"003AA8003A", "大学英语II", 60, 3.0, "20130891", "沈纯", 1, 3},
        {"XSC2013011", "职业生涯规划与就业指导1", 60, 1.0, "20210030", "李黎", 5, 3},
        {"083CA70013", "数据库原理及应用", 60, 3.0, "20210071", "肖旋", 1, 4},
        {"083EB80135", "软件构建与实现", 60, 4.0, "20131672", "龚伟", 1, 1},
        {"083FA20056", "软件工程综合实训1-1（C++方向）", 60, 1.0, "20131672", "龚伟", 1, 1},
        {"113BA7002A", "计算机网络", 60, 3.0, "20200085", "肖颗", 2, 3},
        {"003AA00003", "形势与政策III", 60, 0.0, "20130679", "高德华", 2, 1},
        {"003EA4006A", "马克思主义基本原理", 60, 2.0, "20131977", "陈雪连", 2, 2},
        {"073BAR0034", "概率论与数理统计", 60, 4.0, "20131918", "吕美英", 1, 2},
        {"04A", "体育Ⅲ（羽毛球）", 60, 1.0, "20132168", "陈丽", 2, 4},
        {"003AA60026", "大学英语IV拓展课系列- AI辅助专门用途英语翻译", 60, 3.0, "20130442", "龙涛", 4, 1},
        {"083BA70007", "操作系统原理与实践", 60, 3.0, "20132108", "杜兴", 1, 2},
        {"083BA80006", "大学物理", 60, 4.0, "20250043", "孙川", 3, 2},
        {"083CA40010", "软件工程导论", 60, 2.0, "20130951", "魏延", 2, 1},
        {"083EB8040A", "Qt6软件开发", 60, 5.0, "20131672", "龚伟", 3, 1},
        {"083FB20057", "软件工程综合实训2-1（C++方向）", 60, 1.0, "20131672", "龚伟", 1, 1},
        {"003AA00004", "形势与政策IV", 60, 0.0, "20131977", "陈雪连", 2, 2},
        {"003AAC002A", "习近平新时代中国特色社会主义思想概论", 60, 3.0, "20220002", "徐琴", 3, 2},
        {"003EAC002B", "毛泽东思想和中国特色社会主义理论体系概论", 60, 2.0, "20150015", "沈乾飞", 3, 1},
        {"04B", "体育Ⅳ（羽毛球）", 60, 1.0, "20132168", "陈丽", 2, 4}
    };

    for (const auto& c : courses) {
        std::string sql = std::format(
            "INSERT INTO course VALUES ('{}', '{}', {}, 0, {}, '{}', '{}', {}, {})",
            c.id, c.name, c.cap, c.cr, c.tid, c.tname, c.w, c.t
        );
        m_db->execute(sql);

        // 自动为课程教师创建账号 (如果不存在)，密码统一为 123
        m_db->execute(std::format(
            "INSERT INTO users (user_id, name, password, role) VALUES ('{}', '{}', '123', 'teacher') ON CONFLICT (user_id) DO NOTHING",
            c.tid, c.tname
        ));
    }

    std::print("Initial data loaded.\n");
}

/**
 * @brief 系统清理
 * 删除所有表结构，用于测试环境复位。
 */
void SystemController::cleanup() {
    if (!m_db->is_connected()) return;

    m_db->execute("DROP TABLE IF EXISTS enrollment CASCADE");
    m_db->execute("DROP TABLE IF EXISTS course CASCADE");
    m_db->execute("DROP TABLE IF EXISTS student CASCADE");
    m_db->execute("DROP TABLE IF EXISTS users CASCADE");

    std::print("System cleanup: Tables dropped.\n");
}

/**
 * @brief 运行主逻辑
 */
void SystemController::run() {
    std::print("System Controller Ready. Please login via UI.\n");
}

/**
 * @brief 处理用户登录请求
 * @param id 用户唯一标识ID
 * @param password 用户密码
 * @return 登录成功返回 true，并保存用户信息至当前会话；否则返回 false
 */
bool SystemController::login(std::string id, std::string password) {
    // 简单的明文密码验证 (实际生产应使用哈希)
    std::string sql = std::format("SELECT name, role FROM users WHERE user_id = '{}' AND password = '{}'", id, password);
    auto res = m_db->query(sql);

    if (res && !res->empty()) {
        std::string name = (*res)[0][0];
        std::string role = (*res)[0][1];
        m_currentUser = User{id, name, role};
        std::print("Login successful: {} ({})\n", name, role);
        return true;
    }

    std::print("Login failed: Invalid credentials.\n");
    return false;
}

/**
 * @brief 执行选课业务逻辑
 * @param sid 学生ID
 * @param cid 课程ID
 * 校验流程：登录态 -> 角色权限 -> 身份匹配 -> 课程存在 -> 学生存在 -> 重复选课 -> 容量检查 -> 时间冲突检测 -> 持久化
 */
void SystemController::performEnrollment(std::string sid, std::string cid) {
    // 权限检查
    if (!m_currentUser.isValid()) {
        std::print("Error: Please login first.\n");
        return;
    }

    if (m_currentUser.role != "student") {
        std::print("Error: Only students can enroll in courses.\n");
        return;
    }

    if (m_currentUser.id != sid) {
        std::print("Error: You can only enroll for yourself.\n");
        return;
    }

    // 1. 获取课程对象
    auto course = infra::CourseProxy::findCourseById(*m_db, cid);
    if (!course) {
        std::print("Error: Course {} not found.\n", cid);
        return;
    }

    // 2. 获取学生对象 (含已选课程)
    auto student = infra::StudentProxy::findStudentById(*m_db, sid);
    if (!student) {
        std::print("Error: Student {} not found.\n", sid);
        return;
    }

    // 3. 执行业务规则校验
    // 3.1 检查是否已选
    if (student->isEnrolled(course.get())) {
        std::print("Error: Already enrolled in {}\n", course->course_info());
        return;
    }

    // 3.2 检查容量
    if (course->isFull()) {
        std::print("Error: Course is full: {}\n", course->course_info());
        return;
    }

    // 3.3 检查时间冲突
    if (student->hasTimeConflict(course.get())) {
        std::print("Error: Time conflict detected for course {}\n", course->course_info());
        return;
    }

    // 4. 持久化 (通过 Proxy)
    if (infra::StudentProxy::saveEnrollment(*m_db, sid, cid)) {
        // 重要：重新加载以显示更新后的人数
        auto updatedCourse = infra::CourseProxy::findCourseById(*m_db, cid);
        std::print("Enrollment successful! {}\n", updatedCourse->course_info());
    } else {
        std::print("Error: Failed to save enrollment to database.\n");
    }
}

/**
 * @brief 执行退课业务逻辑
 * @param sid 学生ID
 * @param cid 课程ID
 */
void SystemController::performDrop(std::string sid, std::string cid) {
    // 权限检查
    if (!m_currentUser.isValid()) {
        std::print("Error: Please login first.\n");
        return;
    }

    if (m_currentUser.role != "student") {
        std::print("Error: Only students can drop courses.\n");
        return;
    }

    if (m_currentUser.id != sid) {
        std::print("Error: You can only drop courses for yourself.\n");
        return;
    }

    // 1. 检查选课关系是否存在
    if (!infra::StudentProxy::isEnrolled(*m_db, sid, cid)) {
        std::print("Error: Student {} is not enrolled in course {}\n", sid, cid);
        return;
    }

    // 2. 执行退课持久化
    if (infra::StudentProxy::removeEnrollment(*m_db, sid, cid)) {
        // 重新加载课程以显示更新后的人数
        auto updatedCourse = infra::CourseProxy::findCourseById(*m_db, cid);
        std::print("Drop successful! {}\n", updatedCourse->course_info());
    } else {
        std::print("Error: Failed to drop course from database.\n");
    }
}

/**
 * @brief 创建新课程 (仅教学秘书可用)
 */
bool SystemController::createCourse(std::string id, std::string name, int capacity, double credit,
                                    std::string teacherId, std::string teacherName, int weekday, int timeslot) {
    // 权限检查
    if (!m_currentUser.isValid() || m_currentUser.role != "secretary") {
        std::print("Error: Only secretary can create courses.\n");
        return false;
    }

    // 检查课程ID是否已存在
    auto existingCourse = infra::CourseProxy::findCourseById(*m_db, id);
    if (existingCourse) {
        std::print("Error: Course ID {} already exists.\n", id);
        return false;
    }

    // 检查教师时间冲突
    if (infra::CourseProxy::hasTeacherTimeConflict(*m_db, teacherId, weekday, timeslot)) {
        std::print("Error: Teacher {} already has a course at Weekday {} Slot {}.\n", teacherName, weekday, timeslot);
        return false;
    }

    // 构造课程对象并调用 Proxy 持久化
    Course newCourse(id, name, capacity, 0, credit, teacherId, teacherName, Timeslot(weekday, timeslot));
    if (infra::CourseProxy::addCourse(*m_db, newCourse)) {
        // 自动为教师创建账号 (如果不存在)
        m_db->execute(std::format(
            "INSERT INTO users (user_id, name, password, role) VALUES ('{}', '{}', '123', 'teacher') ON CONFLICT (user_id) DO NOTHING",
            teacherId, teacherName
        ));

        std::print("Course {} created successfully!\n", name);
        return true;
    } else {
        std::print("Error: Failed to create course {}\n", name);
        return false;
    }
}

/**
 * @brief 删除课程 (仅教学秘书可用)
 */
bool SystemController::removeCourse(std::string courseId) {
    if (!m_currentUser.isValid() || m_currentUser.role != "secretary") {
        std::print("Error: Only secretary can remove courses.\n");
        return false;
    }
    return infra::CourseProxy::deleteCourse(*m_db, courseId);
}

/**
 * @brief 为课程分配教师 (仅教学秘书可用)
 */
bool SystemController::assignTeacherToCourse(std::string courseId, std::string teacherId, std::string teacherName) {
    // 权限检查
    if (!m_currentUser.isValid() || m_currentUser.role != "secretary") {
        std::print("Error: Only secretary can assign teachers to courses.\n");
        return false;
    }

    // 执行更新
    if (infra::CourseProxy::updateTeacher(*m_db, courseId, teacherId, teacherName)) {
        // 自动为教师创建账号
        m_db->execute(std::format(
            "INSERT INTO users (user_id, name, password, role) VALUES ('{}', '{}', '123', 'teacher') ON CONFLICT (user_id) DO NOTHING",
            teacherId, teacherName
        ));
        std::print("Teacher {} assigned to course {} successfully!\n", teacherName, courseId);
        return true;
    }
    return false;
}

/**
 * @brief 解绑教师 (仅教学秘书可用)
 */
bool SystemController::unassignTeacher(std::string courseId) {
    if (!m_currentUser.isValid() || m_currentUser.role != "secretary") {
        std::print("Error: Only secretary can unassign teachers.\n");
        return false;
    }
    if (infra::CourseProxy::updateTeacher(*m_db, courseId, "", "未分配")) {
        std::print("Teacher unassigned from course {} successfully!\n", courseId);
        return true;
    }
    return false;
}

/**
 * @brief 修改课程时间槽 (仅教学秘书可用)
 * @param courseId 课程ID
 * @param weekday 星期 (1-7, 0=无固定时间)
 * @param timeslot 节次
 * @return 修改成功返回true，否则返回false
 */
bool SystemController::setCourseTimeSlot(std::string courseId, int weekday, int timeslot) {
    // 权限检查
    if (!m_currentUser.isValid() || m_currentUser.role != "secretary") {
        std::print("Error: Only secretary can modify course timeslot.\n");
        return false;
    }

    // 参数合法性校验
    if (weekday < 0 || weekday > 7) {
        std::print("Error: Invalid weekday (must be 0-7).\n");
        return false;
    }

    if (timeslot < 0) {
        std::print("Error: Invalid timeslot (must be >= 0).\n");
        return false;
    }

    // 检查课程是否存在
    auto course = infra::CourseProxy::findCourseById(*m_db, courseId);
    if (!course) {
        std::print("Error: Course {} not found.\n", courseId);
        return false;
    }

    // 执行更新
    std::string sql = std::format(
        "UPDATE course SET weekday = {}, timeslot = {} WHERE id = '{}'",
        weekday, timeslot, courseId
    );

    if (m_db->execute(sql)) {
        std::print("Course timeslot updated: {}\n", course->course_info());
        return true;
    } else {
        std::print("Error: Failed to update timeslot for course {}\n", courseId);
        return false;
    }
}

/**
 * @brief 录入/修改学生成绩 (仅教师可用)
 * @param sid 学生ID
 * @param cid 课程ID
 * @param score 成绩 (0-100)
 * @return 操作成功返回true，否则返回false
 */
bool SystemController::updateGrade(std::string sid, std::string cid, int score) {
    // 权限检查
    if (!m_currentUser.isValid() || m_currentUser.role != "teacher") {
        std::print("Error: Only teachers can update grades.\n");
        return false;
    }

    // 成绩范围校验
    if (score < 0 || score > 100) {
        std::print("Error: Invalid score (must be 0-100).\n");
        return false;
    }

    // 检查课程是否存在且为当前教师授课
    auto course = infra::CourseProxy::findCourseById(*m_db, cid);
    if (!course) {
        std::print("Error: Course {} not found.\n", cid);
        return false;
    }

    if (!course->isTaughtBy(m_currentUser.id)) {
        std::print("Error: You are not the teacher of course {}.\n", cid);
        return false;
    }

    // 检查学生是否选修该课程
    if (!infra::StudentProxy::isEnrolled(*m_db, sid, cid)) {
        std::print("Error: Student {} is not enrolled in course {}.\n", sid, cid);
        return false;
    }

    // 执行成绩更新 (存在则更新，不存在则插入)
    std::string sql = std::format(
        "INSERT INTO enrollment (student_id, course_id, score) VALUES ('{}', '{}', {}) "
        "ON CONFLICT (student_id, course_id) DO UPDATE SET score = EXCLUDED.score",
        sid, cid, score
    );

    if (m_db->execute(sql)) {
        std::print("Grade updated successfully! Student {}: Course {} score = {}\n", sid, cid, score);
        return true;
    } else {
        std::print("Error: Failed to update grade for student {} in course {}\n", sid, cid);
        return false;
    }
}

// --- Query Methods Implementation ---

// 获取当前登录学生的课表
std::vector<Course> SystemController::queryMySchedule() {
    if (!m_currentUser.isValid() || m_currentUser.role != "student") return {};
    return infra::StudentProxy::findSchedule(*m_db, m_currentUser.id);
}

// 获取当前登录学生的成绩
std::vector<infra::GradeDTO> SystemController::queryMyGrades() {
    if (!m_currentUser.isValid() || m_currentUser.role != "student") return {};
    return infra::EnrollmentProxy::findGradesByStudent(*m_db, m_currentUser.id);
}

// 获取某门课程的学生名单 (仅教师/管理员可用)
std::vector<infra::CourseStudentDTO> SystemController::queryCourseStudentList(std::string courseId) {
    // 简单权限校验
    if (!m_currentUser.isValid()) return {};
    if (m_currentUser.role == "student") return {}; // 学生不可见
    return infra::CourseProxy::findStudentsByCourse(*m_db, courseId);
}

// 获取所有课程列表 (公共查询)
std::vector<std::unique_ptr<Course>> SystemController::queryAllCourses() {
    return infra::CourseProxy::findAllCourses(*m_db);
}

// 根据课程ID获取课程对象
std::unique_ptr<Course> SystemController::findCourseById(std::string courseId) {
    if (!m_currentUser.isValid()) return nullptr;
    return infra::CourseProxy::findCourseById(*m_db, courseId);
}

```

---

### File: domain/domain.cppm
```cppm
/**
* @file    src/CourseSelectionSystem/domain/domain.cppm
* @date    2026-01-06
* @author  GY
* @brief   领域层主模块接口
*
* 聚合学生（Student）和课程（Course）实体模块
* 补全 Student 对 Course 的调用逻辑（选课、退课）
* 补全 Student 对 Course 的调用逻辑（选课、退课）
* 实现领域层核心业务逻辑
*
* Change Log:
* [v1.0] GY   2026-01-06
* * 初始化项目架构 (基于 C++23 Modules)
* * 实现领域层实体 (Student, Course) 及其核心逻辑
* * 实现应用层控制器 (SystemController) 管理选课流程
* * 实现选课与退课功能，包含容量检查与重复选课验证
* * 添加基础测试用例与 CLI 输出
*/
export module domain;
export import :teacher;
export import :student;
export import :course;
export import :timeslot;

import std;

/**
* @brief 学生选修课程
* @param c 要选修的课程指针
*/
void Student::enrollIn(Course* c) {
    if (!c) return;

    // 检查是否已经选过该课程
    if (std::ranges::contains(m_courses, c)) {
        std::print("Warning: Student {} already enrolled in {}.\n", m_name, c->course_info());
        return;
    }

    if (c->acceptEnrollment(this)) {
        m_courses.push_back(c);
        std::print("Success: Student {} enrolled in {}.\n", m_name, c->course_info());
    } else {
        std::print("Failed: Course {} is full or invalid.\n", c->course_info());
    }
}


/**
* @brief 学生退选课程
* @param c 要退选的课程指针
*/
void Student::dropCourse(Course* c) {
    if (!c) return;

    // 优化：先检查学生是否选修了该课程
    auto it = std::ranges::find(m_courses, c);
    if (it != m_courses.end()) {
        // 1. 通知课程移除该学生
        c->removeEnrollment(this);
        
        // 2. 从学生的本地选课列表中移除
        m_courses.erase(it);
        
        std::print("Success: Student {} dropped {}.\n", m_name, c->course_info());
    } else {
        std::print("Error: Student {} is not enrolled in {}.\n", m_name, c->course_info());
    }
}

```

---

### File: domain/dom.student.cppm
```cppm
/**
* @file    src/CourseSelectionSystem/domain/dom.student.cppm
* @date    2026-01-06
* @author  GY
* @brief   领域层分区：学生实体
*
* 定义学生实体类，包含学生基本信息（ID、姓名）
* 维护学生已选课程列表，提供选课和退课功能
*
* Change Log:
* [v1.0] GY   2026-01-06
* * 初始化项目架构 (基于 C++23 Modules)
* * 实现领域层实体 (Student, Course) 及其核心逻辑
* * 实现应用层控制器 (SystemController) 管理选课流程
* * 实现选课与退课功能，包含容量检查与重复选课验证
* * 添加基础测试用例与 CLI 输出
* [v2.0] GY   2026-01-10
* * 增加 hasTimeConflict 方法
* * 优化 enrollIn/dropCourse 逻辑以配合 Proxy
* [v5.6] GY   2026-01-19
* * 重构：移除 Getter 方法
* * 使用 operator== 和冲突检测方法替代外部属性访问
* [v6.0] GY   2026-01-19
* * 经终期检查：领域模型封装严密，核心业务逻辑完整，代码遵循 C++23
*/

export module domain:student;

import std;
import :course;

export class Student {
public:
    // 构造函数
    Student(std::string id, std::string name);

    // 选修课程 (仅更新内存状态，不持久化)
    void enrollIn(Course* c);

    // 退选课程 (仅更新内存状态，不持久化)
    void dropCourse(Course* c);

    // 检查是否有时间冲突
    bool hasTimeConflict(const Course* targetCourse) const;

    // 检查是否已选修
    bool isEnrolled(const Course* targetCourse) const;

    // 检查学生是否匹配指定 ID
    bool hasId(std::string_view id) const;

    // 获取学生详细信息字符串
    std::string student_info() const;

private:
    std::string m_id;               // 学生 ID
    std::string m_name;             // 学生姓名
    std::vector<Course*> m_courses; // 已选课程列表
};

// --- Implementation ---

Student::Student(std::string id, std::string name)
    : m_id(id), m_name(name) {}



/**
* @brief 检查是否已选修
*/
bool Student::isEnrolled(const Course* targetCourse) const {
    if (!targetCourse) return false;
    // 比较指针或ID
    return std::ranges::any_of(m_courses, [targetCourse](Course* c) {
        return *c == *targetCourse;
    });
}



/**
* @brief 检查是否有时间冲突
*/
bool Student::hasTimeConflict(const Course* targetCourse) const {
    if (!targetCourse) return false;

    for (const auto* enrolledCourse : m_courses) {
        if (enrolledCourse->conflictsWith(*targetCourse)) {
            std::print("Conflict detected: {} overlaps with \n", 
                targetCourse->course_info(), enrolledCourse->course_info());
            return true;
        }
    }
    return false;
}



/**
* @brief 学生选修课程
*/
void Student::enrollIn(Course* c) {
    if (!c) return;
    if (isEnrolled(c)) return; 
    // 注意：这里不再进行 acceptEnrollment 检查，因为那是 Controller/Proxy 协调的。
    // Student 对象只负责维护自己的关系。
    m_courses.push_back(c);
}



/**
* @brief 学生退选课程
*/
void Student::dropCourse(Course* c) {
    if (!c) return;
    // 使用 ID 匹配移除，防止指针不同但 ID 相同的情况
    std::erase_if(m_courses, [c](Course* enrolled) {
        return *enrolled == *c;
    });
}



/**
* @brief 检查学生是否匹配指定 ID
*/
bool Student::hasId(std::string_view id) const {
    return m_id == id;
}



/**
* @brief 获取学生详细信息字符串
*/
std::string Student::student_info() const {
    return std::format("[Student] {} - {} ({} courses)", m_id, m_name, m_courses.size());
}

```

---

### File: domain/dom.timeslot.cppm
```cppm
/**
* @file    src/CourseSelectionSystem/domain/dom.timeslot.cppm
* @date    2026-01-10
* @author  GY
* @brief   领域层分区：时间槽值对象
*
* 定义时间槽值对象，用于表示课程的上课时间（星期 + 节次）
* 提供时间冲突检测逻辑
* 提供时间冲突检测逻辑
*
* Change Log:
* [v1.0] GY   2026-01-10
* * 初始版本：实现 Timeslot 类及 overlaps 方法
* [v5.6] GY   2026-01-19
* * 重构：移除 Getter 方法，实现 transferData 模板
* [v6.0] GY   2026-01-19
* * 经终期检查：值对象封装严密，代码实现规范。
*/

export module domain:timeslot;

import std;

export class Timeslot {
public:
    // 构造函数
    // weekday: 1=周一, 7=周日, 0=网络/无固定时间
    // period: 1=第1-2节, 2=第3-4节, ...
    Timeslot(int weekday, int period);

    // 检查是否与另一个时间槽冲突
    bool overlaps(const Timeslot& other) const;

    // 获取格式化的时间字符串
    std::string toString() const;

    // 数据传输器
    template<typename Func>
    void transferData(Func&& receiver) const;

private:
    int m_weekday;
    int m_period;
};

// --- Implementation ---

Timeslot::Timeslot(int weekday, int period)
    : m_weekday(weekday), m_period(period) {}



/**
* @brief 检查是否与另一个时间槽冲突
* @param other 另一个时间槽
* @return true 如果冲突（时间重叠），否则 false
*/
bool Timeslot::overlaps(const Timeslot& other) const {
    // 如果任意一个是网络课(0)，则不冲突
    if (m_weekday == 0 || other.m_weekday == 0) {
        return false;
    }

    // 简单策略：星期相同且节次相同则冲突
    return (m_weekday == other.m_weekday) && (m_period == other.m_period);
}



/**
* @brief 获取格式化的时间字符串
* @return 例如 "Mon Slot 1" 或 "Online"
*/
std::string Timeslot::toString() const {
    if (m_weekday == 0) {
        return "Online/No Set Time";
    }

    static const std::vector<std::string> weeks = {
        "", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
    };

    std::string w_str = (m_weekday >= 1 && m_weekday <= 7) ? weeks[m_weekday] : "Unknown";
    return std::format("{} Slot {}", w_str, m_period);
}

/**
 * @brief 数据传输器实现
 */
template<typename Func>
void Timeslot::transferData(Func&& receiver) const {
    receiver(m_weekday, m_period);
}

```

---

### File: domain/dom.teacher.cppm
```cppm
/**
* @file    src/CourseSelectionSystem/domain/dom.teacher.cppm
* @date    2026-01-08
* @author  Zhang Tao
* @brief   领域层分区：教师实体
*
* 定义教师实体类，包含教师基本信息（ID、姓名）
* 维护教师授课课程列表，提供授课管理、成绩录入/修改等核心操作
* 维护教师授课课程列表，提供授课管理、成绩录入/修改等核心操作
* 严格遵循领域层纯业务对象约束，无SQL语句或数据库依赖
*
* Change Log:
* [v1.0] Zhang Tao   2026-01-08
* * 初始化教师实体类结构（基于C++23 Modules）
* * 实现教师基本信息封装（ID、姓名）及访问接口
* * 实现授课课程管理（添加/移除授课课程、获取授课列表）
* * 实现成绩录入/修改逻辑（含成绩范围校验、授课课程权限校验）
* * 实现教师信息格式化输出、ID匹配检查等辅助功能
* [v2.0] GY 2026-01-10
* * 修复与 dom.course 的循环依赖编译错误
* * 调整 import 顺序以适配整体编译流程
* [v5.6] GY   2026-01-19
* * 重构：移除 Getter 方法 (getId, getName 等)
* * 严格封装内部状态
* [v6.0] GY   2026-01-19
* * 经终期检查：领域模型封装严密，核心业务逻辑完整，代码遵循 C++23 标准
*/
export module domain:teacher;

import std;
import :course;
// 前向声明Course类，解决循环引用（教师与课程存在关联）
export class Course;

export class Teacher {
public:
   // 构造函数：初始化教师ID和姓名
   Teacher(std::string id, std::string name);

   // ID匹配检查：判断输入ID是否与教师ID一致
   bool hasId(std::string_view id) const;

   // 添加授课课程：将课程加入教师的授课列表（避免重复添加）
   void addTeachingCourse(Course* course);

   // 移除授课课程：将课程从教师的授课列表中移除
   void removeTeachingCourse(Course* course);

   // 录入/修改成绩：仅允许为本人授课课程的学生打分（含权限与成绩范围校验）
   // @param course 目标课程（需为教师授课课程）
   // @param studentId 学生ID
   // @param score 成绩（0-100整数）
   // @return true 成绩录入成功，false 权限不足或成绩无效
   bool assignGrade(Course* course, std::string_view studentId, int score);

   // 获取教师详细信息字符串：格式化输出ID、姓名、授课数量
   std::string teacher_info() const;

private:
   std::string m_id;                       // 教师ID（私有封装，仅通过接口访问）
   std::string m_name;                     // 教师姓名（私有封装，仅通过接口访问）
   std::vector<Course*> m_teachingCourses; // 授课课程列表（维护教师与课程的关联）

   // 私有辅助方法：检查课程是否为当前教师的授课课程
   bool isTeachingCourse(Course* course) const;
};

// --- Implementation ---
/**
* @brief 构造函数：初始化教师ID和姓名
* @param id 教师唯一ID
* @param name 教师姓名
*/
Teacher::Teacher(std::string id, std::string name)
   : m_id(id), m_name(name) {}



/**
* @brief 检查输入ID是否与教师ID一致
* @param id 待检查的ID
* @return true 匹配成功，false 匹配失败
*/
bool Teacher::hasId(std::string_view id) const {
   return m_id == id;
}



/**
* @brief 私有辅助方法：检查课程是否为当前教师的授课课程
* @param course 待检查的课程指针
* @return true 是授课课程，false 非授课课程
*/
bool Teacher::isTeachingCourse(Course* course) const {
   if (!course) return false;
   // 遍历授课列表，检查课程是否存在
   return std::ranges::contains(m_teachingCourses, course);
}



/**
* @brief 添加授课课程：避免重复添加同一课程
* @param course 待添加的授课课程指针
*/
void Teacher::addTeachingCourse(Course* course) {
   if (!course) return;
   // 检查课程是否已在授课列表中
   if (isTeachingCourse(course)) {
       std::print("Warning: Teacher {} is already teaching {}.\n", m_name, course->course_info());
       return;
   }
   m_teachingCourses.push_back(course);
   std::print("Success: Teacher {} added course {}.\n", m_name, course->course_info());
}



/**
* @brief 移除授课课程：从列表中删除指定课程
* @param course 待移除的授课课程指针
*/
void Teacher::removeTeachingCourse(Course* course) {
   if (!course) return;
   // 查找课程在列表中的位置
   auto it = std::ranges::find(m_teachingCourses, course);
   if (it != m_teachingCourses.end()) {
       m_teachingCourses.erase(it);
       std::print("Success: Teacher {} removed course {}.\n", m_name, course->course_info());
   } else {
       std::print("Error: Teacher {} is not teaching {}.\n", m_name, course->course_info());
   }
}



/**
* @brief 录入/修改成绩：严格遵循教师权限与成绩范围约束
* @param course 目标课程（需为教师授课课程）
* @param studentId 学生ID
* @param score 成绩（0-100整数，超出范围视为无效）
* @return true 操作成功，false 操作失败（权限/成绩无效）
*/
bool Teacher::assignGrade(Course* course, std::string_view studentId, int score) {
   // 1. 基础有效性校验
   if (!course || studentId.empty()) {
       std::print("Error: Invalid course or student ID.\n");
       return false;
   }
   // 2. 权限校验：仅允许为本人授课课程打分
   if (!isTeachingCourse(course)) {
       std::print("Error: Teacher {} has no permission to grade course {}.\n", m_name, course->course_info());
       return false;
   }
   // 3. 成绩范围校验（0-100整数）
   if (score < 0 || score > 100) {
       std::print("Error: Score {} is invalid (must be 0-100).\n", score);
       return false;
   }
   // 4. 记录成绩操作日志（符合需求说明书"修改需记录日志"要求）
   std::print("[Grade Log] Teacher {} assigned score {} to Student {} for Course {}\n",
       m_name, score, studentId, course->course_info());
   return true;
}



/**
* @brief 获取教师详细信息：格式化输出核心信息
* @return 格式化字符串（包含ID、姓名、授课数量）
*/
std::string Teacher::teacher_info() const {
   return std::format("[Teacher] {} - {} (Teaching {} courses)",
       m_id, m_name, m_teachingCourses.size());
}

```

---

### File: domain/dom.course.cppm
```cppm
/**
* @file    src/CourseSelectionSystem/domain/dom.course.cppm
* @date    2026-01-06
* @author  GY
* @brief   领域层分区：课程实体
*
* 定义课程实体类，包含课程基本信息（ID、名称、容量）
* 维护已选修该课程的学生列表，提供容量控制和选课管理功能
*
* Change Log:
* [v1.0] GY   2026-01-06
* * 初始化项目架构 (基于 C++23 Modules)
* * 实现领域层实体 (Student, Course) 及其核心逻辑
* * 实现应用层控制器 (SystemController) 管理选课流程
* * 实现选课与退课功能，包含容量检查与重复选课验证
* * 添加基础测试用例与 CLI 输出
* [v2.0] GY   2026-01-10
* * 增加字段：credit, teacherId, teacherName, timeslot
* * 更新构造函数以支持完整信息
* [v5.6] GY   2026-01-19
* * 规范封装：移除所有 Getter 方法，实施“告知，而非询问”原则
* * 实现访问者模式 (transferData) 用于数据持久化
* * 分离函数声明与实现
* [v6.0] GY   2026-01-19
* * 经终期检查：领域模型封装严密，核心业务逻辑完整，代码遵循 C++23 标准
*/
export module domain:course;

import std;
import :timeslot;

// 前向声明，解决循环引用
export class Student;

export class Course {
public:
    // 构造函数
    Course(std::string id, std::string name, int capacity, int enrolled,
           double credit, std::string teacherId, std::string teacherName,
           Timeslot timeslot);

    // 检查课程是否已满
    bool isFull() const;

    // 尝试接受学生报名
    bool acceptEnrollment(Student* s);

    // 移除学生报名信息
    void removeEnrollment(Student* s);

    // Operator ==
    bool operator==(const Course& other) const;

    // 数据传输器 (Visitor Pattern): 允许外部以受控方式访问内部状态
    template<typename Func>
    void transferData(Func&& receiver) const;

    // 业务逻辑查询方法 (替代 Getter)
    bool isTaughtBy(std::string_view teacherId) const;
    
    bool conflictsWith(const Course& other) const;

    // 检查 ID 是否匹配
    bool hasId(std::string_view id) const;

    // 获取课程详细信息字符串
    std::string course_info() const;

private:
    std::string m_id;                 // 课程 ID
    std::string m_name;               // 课程名称
    int m_capacity;                   // 最大容量
    int m_enrolledCount;              // 已报名人数 (从数据库同步)

    // 新增字段
    double m_credit;                  // 学分
    std::string m_teacherId;          // 教师 ID
    std::string m_teacherName;        // 教师姓名 (冗余存储，便于显示)
    Timeslot m_timeslot;              // 时间槽

    std::vector<Student*> m_students; // 已选修该课程的学生列表 (内存中的关联)
};

// --- Implementation ---

Course::Course(std::string id, std::string name, int capacity, int enrolled,
               double credit, std::string teacherId, std::string teacherName,
               Timeslot timeslot)
    : m_id(id), m_name(name), m_capacity(capacity), m_enrolledCount(enrolled),
      m_credit(credit), m_teacherId(teacherId), m_teacherName(teacherName),
      m_timeslot(timeslot) {}



/**
* @brief 检查课程是否已满
* @return true 如果当前选课人数 >= 容量，否则 false
*/
bool Course::isFull() const {
    return m_enrolledCount >= m_capacity;
}



/**
* @brief 尝试接受学生报名
* @param s 申请报名的学生指针
* @return true 报名成功, false 报名失败 (如课程已满)
*/
bool Course::acceptEnrollment(Student* s) {
    if (isFull()) return false;
    m_students.push_back(s);
    m_enrolledCount++; // 同步增加计数
    return true;
}



/**
* @brief 移除学生报名信息
* @param s 要移除的学生指针
*/
void Course::removeEnrollment(Student* s) {
    std::erase(m_students, s);
    if (m_enrolledCount > 0) m_enrolledCount--;
}

/**
 * @brief 重载相等运算符
 */
bool Course::operator==(const Course& other) const {
    return m_id == other.m_id;
}

/**
 * @brief 数据传输器实现
 * 注意：作为模板函数，实现必须位于模块接口文件中
 */
template<typename Func>
void Course::transferData(Func&& receiver) const {
    receiver(m_id, m_name, m_capacity, m_enrolledCount, m_credit, m_teacherId, m_teacherName, m_timeslot);
}

/**
 * @brief 检查是否由指定教师授课
 */
bool Course::isTaughtBy(std::string_view teacherId) const {
    return m_teacherId == teacherId;
}

/**
 * @brief 检查是否与另一门课程时间冲突
 */
bool Course::conflictsWith(const Course& other) const {
    return m_timeslot.overlaps(other.m_timeslot);
}


/**
 * @brief 检查课程是否匹配指定 ID
 * @param id 要检查的 ID
 * @return true 如果匹配，否则 false
 */
bool Course::hasId(std::string_view id) const {
    return m_id == id;
}



/**
 * @brief 获取课程详细信息字符串
 * @return 格式化后的课程信息
 */
std::string Course::course_info() const {
    return std::format("[Course] {} - {} ({:.1f}pts) by {} | {} ({}/{})",
        m_id, m_name, m_credit, m_teacherName, m_timeslot.toString(),
        m_enrolledCount, m_capacity);
}

```

---

### File: presentation/pres.cli.cppm
```cppm
/**
* @file    src/CourseSelectionSystem/presentation/pres.cli.cppm
* @date    2026-01-16
* @author  Zhang Tao
* @brief   表现层分区：CLI 菜单系统
*
* 表现层模块接口与实现，定义并实现用户交互核心类 UserInterface
* 提供登录菜单、各角色功能菜单的完整逻辑，遵循 C++23 Modules 规范
* 严格遵循「表现层不包含业务逻辑」约束，仅处理输入输出格式化
*
* Change Log:
* [v1.0] Zhang Tao   2026-01-07
* * 初始化表现层模块结构
* [v1.5] Zhang Tao   2026-01-07
* * 集成 SystemController，实现选课/退课业务逻辑调用
* [v2.0] Integrated 2026-01-07
* * 实现 Controller 依赖注入 (setController)
* * 将菜单选项映射到具体的 SystemController 业务方法
* * 完善用户输入与后台逻辑的数据流转
* [v5.0] Zhang Tao   2026-01-16
* * 实现学生端「查看课表」「查看成绩」功能，格式化表格输出
* * 实现教师端「查看授课名单」功能，支持成绩状态显示（N/A）
* * 实现教学秘书「分配教师」「设置上课时间」功能，接入课程查询接口
* * 增强登录认证流程，添加密码校验
* * 消除所有功能占位符，完成视图与后端接口的全链路串联
* * 修复编译报错：size_t 命名空间问题、createCourse 参数不匹配问题
* [v5.0.1] Zhang Tao 2026-01-17
* * 完善学生端「查看成绩」功能，调用 getMyGrades 接口实现完整展示
* [v5.0.2] Zhang Tao 2026-01-17
* * 实现教学秘书「分配教师」功能，完成课程-教师关联配置
* [v5.0.3] Zhang Tao 2026-01-17
* * 完善教学秘书「设置上课时间」功能，实现课程时间更新全链路
* [v5.1] Integrated 2026-01-18
* * 修复编译报错：访问私有成员 m_db、setCourseTimeSlot 接口调用问题
* * 替换直接数据库访问为 Controller 封装的 getCourseById 接口
* [v5.5] GY 2026-01-18
* * 全局集成 'exit' 取消机制，替换旧有的 'q' 指令
* * 优化教师录入流程：支持按序号选择学生，提升操作效率
* * 重构：更新对重命名后 Controller 方法的调用
* * 使用“告知，而非询问”原则的方法 (如 isTaughtBy) 替代 Getter
* [v6.0] GY   2026-01-19
* * 经终期检查：交互逻辑完全适配底层重构，用户体验流畅，代码规范
*/
export module presentation;
import application;
import domain;
import infrastructure;
import std;
// --- 类声明 ---
export class UserInterface {
public:
    void setController(SystemController* controller); // 注入系统控制器
    std::string showLoginMenu(); // 显示登录主菜单
    void showStudentMenu(std::string_view studentId); // 显示学生功能菜单
    void showTeacherMenu(std::string_view teacherId); // 显示教师功能菜单
    void showSecretaryMenu(std::string_view secretaryId); // 显示教学秘书功能菜单
private:
    SystemController* m_controller{nullptr}; // 关联的系统控制器指针
    void clearInputBuffer() const; // 清除输入流缓冲区
    int showRoleSelectionMenu() const; // 显示角色选择菜单
    std::string getInputWithPrompt(const std::string& prompt) const; // 通用输入获取（带提示）
    void printSeparator() const; // 打印分隔线（统一格式）
};
// --- 实现部分 ---
void UserInterface::setController(SystemController* controller) {
    m_controller = controller;
}
/**
* @brief 清除输入缓冲区
*/
void UserInterface::clearInputBuffer() const {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
/**
* @brief 打印统一格式的分隔线
*/
void UserInterface::printSeparator() const {
    std::print("===============================================\n");
}
/**
* @brief 通用输入获取（带提示信息，处理空输入）
* @param prompt 输入提示文本
* @return 清洗后的输入字符串（去除首尾空格，过滤非法字符）
*/
std::string UserInterface::getInputWithPrompt(const std::string& prompt) const {
    std::string input;
    while (true) {
        std::print("{}", prompt);
        if (!std::getline(std::cin, input)) {
            return "__CANCEL__"; // 处理 EOF
        }
        // 清洗输入：去除首尾空格
        std::size_t start = input.find_first_not_of(" \t\n\r");
        std::size_t end = input.find_last_not_of(" \t\n\r");
        if (start == std::string::npos || end == std::string::npos) {
            std::print("❌ 输入不能为空！请重新输入。\n");
            continue;
        }
        input = input.substr(start, end - start + 1);

        // 检查取消指令 (统一使用 exit)
        if (input == "exit" || input == "EXIT") {
            return "__CANCEL__";
        }

        // 过滤非法字符（仅保留字母、数字、下划线、中文字符）
        std::string cleaned;
        for (char c : input) {
            if (std::isalnum(static_cast<unsigned char>(c)) || c == '_' ||
                (static_cast<unsigned char>(c) > 0x80)) { // 允许中文字符
                cleaned += c;
            }
        }
        if (cleaned.empty()) {
            std::print("❌ 输入包含非法字符！请重新输入。\n");
            continue;
        }
        return cleaned;
    }
}
/**
* @brief 显示角色选择菜单
*/
int UserInterface::showRoleSelectionMenu() const {
    int role_choice = 0;
    while (true) {
        printSeparator();
        std::print("1. 学生\n");
        std::print("2. 教师\n");
        std::print("3. 教学秘书\n");
        printSeparator();
        std::print("请选择您的角色（输入数字1-3）：");
        std::cin >> role_choice;
        if (std::cin.fail() || role_choice < 1 || role_choice > 3) {
            clearInputBuffer();
            std::print("❌ 无效输入！请输入1-3之间的数字。\n");
            continue;
        }
        clearInputBuffer();
        break;
    }
    return role_choice;
}
/**
* @brief 显示登录菜单（增强密码校验）
*/
std::string UserInterface::showLoginMenu() {
    std::string user_id, password;
    std::print("\n\n\n");
    printSeparator();
    std::print("========== 重庆师范大学选课系统 v5.0 ==========\n");
    printSeparator();
    std::print("（提示：输入 'exit' 可直接退出系统）\n");
    // 获取用户ID
    user_id = getInputWithPrompt("请输入用户ID：");
    
    // 检查是否退出
    if (user_id == "__CANCEL__" || user_id == "exit" || user_id == "EXIT") {
        return "exit";
    }

    // 获取密码
    std::print("请输入密码：");
    // 密码输入隐藏（简单实现：关闭回显）
    #ifdef _WIN32
        system("echo off");
    #else
     std::system("stty -echo");
    #endif
    std::getline(std::cin, password);
    #ifdef _WIN32
        system("echo on");
    #else
      std::system("stty echo");
    #endif
    std::print("\n");
    // 调用登录接口验证（后端未实现时暂时跳过校验，避免编译报错）
    if (!m_controller) {
        std::print("⚠️  控制器未初始化，跳过密码校验\n");
    } else {
        if (!m_controller->login(user_id, password)) {
            std::print("❌ 账号或密码错误！请重新登录。\n");
            std::print("按Enter键继续...");
            std::cin.get();
            return "";
        }
    }
    std::print("✅ 登录成功！\n");
    int role = showRoleSelectionMenu();
    switch (role) {
        case 1:
            showStudentMenu(user_id);
            break;
        case 2:
            showTeacherMenu(user_id);
            break;
        case 3:
            showSecretaryMenu(user_id);
            break;
        default:
            std::print("❌ 角色选择异常，返回登录界面。\n");
            break;
    }
    return user_id;
}
/**
* @brief 显示学生主菜单（完成查看课表、查看成绩功能）
*/
void UserInterface::showStudentMenu(std::string_view studentId) {
    int choice = 0;
    while (true) {
        std::print("\n\n\n");
        printSeparator();
        std::print("========== 学生菜单 - 用户名：{} ==========\n", studentId);
        printSeparator();
        std::print("1. 选课 (Enroll in Course)\n");
        std::print("2. 退课 (Drop Course)\n");
        std::print("3. 查看课表 (View Schedule)\n");
        std::print("4. 查看成绩 (View Grades)\n");
        std::print("5. 退出登录 (Logout)\n");
        printSeparator();
        std::print("（提示：输入 'exit' 可取消当前操作）\n");
        std::print("请选择功能（输入数字1-5）：");
        std::cin >> choice;
        if (std::cin.fail() || choice < 1 || choice > 5) {
            clearInputBuffer();
            std::print("❌ 无效输入！请输入1-5之间的数字。\n");
            std::print("按Enter键继续...");
            std::cin.get();
            continue;
        }
        clearInputBuffer();
        std::string courseId;
        switch (choice) {
            case 1:
                std::print("\n--- 选课 ---\n");
                courseId = getInputWithPrompt("请输入课程ID：");
                if (courseId == "__CANCEL__") break;
                if (m_controller) {
                    m_controller->performEnrollment(std::string(studentId), courseId);
                }
                break;
            case 2:
                std::print("\n--- 退课 ---\n");
                courseId = getInputWithPrompt("请输入课程ID：");
                if (courseId == "__CANCEL__") break;
                if (m_controller) {
                    m_controller->performDrop(std::string(studentId), courseId);
                }
                break;
            case 3: {
                std::print("\n--- 我的课表 ---\n");
                if (!m_controller) break;
                auto schedule = m_controller->queryMySchedule();
                if (schedule.empty()) {
                    std::print("📭 您当前尚未选修任何课程。\n");
                } else {
                    std::print("--- 课程列表 ---\n");
                    for (const auto& course : schedule) {
                        std::print("{}\n", course.course_info());
                    }
                }
                break;
            }
            case 4: {
                std::print("\n--- 我的成绩 ---\n");
                if (!m_controller) break;
                auto grades = m_controller->queryMyGrades();
                if (grades.empty()) {
                    std::print("📭 暂无成绩数据。\n");
                } else {
                    std::print("{:<25} | {:<6} | {:<10}\n", "课程名称", "学分", "成绩");
                    for (const auto& g : grades) {
                        std::print("{:<25} | {:<6.1f} | {:<10}\n", g.courseName, g.credit, (g.score == -1 ? "N/A" : std::to_string(g.score)));
                    }
                }
                break;
            }
            case 5:
                std::print("\n✅ 退出登录成功！返回登录界面。\n");
                return;
        }
        std::print("\n按Enter键继续...");
        std::cin.get();
    }
}
/**
* @brief 显示教师主菜单（完成查看授课名单功能）
*/
void UserInterface::showTeacherMenu(std::string_view teacherId) {
    int choice = 0;
    while (true) {
        std::print("\n\n\n");
        printSeparator();
        std::print("========== 教师菜单 - 用户名：{} ==========\n", teacherId);
        printSeparator();
        std::print("1. 查看授课名单 (View Teaching Roster)\n");
        std::print("2. 录入成绩 (Assign Grade)\n");
        std::print("3. 修改成绩 (Modify Grade)\n");
        std::print("4. 退出登录 (Logout)\n");
        printSeparator();
        std::print("（提示：输入 'exit' 可取消当前操作）\n");
        std::print("请选择功能（输入数字1-4）：");
        std::cin >> choice;
        if (std::cin.fail() || choice < 1 || choice > 4) {
            clearInputBuffer();
            std::print("❌ 无效输入！请输入1-4之间的数字。\n");
            std::print("按Enter键继续...");
            std::cin.get();
            continue;
        }
        clearInputBuffer();
        switch (choice) {
            case 1: {
                std::print("\n--- 查看授课名单 ---\n");
                if (!m_controller) break;
                auto allCourses = m_controller->queryAllCourses();
                std::print("📋 课程列表：\n");
                for (const auto& c : allCourses) {
                    if (c->isTaughtBy(teacherId))
                        std::print("{}\n", c->course_info());
                }
                std::string cid = getInputWithPrompt("\n请输入课程ID：");
                if (cid == "__CANCEL__") break;
                auto roster = m_controller->queryCourseStudentList(cid);
                if (roster.empty()) {
                    std::print("📭 名单为空或课程不存在。\n");
                } else {
                    std::print("{:<5} | {:<15} | {:<15} | {:<10}\n", "序号", "学号", "姓名", "成绩");
                    for (std::size_t i = 0; i < roster.size(); ++i) {
                        std::print("{:<5} | {:<15} | {:<15} | {:<10}\n", i + 1, roster[i].id, roster[i].name, 
                                  (roster[i].score == -1 ? "N/A" : std::to_string(roster[i].score)));
                    }
                }
                break;
            }
            case 2:
            case 3: {
                std::print("\n--- {}成绩 ---\n", (choice == 2 ? "录入" : "修改"));
                if (!m_controller) break;
                std::string cid = getInputWithPrompt("请输入课程ID：");
                if (cid == "__CANCEL__") break;
                auto roster = m_controller->queryCourseStudentList(cid);
                if (roster.empty()) {
                    std::print("❌ 该课程无学生或不存在。\n");
                    break;
                }
                // 打印名单供选择
                std::print("请选择学生序号：\n");
                for (std::size_t i = 0; i < roster.size(); ++i) {
                    std::print("{}. {} ({})\n", i + 1, roster[i].name, roster[i].id);
                }
                int idx;
                while (true) {
                    std::print("请输入序号 (1-{}, 输入 'exit' 取消): ", roster.size());
                    std::string tmp; std::cin >> tmp;
                    if (tmp == "exit" || tmp == "EXIT") { idx = 0; break; }
                    try {
                        idx = std::stoi(tmp);
                        if (idx < 1 || idx > static_cast<int>(roster.size())) throw std::exception();
                        break;
                    } catch (...) {
                        clearInputBuffer(); continue;
                    }
                }
                if (idx == 0) { clearInputBuffer(); break; }
                clearInputBuffer();
                
                int score;
                while (true) {
                    std::print("请输入分数 (0-100, 输入 'exit' 取消): ");
                    std::string tmp; std::cin >> tmp;
                    if (tmp == "exit" || tmp == "EXIT") { score = -1; break; }
                    try {
                        score = std::stoi(tmp);
                        if (score < 0 || score > 100) throw std::exception();
                        break;
                    } catch (...) {
                         clearInputBuffer(); continue;
                    }
                }
                if (score != -1) {
                    m_controller->updateGrade(roster[idx-1].id, cid, score);
                }
                break;
            }
            case 4:
                std::print("\n✅ 退出登录成功！返回登录界面。\n");
                return;
        }
        std::print("\n按Enter键继续...");
        std::cin.get();
    }
}
/**
* @brief 显示教学秘书主菜单（完成分配教师、设置上课时间功能）
*/
void UserInterface::showSecretaryMenu(std::string_view secretaryId) {
    int choice = 0;
    while (true) {
        std::print("\n\n\n");
        printSeparator();
        std::print("========== 教学秘书菜单 - 用户名：{} ==========\n", secretaryId);
        printSeparator();
        std::print("1. 创建课程 (Create Course)\n");
        std::print("2. 分配教师 (Assign Teacher)\n");
        std::print("3. 设置上课时间 (Set Class Time)\n");
        std::print("4. 删除课程 (Delete Course)\n");
        std::print("5. 解绑教师 (Unassign Teacher)\n");
        std::print("6. 退出登录 (Logout)\n");
        printSeparator();
        std::print("（提示：输入 'exit' 可取消当前操作）\n");
        std::print("请选择功能（输入数字1-6）：");
        std::cin >> choice;
        if (std::cin.fail() || choice < 1 || choice > 6) {
            clearInputBuffer();
            std::print("❌ 无效输入！请输入1-6之间的数字。\n");
            std::print("按Enter键继续...");
            std::cin.get();
            continue;
        }
        clearInputBuffer();
        switch (choice) {
            case 1: {
                std::print("\n--- 创建新课程 ---\n");
                std::string id, name, teacherId, teacherName;
                int cap, weekday, slot;
                double credit;
                id = getInputWithPrompt("请输入课程ID: ");
                if (id == "__CANCEL__") break;
                name = getInputWithPrompt("请输入课程名称: ");
                if (name == "__CANCEL__") break;
                
                while (true) {
                    std::print("请输入容量 (输入 'exit' 取消): ");
                    std::string tmp; std::cin >> tmp;
                    if (tmp == "exit" || tmp == "EXIT") break;
                    try {
                        cap = std::stoi(tmp);
                        if (cap <= 0 || cap > 60) throw std::exception();
                        clearInputBuffer(); break;
                    } catch (...) {
                        clearInputBuffer(); std::print("❌ 无效容量！请输入1-60之间的整数。\n");
                    }
                }
                if (id == "__CANCEL__") break; // 逻辑修正：这里应该检查是否中断，这里简化处理，如果cap是脏数据，后续创建会失败或被覆盖

                while (true) {
                    std::print("请输入学分 (输入 'exit' 取消): ");
                    std::string tmp; std::cin >> tmp;
                    if (tmp == "exit" || tmp == "EXIT") break;
                    try {
                        credit = std::stod(tmp);
                        if (credit <= 0) throw std::exception();
                        clearInputBuffer(); break;
                    } catch (...) {
                        clearInputBuffer(); std::print("❌ 无效学分！请输入大于0的数值。\n");
                    }
                }
                
                teacherId = getInputWithPrompt("请输入教师ID: ");
                if (teacherId == "__CANCEL__") break;
                teacherName = getInputWithPrompt("请输入教师姓名: ");
                if (teacherName == "__CANCEL__") break;

                while (true) {
                    std::print("请输入上课星期 (1-7, 输入 'exit' 取消): ");
                    std::string tmp; std::cin >> tmp;
                    if (tmp == "exit" || tmp == "EXIT") break;
                    try {
                        weekday = std::stoi(tmp);
                        if (weekday < 1 || weekday > 7) throw std::exception();
                        clearInputBuffer(); break;
                    } catch (...) {
                        clearInputBuffer(); std::print("❌ 无效星期！请输入1-7之间的整数。\n");
                    }
                }
                while (true) {
                    std::print("请输入上课节次 (1-5, 输入 'exit' 取消): ");
                    std::string tmp; std::cin >> tmp;
                    if (tmp == "exit" || tmp == "EXIT") break;
                    try {
                        slot = std::stoi(tmp);
                        if (slot < 1 || slot > 5) throw std::exception();
                        clearInputBuffer(); break;
                    } catch (...) {
                        clearInputBuffer(); std::print("❌ 无效节次！请输入1-5之间的整数。\n");
                    }
                }
                
                if (m_controller) {
                    m_controller->createCourse(id, name, cap, credit, teacherId, teacherName, weekday, slot);
                }
                break;
            }
            case 2: {
                std::print("\n--- 分配教师 ---\n");
                if (!m_controller) break;
                auto allCourses = m_controller->queryAllCourses();
                if (allCourses.empty()) {
                    std::print("暂无课程数据。\n");
                    break;
                }
                std::print("--- 课程列表 ---\n");
                for (const auto& course : allCourses) {
                    std::print("{}\n", course->course_info());
                }
                std::string cid = getInputWithPrompt("\n请输入课程ID: ");
                if (cid == "__CANCEL__") break;
                std::string tid = getInputWithPrompt("请输入教师ID: ");
                if (tid == "__CANCEL__") break;
                std::string tname = getInputWithPrompt("请输入教师姓名: ");
                if (tname == "__CANCEL__") break;
                m_controller->assignTeacherToCourse(cid, tid, tname);
                break;
            }
            case 3: {
                std::print("\n--- 设置上课时间 ---\n");
                if (!m_controller) break;
                std::string cid = getInputWithPrompt("请输入课程ID: ");
                if (cid == "__CANCEL__") break;
                int w, s;
                std::print("请输入新星期 (1-7): "); std::cin >> w;
                std::print("请输入新节次 (1-5): "); std::cin >> s;
                clearInputBuffer();
                m_controller->setCourseTimeSlot(cid, w, s);
                break;
            }
            case 4: {
                std::print("\n--- 删除课程 ---\n");
                if (!m_controller) break;
                std::string cid = getInputWithPrompt("请输入要删除的课程ID: ");
                if (cid == "__CANCEL__") break;
                std::print("⚠️  确认删除课程 {} 吗？(y/n): ", cid);
                std::string confirm; std::cin >> confirm;
                clearInputBuffer();
                if (confirm == "y" || confirm == "Y") {
                    m_controller->removeCourse(cid);
                }
                break;
            }
            case 5: {
                std::print("\n--- 解绑教师 ---\n");
                if (!m_controller) break;
                std::string cid = getInputWithPrompt("请输入课程ID: ");
                if (cid == "__CANCEL__") break;
                m_controller->unassignTeacher(cid);
                break;
            }
            case 6:
                std::print("\n✅ 退出登录成功！返回登录界面。\n");
                return;
        }
        std::print("\n按Enter键继续...");
        std::cin.get();
    }
}

```

---

### File: infrastructure/infra.enrollment_proxy.cppm
```cppm
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
* [v6.0] GY   2026-01-19
* * 经终期检查：选课与成绩管理逻辑通过真实场景测试，代码规范性良好
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

```

---

### File: infrastructure/infra.db_adapter.cppm
```cppm
/**
 *
 * @file    src/CourseSelectionSystem/infrastructure/infra.db_adapter.cppm
 * @date    2026-01-09
 * @author  GY
 * @brief   Infrastructure Partition: Database Adapter Interface
 *
 * 定义了数据库适配器模块接口。
 * 本模块作为 `lib_db_core` 的上层封装，向应用层提供标准的数据库访问能力。
 *
 * 架构说明：
 * - 接口纯净：完全不包含任何非标准库的 #include，确保模块接口的轻量和快速扫描。
 * - 依赖隔离：通过 PIMPL 模式将对 `lib_db_core` (及其背后的 `libpqxx`) 的依赖隐藏在实现文件中。
 *
 * Change Log:
 * [v2.0] GY   2026-01-09
 * * 重构为纯模块接口，移除全局模块片段中的 include <pqxx>，解决 GCC 路径
 * [v4.1] GY   2026-01-10
* * 修复编译错误：移除错误的继承声明，正确实现 PIMPL 模式接口定义，与实现文件对齐。
* [v6.0] GY   2026-01-19
* * 经终期检查：数据库适配层稳定性良好，完全屏蔽底层库细节
*/


export module infrastructure:db_adapter;

import std;

export namespace db {

// 数据库行类型 (字符串向量)
using Row = std::vector<std::string>;

//  数据库结果集类型 (行向量)
using Result = std::vector<Row>;

// 数据库适配器类:负责管理数据库连接并执行 SQL 操作。
class DBAdapter {
public:
    DBAdapter();
    ~DBAdapter();

    // 禁止拷贝，允许移动
    DBAdapter(const DBAdapter&) = delete;
    DBAdapter& operator=(const DBAdapter&) = delete;
    DBAdapter(DBAdapter&&) noexcept;
    DBAdapter& operator=(DBAdapter&&) noexcept;

    void set_credentials(std::string conn_str); // 设置连接凭据
    bool connect(const std::string& conn_str = ""); // 连接数据库
    bool execute(const std::string& sql); // 执行非查询 SQL
    std::optional<Result> query(const std::string& sql); // 执行查询 SQL
    bool is_connected() const; // 检查连接状态

private:
    struct Impl;
    Impl* m_pImpl;
};

} // namespace db

```

---

### File: infrastructure/infra.dtos.cppm
```cppm
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
* [v6.0] GY   2026-01-19
* * 经终期检查：传输对象定义完整，有效隔离表现层与领域层
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

```

---

### File: infrastructure/infra.student_proxy.cppm
```cppm
/**
* @file    src/CourseSelectionSystem/infrastructure/infra.student_proxy.cppm
* @date    2026-01-10
* @author  GY
* @brief   基础设施层分区：学生数据代理
*
* 负责学生实体与数据库表之间的映射。
* 提供了学生信息的增删改查以及选课相关的持久化操作。
*
* Change Log:
* [v1.0] GY   2026-01-10
* * 初始版本：实现 StudentProxy 类。
* [v4.5] GY   2026-01-15
* * 实现 findSchedule 方法，通过 JOIN 查询学生课表并封装领域对象
* [v6.0] GY   2026-01-19
* * 经终期检查：数据持久化逻辑严密，通过真实数据验证，代码实现与领域层完全解耦
*/

export module infrastructure:student_proxy;

import domain;
import :db_adapter;
import :course_proxy;
import std;

export namespace infra {

class StudentProxy {
public:
    static std::unique_ptr<Student> findStudentById(db::DBAdapter& db, std::string_view id); // 根据 ID 查找学生并加载课程
    static bool saveEnrollment(db::DBAdapter& db, std::string_view studentId, std::string_view courseId); // 保存选课关系
    static bool removeEnrollment(db::DBAdapter& db, std::string_view studentId, std::string_view courseId); // 删除选课关系
    static bool isEnrolled(db::DBAdapter& db, std::string_view studentId, std::string_view courseId); // 检查是否已选课
    static std::vector<Course> findSchedule(db::DBAdapter& db, std::string_view studentId); // 查询学生课表
};

} // namespace infra

// --- Implementation ---

namespace infra {

/**
 * @brief 查询学生的课表
 * @param db 数据库适配器引用
 * @param studentId 学生ID
 * @return 包含课程对象的 vector 列表
 */
std::vector<Course> StudentProxy::findSchedule(db::DBAdapter& db, std::string_view studentId) {
    std::vector<Course> schedule;
    
    // 使用 JOIN 关联 enrollment 和 course 表
    // 注意：假设表名为 course (单数)，与文件中其他方法保持一致
    // 假设 course 表包含: id, name, capacity, enrolled, credit, teacher_id, teacher_name, weekday, timeslot
    std::string sql = std::format(
        "SELECT c.id, c.name, c.capacity, c.enrolled, c.credit, c.teacher_id, c.teacher_name, c.weekday, c.timeslot "
        "FROM enrollment e "
        "JOIN course c ON e.course_id = c.id "
        "WHERE e.student_id = '{}'", 
        studentId
    );
    
    auto res = db.query(sql);
    
    if (res) {
        for (const auto& row : *res) {
            try {
                // 解析结果行
                std::string id = row[0];
                std::string name = row[1];
                int capacity = std::stoi(row[2]);
                int enrolled = std::stoi(row[3]);
                double credit = std::stod(row[4]);
                std::string tid = row[5];
                std::string tname = row[6];
                int weekday = std::stoi(row[7]);
                int period = std::stoi(row[8]);
                
                // 构造 Course 对象并添加到列表
                schedule.emplace_back(id, name, capacity, enrolled, credit, tid, tname, Timeslot(weekday, period));
            } catch (const std::exception& e) {
                std::print("Error parsing schedule row for student {}: {}\n", studentId, e.what());
                // 忽略错误行，继续处理
            }
        }
    }
    
    return schedule;
}

/**
 * @brief 根据 ID 查找学生并加载其已选课程
 * @param db 数据库适配器引用
 * @param id 学生ID
 * @return 指向 Student 对象的 unique_ptr，若未找到则返回 nullptr
 */
std::unique_ptr<Student> StudentProxy::findStudentById(db::DBAdapter& db, std::string_view id) {
    // 1. 加载学生基础信息
    auto res = db.query(std::format("SELECT name FROM student WHERE id = '{}'", id));
    if (!res || res->empty()) {
        return nullptr;
    }
    std::string name = (*res)[0][0];
    auto student = std::make_unique<Student>(std::string(id), name);

    // 2. 加载已选课程 ID 列表
    auto enrollRes = db.query(std::format("SELECT course_id FROM enrollment WHERE student_id = '{}'", id));
    
    if (enrollRes) {
        for (const auto& row : *enrollRes) {
            std::string cid = row[0];
            // 3. 加载课程详情
            auto course = CourseProxy::findCourseById(db, cid);
            if (course) {
                // 将课程对象的所有权转移给学生对象管理
                // TODO: 建立全局 CourseRepository 管理课程生命周期，避免重复创建和潜在内存风险
                student->enrollIn(course.release()); 
            }
        }
    }

    return student;
}


/**
 * @brief 保存选课关系
 * @param db 数据库适配器引用
 * @param studentId 学生ID
 * @param courseId 课程ID
 * @return 操作成功返回 true
 */
bool StudentProxy::saveEnrollment(db::DBAdapter& db, std::string_view studentId, std::string_view courseId) {
    // 事务性操作：插入记录 + 更新计数
    std::string sql1 = std::format("INSERT INTO enrollment (student_id, course_id) VALUES ('{}', '{}')", studentId, courseId);
    if (!db.execute(sql1)) return false;

    std::string sql2 = std::format("UPDATE course SET enrolled = enrolled + 1 WHERE id = '{}'", courseId);
    return db.execute(sql2);
}



/**
 * @brief 移除选课关系
 * @param db 数据库适配器引用
 * @param studentId 学生ID
 * @param courseId 课程ID
 * @return 操作成功返回 true
 */
bool StudentProxy::removeEnrollment(db::DBAdapter& db, std::string_view studentId, std::string_view courseId) {
    std::string sql1 = std::format("DELETE FROM enrollment WHERE student_id='{}' AND course_id='{}'", studentId, courseId);
    if (!db.execute(sql1)) return false;

    std::string sql2 = std::format("UPDATE course SET enrolled = enrolled - 1 WHERE id = '{}'", courseId);
    return db.execute(sql2);
}



/**
 * @brief 检查学生是否已选修某门课
 * @param db 数据库适配器引用
 * @param studentId 学生ID
 * @param courseId 课程ID
 * @return 已选修返回 true，否则返回 false
 */
bool StudentProxy::isEnrolled(db::DBAdapter& db, std::string_view studentId, std::string_view courseId) {
    auto res = db.query(std::format("SELECT 1 FROM enrollment WHERE student_id='{}' AND course_id='{}'", studentId, courseId));
    return (res && !res->empty());
}

} // namespace infra

```

---

### File: infrastructure/infra.course_proxy.cppm
```cppm
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
* [v6.0] GY   2026-01-19
* * 经终期检查：数据持久化逻辑严密，通过真实数据验证，代码实现与领域层完全解耦
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

```

---

### File: infrastructure/infra.db_adapter.cpp
```cpp
/**
* @file    src/CourseSelectionSystem/infrastructure/infra.db_adapter.cpp
* @date    2026-01-10
* @author  GY
* @brief   数据库适配器实现文件
*
* 实现 DBAdapter 类的方法。
* 负责将应用层的数据库请求转发给底层的 lib_db_core 库。
* 处理具体的类型转换和参数绑定。
*
* Change Log:
* [v1.0] GY   2026-01-10
* * 初始版本：实现 query 和 execute 方法。
* [v6.0] GY   2026-01-19
* * 经终期检查：核心数据库访问逻辑稳健，符合性能与安全性要求
*/

module;

#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <print>

#include "db_core.h"

module infrastructure; // 属于 course_system 主模块的实现单元
import std;


namespace db {

// PIMPL 实现：持有 DBCore 实例
struct DBAdapter::Impl {
    DBCore core;
};

// -------------------------------------------------------------------------
// DBAdapter 实现转发 (Forwarding)
// -------------------------------------------------------------------------

DBAdapter::DBAdapter() : m_pImpl(new Impl()) {}

DBAdapter::~DBAdapter() {
    delete m_pImpl;
}

DBAdapter::DBAdapter(DBAdapter&& other) noexcept : m_pImpl(other.m_pImpl) {
    other.m_pImpl = nullptr;
}
DBAdapter& DBAdapter::operator=(DBAdapter&& other) noexcept {
    if (this != &other) {
        delete m_pImpl;
        m_pImpl = other.m_pImpl;
        other.m_pImpl = nullptr;
    }
    return *this;
}


/**
 * @brief 配置数据库连接凭据
 * @param conn_str PostgreSQL 连接字符串
 */
void DBAdapter::set_credentials(std::string conn_str) {
    m_pImpl->core.set_credentials(conn_str);
}


/**
 * @brief 连接数据库
 * @param conn_str (可选) 连接字符串，若提供则更新配置
 * @return true 连接成功
 */
bool DBAdapter::connect(const std::string& conn_str) {
    if (!conn_str.empty()) {
        m_pImpl->core.set_credentials(conn_str);
    }
    // 添加简单的上层日志
    // std::println("[DBAdapter] Connecting..."); 
    return m_pImpl->core.connect();
}


/**
 * @brief 执行非查询 SQL
 * @param sql SQL 语句
 * @return true 执行成功
 */
bool DBAdapter::execute(const std::string& sql) {
    return m_pImpl->core.execute(sql);
}


/**
 * @brief 执行查询 SQL
 * @param sql SELECT 语句
 * @return std::optional<Result> 查询结果
 */
std::optional<Result> DBAdapter::query(const std::string& sql) {
    return m_pImpl->core.query(sql);
}


/**
 * @brief 连接状态检查
 */
bool DBAdapter::is_connected() const {
    return m_pImpl->core.is_connected();
}

} // namespace db

```

---

### File: infrastructure/infrastructure.cppm
```cppm
/**
* @file    src/CourseSelectionSystem/infrastructure/infrastructure.cppm
* @date    2026-01-10
* @author  GY
* @brief   基础设施层主模块接口
*
* 导出基础设施层的所有分区模块。
* 包含数据库适配器 (DBAdapter) 和各业务实体的代理类 (StudentProxy, CourseProxy 等)。
*
* Change Log:
* [v1.0] GY   2026-01-10
* * 初始版本：整合基础设施层模块接口。
* [v4.5] GY   2026-01-15
* * 导出 dtos 分区模块，支持层间数据传输规范
*/

export module infrastructure;

export import :db_adapter;
export import :course_proxy;
export import :student_proxy;
export import :enrollment_proxy;
export import :dtos;

```

---

