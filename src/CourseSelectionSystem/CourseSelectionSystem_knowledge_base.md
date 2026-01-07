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

# 1. 定义各层级的模块文件列表 (Code Grouping)
add_executable(CourseSelectionSystem
    main.cpp
)

# 表现层模块 (Presentation Layer)
set(PRESENTATION_MODULES
    # CourseSelectionSystem/presentation/.cppm
    # presentation/pre.cppm
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
)

# 数据管理层模块 (Infrastructure Layer)
set(INFRASTRUCTURE_MODULES
    # CourseSelectionSystem/infrastructure/.cppm
    infrastructure/infra.db_adapter.cppm
)

# 2. 配置 IDE 中的显示分组 (IDE Display Grouping)
source_group("Presentation" FILES ${PRESENTATION_MODULES})
source_group("Application"  FILES ${APPLICATION_MODULES})
source_group("Domain"       FILES ${DOMAIN_MODULES})
source_group("Infrastructure" FILES ${INFRASTRUCTURE_MODULES})

# 3. 构建目标与源文件关联
# 将所有模块添加到目标中
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

# 4. 数据库依赖 (libpqxx)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

target_link_libraries(CourseSelectionSystem
    PRIVATE
        ${PQXX_LIBRARIES}
)
target_include_directories(CourseSelectionSystem
    PRIVATE
        ${PQXX_INCLUDE_DIRS}
)
target_link_directories(CourseSelectionSystem
    PRIVATE
        ${PQXX_LIBRARY_DIRS}
)


```

---

## 项目源文件
---

### File: main.cpp
```cpp
/**
* @file    /run/media/root/铠侠D/桌面文件/开源技术群资料/考试/CourseSelectionSystem/CourseSelectionSystem/src/CourseSelectionSystem/main.cpp
* @date    2026-01-06
* @author  GY
* @brief   Main entry point
*
* main:程序主入口文件
* 负责创建 SystemController 实例并启动选课系统
* 捕获并处理运行时异常，确保程序异常退出时有错误提示
*
* Change Log:
* [v1.0] GY   2026-01-06
* * 优化了cmake,实现学生选课系统基础
*/
import std;
import course_system; // 导入主模块

int main() {
    try {
        SystemController app;
        app.initialize();
        app.run();
    } catch (const std::exception& e) {
        std::print("Fatal Error: {}\n", e.what());
        return 1;
    }
    return 0;
}


```

---

### File: course_system.cppm
```cppm
/**
* @file    src/CourseSelectionSystem/course_system.cppm
* @date    2026-01-06
* @author  GY
* @brief   Primary module interface
*
* course_system:学生选课系统主模块接口
* 聚合系统的各个功能模块（Domain、Application、Controller等）
* 作为对外暴露的统一接口，方便其他模块导入使用
*
* Change Log:
* [v1.0] GY   2026-01-06
* * 优化了cmake,实现学生选课系统基础
*/
export module course_system;

export import :domain;
export import :app.controller;
export import :infrastructure;
// export import :presentation;   // 后续添加

```

---

### File: postgre_demo_knowledge_base.md
```md
# 项目: postgre_demo

## 项目特征总结





---

## 构建配置文件
---

### File: CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 4.1.1)

#https://github.com/Kitware/CMake/blob/v4.1.1/Help/dev/experimental.rst

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
set(CMAKE_CXX_MODULE_STD ON)

project(postgre_demo LANGUAGES CXX)

find_package(PkgConfig REQUIRED)        # 加载 PkgConfig 模块，用于通过 .pc 文件查找库
pkg_check_modules(PQXX REQUIRED libpqxx) # 使用 pkg-config 查找 libpqxx 库并定义 PQXX_* 变量

# set(MODULE_LIB_DIR1 /opt/utils/modules)  # 模块库的目录的设置
add_executable(postgre_demo
    #math.cpp                              # 模块实现单元
    main.cpp
)

set(MODULE_INTERFACE_FILES
    #${MODULE_LIB_DIR1}/utils.cppm         #添加模块库的模块接口单元
    #math.cppm                             # 模块接口单元
    testdb.cppm
    grade.cppm
    db.cppm
    app.cppm
)

source_group("Module Interfaces" FILES ${MODULE_INTERFACE_FILES})
target_sources(postgre_demo  PRIVATE FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS                              # 配置BASE_DIRS，用于组织构建输出目录，以便构建不同目录中(同名的)模块文件.
        #${MODULE_LIB_DIR1}
        ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${MODULE_INTERFACE_FILES}
)
target_compile_features(postgre_demo PRIVATE cxx_std_23)

target_link_libraries(postgre_demo PRIVATE ${PQXX_LIBRARIES})      # 链接 libpqxx 动态库/静态库
target_include_directories(postgre_demo PRIVATE ${PQXX_INCLUDE_DIRS}) # 添加 libpqxx 的头文件搜索路径
target_link_directories(postgre_demo PRIVATE ${PQXX_LIBRARY_DIRS})    # 添加 libpqxx 的库文件搜索路径












































# target_link_directories(postgre_demo PRIVATE /opt/utils/lib)  # -L /opt/utils/lib 设置链接路径
# target_link_libraries(postgre_demo PRIVATE utils)             # -lutils           设置链接库名

# set_target_properties(postgre_demo PROPERTIES
#     INSTALL_RPATH_USE_LINK_PATH  True                           # 使用链接路径作为安装文件的RPath，等价于"-Wl,-rpath=/opt/utils/lib"
# )

#add_library(postgre_demo SHARED
#  utils.cpp
#)

#set_target_properties( postgre_demo PROPERTIES
#    PUBLIC_HEADER utils.cppm
#    VERSION   2.5.1
#    SOVERSION 2
#    LIBRARY_OUTPUT_DIRECTORY  "${CMAKE_BINARY_DIR}/lib"
#)

#install(TARGETS postgre_demo
#    LIBRARY DESTINATION /opt/utils/lib
#    PUBLIC_HEADER DESTINATION  /opt/utils/modules
#)

#add_custom_command(TARGET postgre_demo
#    POST_BUILD
#    COMMAND cmake --install "${CMAKE_CURRENT_BINARY_DIR}"
#)

```

---

## 项目源文件
---

### File: grades.dat
```dat
张三 85 90 80 85 90
李四 78 82 75 80 85
王五 92 88 90 85 95
赵六 88 85 82 88 90
钱七 95 92 90 92 88
```

---

### File: testdb.cppm
```cppm
/**
 * @file    testdb.cppm
 * @brief   testDB 主模块接口
 * * 统一导出各功能分区，作为外部访问入口。
 */
export module testdb;

export import :grade; // 导出成绩数据模型分区
export import :db;    // 导出数据库操作分区
export import :app;   // 导出应用程序逻辑分区

```

---

### File: grade.cppm
```cppm
/**
 * @file    grade.cppm
 * @brief   Module partition: grade - Defines Grade class
 * * 封装学生成绩数据与计算逻辑。
 */
export module testdb:grade;

import std;

using std::string;
using std::vector;
using std::ifstream;
using std::istringstream;
using std::max;

export class Grade {
public:
    Grade(string name, double mid, double fin, vector<double> hw);
    double getFinal(); //get the final score.
    string getName();  //return a name.
    double getMedian();//get the median score from homework.
private:
    string m_name;
    double m_mid;
    double m_fin;
    vector<double> m_homework;
};

// ----- Implementation -----

Grade::Grade(string name, double mid, double fin, vector<double> hw)
    : m_name{name}, m_mid{mid}, m_fin{fin}, m_homework{hw}
{}

double Grade::getFinal()
{
   return 0.2 * m_mid + 0.4 * m_fin + 0.4 * getMedian();
}

string Grade::getName()
{
    return m_name;
}

double Grade::getMedian()
{
    typedef vector<double>::size_type vec_sz;
    vec_sz size {m_homework.size()};
    if (size == 0) return 0.0; // 简单防护空作业情况
    if (size % 2 == 0) {
        return (m_homework[size / 2 - 1] + m_homework[size / 2]) / 2;
    } else {
        return m_homework[size / 2];
    }
}

```

---

### File: db.cppm
```cppm
/**
 * @file    db.cppm
 * @brief   Module partition: db - Defines OperationToDB class
 * * 封装 PostgreSQL 数据库的 CRUD 操作。
 * 使用 Global Module Fragment 处理非模块化的 pqxx 头文件。
 *
 * PostgreSQL相关说明:
 * - PostgreSQL是功能强大的开源对象关系数据库系统
 * - 使用pqxx库作为C++的PostgreSQL客户端接口
 * - 本模块演示了基本的CRUD操作：Create(创建表/插入数据)、Read(查询)、Update(未实现)、Delete(删除)
 *
 * pqxx库核心类说明:
 * - pqxx::connection: 数据库连接对象，通过连接字符串建立连接
 * - pqxx::work: 事务对象，用于执行需要提交的SQL操作（INSERT/UPDATE/DELETE/DROP等）
 * - pqxx::nontransaction: 非事务对象，用于只读查询（SELECT），不需要提交
 * - pqxx::result: 存储查询结果的容器，可以遍历访问每一行
 */
module;
#include <pqxx/pqxx>

export module testdb:db;


import std;
import :grade;

using std::string;
using std::vector;
using std::to_string;
using std::print;
using std::cin;
using namespace pqxx;

export class OperationToDB {
public:
    OperationToDB(string name, string user, string password);
    // 禁止拷贝，防止潜在的资源管理问题
    OperationToDB(const OperationToDB&) = delete;
    OperationToDB& operator=(const OperationToDB&) = delete;
    ~OperationToDB() { std::print("OperationToDB destroyed\n"); }

    void deleteOneFromTB();
    void deleteAllFromTB();
    void insertAllToTB(const vector<Grade>& g);
    void selectFromTB();
    void dropTable();
private:
    string m_dbname;
    string m_user;
    string m_password;
};

// ----- Implementation -----

OperationToDB::OperationToDB(string name, string user, string password)
    : m_dbname{name}, m_user{user}, m_password{password}
{}

void OperationToDB::deleteOneFromTB()
{
    /*connect database*/
    // PostgreSQL连接字符串格式: dbname=数据库名 user=用户名 password=密码 hostaddr=IP地址 port=端口号
    string sql;
    string address = "dbname = " + m_dbname + " user=" + m_user + " password=" + m_password + " hostaddr=127.0.0.1 port=5432";

    try {
        // pqxx::connection: 建立与PostgreSQL数据库的连接
        connection C(address);
        if (!C.is_open()) {
            print("无法打开数据库\n");
            return;
        }
        /*init sql with the function delete*/
        string name;
        print("请输入要删除的学生姓名: ");
        cin >> name;
        // SQL DELETE语法: DELETE FROM 表名 WHERE 条件
        // 这里删除grade表中name字段等于指定姓名的记录
        sql = "delete from grade where name = '" + name + "'";

        // pqxx::work: 事务对象，用于执行需要提交的SQL操作（INSERT/UPDATE/DELETE等）
        work W(C);
        /* Execute SQL query */
        // W.exec(): 执行SQL语句
        W.exec(sql);
        // W.commit(): 提交事务，使更改生效
        W.commit();
        print("删除成功\n");
    } catch (const std::exception &e) {
        print("Database error: {}\n", e.what());
    }
}

void OperationToDB::deleteAllFromTB()
{
    /*connect database*/
    string sql;
    string address = "dbname = " + m_dbname + " user=" + m_user + " password=" + m_password + " hostaddr=127.0.0.1 port=5432";

    try {
        connection C(address);
        if (!C.is_open()) {
            print("无法打开数据库\n");
            return;
        }

        char confirm;
        print("警告：这将清空 'grade' 表中的所有数据！确定要继续吗？(y/n): ");
        cin >> confirm;
        if (confirm != 'y' && confirm != 'Y') {
            print("操作已取消\n");
            return;
        }

        // SQL DELETE语法: DELETE FROM 表名 (不带WHERE子句会删除所有记录)
        sql = "delete from grade";

        work W(C);
        /* Execute SQL query */
        W.exec(sql);
        W.commit();
        print("清空成功\n");
    } catch (const std::exception &e) {
        print("Database error: {}\n", e.what());
    }
}

void OperationToDB::insertAllToTB(const vector<Grade>& g)
{
    // Fix: Check if data exists before connecting
    if (g.empty()) {
        print("警告：内存中没有学生数据！请检查 grades.dat 是否存在且已正确读取。\n");
        return;
    }

    /*connect sql*/
     string address = "dbname = " + m_dbname + " user=" + m_user + " password=" + m_password + " hostaddr=127.0.0.1 port=5432";

     try {
         connection C(address);
         if (!C.is_open()) {
             print("无法打开数据库\n");
             return;
         }

         // 1. 确保表存在
         {
             work W_init(C);
             // SQL CREATE TABLE语法: CREATE TABLE IF NOT EXISTS 表名 (字段名 数据类型, ...)
             // TEXT: PostgreSQL文本类型，可变长度字符串
             // DOUBLE PRECISION: PostgreSQL双精度浮点数类型（8字节，约15位精度）
             W_init.exec("CREATE TABLE IF NOT EXISTS grade (name TEXT, score DOUBLE PRECISION)");
             W_init.commit();
         }

         long count = 0;
         // 2. 检查现有数据
         {
             // pqxx::nontransaction: 非事务对象，用于只读查询（SELECT），不需要提交
             nontransaction N(C);
             try {
                // SQL SELECT语法: SELECT count(*) FROM 表名 (统计记录数)
                result r = N.exec("SELECT count(*) FROM grade");
                if (!r.empty()) {
                    count = r[0][0].as<long>();
                }
             } catch(...) {}
         }

         bool do_clear = false;
         if (count > 0) {
             print("检测到数据库中已有 {} 条记录。\n", count);
             print("请选择操作模式: [1] 清空并重新录入 (推荐)  [2] 追加模式  [0] 取消 : ");
             int choice;
             if (!(cin >> choice)) {
                 cin.clear();
                 cin.ignore(1000, '\n');
                 choice = 0;
             }

             if (choice == 1) do_clear = true;
             else if (choice == 2) do_clear = false;
             else { print("操作已取消。\n"); return; }
         }

         // 3. 执行插入
         {
             work W(C);

             if (do_clear) {
                 W.exec("DELETE FROM grade");
                 print("旧数据已清空。\n");
             }

             print("--------------------------------------\n");
             for(const auto& student : g){
                 Grade temp = student;
                 string name = temp.getName();
                 // SQL INSERT语法: INSERT INTO 表名 (字段名1, 字段名2) VALUES (值1, 值2)
                 // 字符串值需要用单引号括起来，数值类型不需要
                 string sql = "insert into grade(name,score) values('" + name + "'," + to_string(temp.getFinal()) + ")";
                 W.exec(sql);
                 print("成功插入 {} 的信息\n", name);
             }
             W.commit();
             print("---------------------------------------\n");
             print("数据处理完成。\n");
         }

     } catch (const std::exception &e) {
         print("发生错误: {}\n", e.what());
     }
 }

void OperationToDB::selectFromTB()
{
    string address = "dbname = " + m_dbname + " user=" + m_user + " password=" + m_password + " hostaddr=127.0.0.1 port=5432";

    try {
        connection C(address);
        if (!C.is_open()) {
            print("无法打开数据库\n");
            return;
        }
        /*init sql*/
        string sql;
        string name1;
        print("请输入学生姓名 (输入 '*' 查询所有): ");
        cin >> name1;

        if(name1 == "*") {
            // SQL SELECT语法: SELECT DISTINCT 字段1, 字段2 FROM 表名
            // DISTINCT: 去除重复记录
            sql = "select distinct name, score from grade";
        } else {
            // SQL SELECT语法: SELECT ... FROM 表名 WHERE 条件
            // WHERE子句用于筛选符合条件的记录
            sql = "select distinct name, score from grade where name = '" + name1 + "'";
        }

        nontransaction ntx(C);
        // pqxx::result: 存储查询结果的容器
        result r(ntx.exec(sql));
        if (r.empty()) {
            print("未找到相关记录。\n");
        } else {
            // 遍历结果集的每一行
            for(const auto& row : r) {
                // row[列索引].as<类型>() 将字段值转换为指定C++类型
                print("姓名: {} 成绩: {}\n", row[0].as<string>(), row[1].as<double>());
            }
        }
    } catch (const std::exception &e) {
        print("Query error: {}\n", e.what());
    }
}

void OperationToDB::dropTable()
{
    string address = "dbname = " + m_dbname + " user=" + m_user + " password=" + m_password + " hostaddr=127.0.0.1 port=5432";

    try {
        connection C(address);
        if (!C.is_open()) {
            print("无法打开数据库\n");
            return;
        }

        char confirm;
        print("严重警告：这将完全删除 'grade' 表结构！确定要继续吗？(y/n): ");
        cin >> confirm;
        if (confirm != 'y' && confirm != 'Y') {
            print("操作已取消\n");
            return;
        }

        // SQL DROP TABLE语法: DROP TABLE 表名
        // 警告: 此操作将删除表及其所有数据，不可恢复！
        string sql = "drop table grade";

        work W(C);
        /* Execute SQL query */
        W.exec(sql);
        W.commit();
        print("表删除成功\n");
    } catch (const std::exception &e) {
        print("Drop error: {}\n", e.what());
    }
}

```

---

### File: app.cppm
```cppm
/**
 * @file    app.cppm
 * @brief   Module partition: app - Defines application control logic
 * * 包含文件读取、菜单循环等核心业务流程。
 */
export module testdb:app;

import std;
import :grade;
import :db;

using std::vector;
using std::string;
using std::ifstream;
using std::print;
using std::cin;
using std::endl; // For legacy compatibility logic

export void read(vector<Grade> &g);
export int selectFunction(OperationToDB& temp, vector<Grade>& g);
export void operateToGrade(vector<Grade> &g);

// ----- Implementation -----

void read(vector<Grade> &g)
{
    ifstream ifs("../../grades.dat");
    // 增加文件打开检查
    if (!ifs) {
        print("Error: 无法打开 '../../grades.dat'！\n");
        try {
             print("尝试读取路径: {}\n", std::filesystem::absolute("grades.dat").string());
        } catch(...) {}
        return;
    }

    while(ifs) {
        double mid, fin;
        string name;
        ifs >> name >> mid >> fin;
        vector<double> hw;
        if(ifs) {
            double x;
            // 注意：这里原本的逻辑是嵌套读取，保持原样
            // 但原逻辑可能存在行尾处理问题，这里尽量保持结构不变
            // 使用 getline 处理单行作业可能更稳健，但遵循“逻辑不变”原则
            while(ifs >> x) {
                hw.push_back(x);
                // 简单的退出条件检测，防止死循环读取下一行作为数字
                if (ifs.peek() == '\n' || ifs.eof()) break;
            }
            // 清除状态以便继续读取（如果不是eof）
            if (!ifs.eof()) ifs.clear();

            Grade student(name, mid, fin, hw);
            g.push_back(student);
        }
    }
}

int selectFunction(OperationToDB& temp, vector<Grade>& g)
{
    int flag;
    print("\n=== 学生成绩数据库管理系统 ===\n");
    print("1. 录入所有学生数据 (从文件)\n");
    print("2. 查询学生信息\n");
    print("3. 删除学生信息\n");
    print("4. 清空表数据\n");
    print("5. 删除表结构\n");
    print("0. 退出系统\n");
    print("请选择操作: ");
    cin >> flag;

    switch (flag) {
    case (1): temp.insertAllToTB(g);
        break;
    case (2): temp.selectFromTB();
        break;
    case (3): temp.deleteOneFromTB();
        break;
    case (4): temp.deleteAllFromTB();
        break;
    case (5): temp.dropTable();
        break;
    case (0):
        print("退出系统。\n");
        return 0;
    default:
        print("无效的选择，请重试。\n");
        break;
    }

    return flag;
}

void operateToGrade(vector<Grade> &g)
{
    // 默认数据库连接信息
    string dbname = "helloword";
    string user = "postgres";
    string passwd = "123";

    OperationToDB grade(dbname, user, passwd);

    print("首次执行未录入数据库信息？请务必先执行'1'倒入数据\n");

    while (true) {
        int flag = selectFunction(grade, g);
        if (flag == 0) {
            break;
        }
    }
}

```

---

### File: main.cpp
```cpp
/**
 * @file    main.cpp
 * @brief   程序入口
 * * 协调读取、数据库操作与交互循环。
 * 引入了顶层错误处理机制。
 */
import std;
import testdb; // 导入主模块

using std::vector;
using std::print;
using std::exception;

int main()
{
    vector<Grade> students;

    try {
        read(students);
        operateToGrade(students);
    } catch (const exception& e) {
        print("报错: {}\n", e.what());
    } catch (...) {
        print("Unknown fatal error!\n");
    }

    // 打印当前工作目录，方便调试文件路径问题
    print("当前程序工作目录：{}\n", std::filesystem::current_path().string());

    students.clear();
    print("Exiting main...\n");
    return 0;
}

```

---


```

---

### File: application/app.controller.cppm
```cppm
/**
* @file    src/CourseSelectionSystem/application/app.controller.cppm
* @date    2026-01-06
* @author  GY
* @brief   Application partition: System Controller
*
* app.controller:应用层系统控制器模块
* 负责管理整个选课系统的运行流程
* 维护学生和课程列表，提供选课、退课等核心业务功能
* 协调领域层实体之间的交互
*
* Change Log:
* [v1.0] GY   2026-01-06
* * 初始化项目架构 (基于 C++23 Modules)
* * 实现领域层实体 (Student, Course) 及其核心逻辑
* * 实现应用层控制器 (SystemController) 管理选课流程
* * 实现选课与退课功能，包含容量检查与重复选课验证
* * 添加基础测试用例与 CLI 输出
*/
export module course_system:app.controller;

import :domain;
import :infrastructure;
import std;

export class SystemController {
public:
    // 初始化系统
    void initialize();

    // 运行系统
    void run();

    // 执行选课操作
    void performEnrollment(std::string sid, std::string cid);

    // 执行退课操作
    void performDrop(std::string sid, std::string cid);

private:
    std::vector<Student*> m_students; // 学生列表
    std::vector<Course*> m_courses;   // 课程列表
    db::DBAdapter m_db;               // 数据库适配器

    // 根据ID查找学生
    Student* findStudent(std::string_view id);

    // 根据ID查找课程
    Course* findCourse(std::string_view id);
};

// --- Implementation ---

/**
* @brief 初始化系统
* 创建初始的学生和课程数据，并与数据库同步
*/
void SystemController::initialize() {
    // 0. 数据库连接 (PostgreSQL)
    if (!m_db.connect("dbname=CourseSelectionSystem user=postgres password=123 hostaddr=127.0.0.1 port=5432")) {
        std::print("[App] Warning: Database connection failed. Running in memory-only mode.\n");
        goto LOAD_MOCK;
    }

    std::print("[App] Database connected successfully.\n");
    
    // 1. 初始化表结构 (Schema Migration)
    m_db.execute("CREATE TABLE IF NOT EXISTS students (id TEXT PRIMARY KEY, name TEXT)");
    m_db.execute("CREATE TABLE IF NOT EXISTS courses (id TEXT PRIMARY KEY, name TEXT, capacity INT)");
    m_db.execute("CREATE TABLE IF NOT EXISTS enrollments (student_id TEXT, course_id TEXT, PRIMARY KEY(student_id, course_id))");

    // 2. 尝试从数据库加载数据
    {
        auto res = m_db.query("SELECT count(*) FROM students");
        if (res && !res->empty() && std::stoi((*res)[0][0]) > 0) {
            std::print("[App] Loading data from database...\n");
            
            // 加载学生
            auto s_res = m_db.query("SELECT id, name FROM students");
            if (s_res) {
                for (const auto& row : *s_res) {
                    m_students.push_back(new Student(row[0], row[1]));
                }
            }

            // 加载课程
            auto c_res = m_db.query("SELECT id, name, capacity FROM courses");
            if (c_res) {
                for (const auto& row : *c_res) {
                    int cap = 60; 
                    try { cap = std::stoi(row[2]); } catch(...) {}
                    m_courses.push_back(new Course(row[0], row[1], cap));
                }
            }

            // 加载选课记录并恢复内存状态
            auto e_res = m_db.query("SELECT student_id, course_id FROM enrollments");
            if (e_res) {
                for (const auto& row : *e_res) {
                    auto s = findStudent(row[0]);
                    auto c = findCourse(row[1]);
                    if (s && c) s->enrollIn(c);
                }
            }
            
            std::print("[App] Data loaded: {} students, {} courses.\n", m_students.size(), m_courses.size());
            return;
        }
    }

LOAD_MOCK:
    std::print("[App] Database empty or unavailable. Initializing with Mock Data and syncing to DB...\n");

    // 1. 初始化学生 (Real Data)
    m_students.push_back(new Student("2024051604085", "Gao Yang"));

    // 2. 初始化课程
    m_courses.push_back(new Course("C0001", "高等数学"));
    m_courses.push_back(new Course("C0002", "马克思主义基本原理"));
    m_courses.push_back(new Course("C0003", "软件构建与实现"));
    m_courses.push_back(new Course("C0004", "计算机网络"));
    m_courses.push_back(new Course("C0005", "计算机导论"));
    m_courses.push_back(new Course("C0006", "线性代数"));
    m_courses.push_back(new Course("C0007", "概率论与数理统计"));
    m_courses.push_back(new Course("C0008", "数据结构"));
    m_courses.push_back(new Course("C0009", "数据库原理及应用"));
    m_courses.push_back(new Course("C0010", "思想道德与法治"));
    m_courses.push_back(new Course("C0011", "形势与政策"));
    m_courses.push_back(new Course("C0012", "大学英语"));
    m_courses.push_back(new Course("C0013", "大学生心理健康教育"));
    m_courses.push_back(new Course("C0014", "体育"));
    m_courses.push_back(new Course("C0015", "中国近现代史纲要"));
    m_courses.push_back(new Course("C0016", "Linux程序设计"));
    m_courses.push_back(new Course("C0017", "C语言程序设计"));

    // 3. 将 Mock 数据写入数据库
    if (m_db.is_connected()) {
        m_db.execute("INSERT INTO students VALUES ('2024051604085', 'Gao Yang')");
        m_db.execute("INSERT INTO courses VALUES ('C0001', '高等数学', 60)");
        m_db.execute("INSERT INTO courses VALUES ('C0002', '马克思主义基本原理', 60)");
        m_db.execute("INSERT INTO courses VALUES ('C0003', '软件构建与实现', 60)");
        m_db.execute("INSERT INTO courses VALUES ('C0004', '计算机网络', 60)");
        m_db.execute("INSERT INTO courses VALUES ('C0005', '计算机导论', 60)");
        m_db.execute("INSERT INTO courses VALUES ('C0006', '线性代数', 60)");
        m_db.execute("INSERT INTO courses VALUES ('C0007', '概率论与数理统计', 60)");
        m_db.execute("INSERT INTO courses VALUES ('C0008', '数据结构', 60)");
        m_db.execute("INSERT INTO courses VALUES ('C0009', '数据库原理及应用', 60)");
        m_db.execute("INSERT INTO courses VALUES ('C0010', '思想道德与法治', 60)");
        m_db.execute("INSERT INTO courses VALUES ('C0011', '形势与政策', 60)");
        m_db.execute("INSERT INTO courses VALUES ('C0012', '大学英语', 60)");
        m_db.execute("INSERT INTO courses VALUES ('C0013', '大学生心理健康教育', 60)");
        m_db.execute("INSERT INTO courses VALUES ('C0014', '体育', 60)");
        m_db.execute("INSERT INTO courses VALUES ('C0015', '中国近现代史纲要', 60)");
        m_db.execute("INSERT INTO courses VALUES ('C0016', 'Linux程序设计', 60)");
        m_db.execute("INSERT INTO courses VALUES ('C0017', 'C语言程序设计', 60)");
        
        std::print("[App] Seed data written to database.\n");
    }

    std::print("Loaded {} students and {} courses.\n", m_students.size(), m_courses.size());
}


/**
* @brief 运行系统
* 启动系统并执行选课操作
*/
void SystemController::run() {
    std::print("System Started.\n");
    std::string sid = "2024051604085";

    // 1. 初始选课
    std::print("\n=== Enrollment Tests ===\n");
    performEnrollment(sid, "C0017"); // C语言
    performEnrollment(sid, "C0001"); // 高数
    performEnrollment(sid, "C0008"); // 数据结构

    // 2. 尝试退课
    std::print("\n=== Drop Tests ===\n");
    performDrop(sid, "C0001"); // 退选高数

    // 3. 再次选课
    std::print("\n=== Re-Enrollment Tests ===\n");
    performEnrollment(sid, "C0016"); // Linux程序设计

    // 4. 尝试退掉未选的课
    std::print("\n=== Invalid Drop Tests ===\n");
    performDrop(sid, "C0005"); // 计算机导论 (未选)
}


/**
* @brief 执行选课操作
* @param sid 学生ID
* @param cid 课程ID
*/
void SystemController::performEnrollment(std::string sid, std::string cid) {
    auto s = findStudent(sid);
    auto c = findCourse(cid);

    if (s && c) {
        s->enrollIn(c);
        
        // 数据库同步
        if (m_db.is_connected()) {
            std::string sql = std::format("INSERT INTO enrollments VALUES ('{}', '{}')", sid, cid);
            // execute 内部已处理异常（如重复插入），这里无需额外 catch
            m_db.execute(sql);
        }
    } else {
        std::print("Error: Student ({}) or Course ({}) not found.\n", sid, cid);
    }
}

/**
* @brief 执行退课操作
* @param sid 学生ID
* @param cid 课程ID
*/
void SystemController::performDrop(std::string sid, std::string cid) {
    auto s = findStudent(sid);
    auto c = findCourse(cid);

    if (s && c) {
        s->dropCourse(c);
        
        // 数据库同步
        if (m_db.is_connected()) {
            std::string sql = std::format("DELETE FROM enrollments WHERE student_id='{}' AND course_id='{}'", sid, cid);
            m_db.execute(sql);
        }
    } else {
        std::print("Error: Student ({}) or Course ({}) not found.\n", sid, cid);
    }
}


/**
* @brief 根据ID查找学生
* @param id 学生ID
* @return 找到的学生指针，未找到返回 nullptr
*/
Student* SystemController::findStudent(std::string_view id) {
    for (auto s : m_students) {
        if (s->hasId(id)) return s;
    }
    return nullptr;
}


/**
* @brief 根据ID查找课程
* @param id 课程ID
* @return 找到的课程指针，未找到返回 nullptr
*/
Course* SystemController::findCourse(std::string_view id) {
    for (auto c : m_courses) {
        if (c->hasId(id)) return c;
    }
    return nullptr;
}

```

---

### File: domain/dom.course.cppm
```cppm
/**
* @file    src/CourseSelectionSystem/domain/dom.course.cppm
* @date    2026-01-06
* @author  GY
* @brief   Domain partition: Course entity
*
* dom.course:领域层课程实体模块
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
*/
export module course_system:domain.course;

import std;

// 前向声明，解决循环引用
export class Student;


export class Course {
public:
    // 构造函数
    Course(std::string id, std::string name, int capacity = 60);


    // 检查课程是否已满
    bool isFull() const;

    // 尝试接受学生报名
    bool acceptEnrollment(Student* s);

    // 移除学生报名信息
    void removeEnrollment(Student* s);

    // 获取课程 ID
    std::string getId() const { return m_id; }

    // 检查 ID 是否匹配
    bool hasId(std::string_view id) const;

    // 获取课程详细信息字符串
    std::string course_info() const;

private:
    std::string m_id;                 // 课程 ID
    std::string m_name;               // 课程名称
    int m_capacity;                   // 最大容量
    std::vector<Student*> m_students; // 已选修该课程的学生列表
};

// --- Implementation ---
Course::Course(std::string id, std::string name, int capacity)
    : m_id(id), m_name(name), m_capacity(capacity) {}


/**
* @brief 检查课程是否已满
* @return true 如果当前选课人数 >= 容量，否则 false
*/
bool Course::isFull() const {
    return m_students.size() >= m_capacity;
}


/**
* @brief 尝试接受学生报名
* @param s 申请报名的学生指针
* @return true 报名成功, false 报名失败 (如课程已满)
*/
bool Course::acceptEnrollment(Student* s) {
    if (isFull()) return false;
    m_students.push_back(s);
    return true;
}


/**
* @brief 移除学生报名信息
* @param s 要移除的学生指针
*/
void Course::removeEnrollment(Student* s) {
    std::erase(m_students, s);
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
 * @return 格式化后的课程信息 (ID - Name (Current/Max))
 */
std::string Course::course_info() const {
    return std::format("[Course] {} - {} ({}/{})",
        m_id, m_name, m_students.size(), m_capacity);
}

```

---

### File: domain/dom.student.cppm
```cppm
/**
* @file    src/CourseSelectionSystem/domain/dom.student.cppm
* @date    2026-01-06
* @author  GY
* @brief   Domain partition: Student entity
*
* dom.student:领域层学生实体模块
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
*/
export module course_system:domain.student;

import std;

export class Course;


export class Student {
public:
    // 构造函数
    Student(std::string id, std::string name);

    // 选修课程
    void enrollIn(Course* c);

    // 退选课程
    void dropCourse(Course* c);

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
* @brief 检查学生是否匹配指定 ID
* @param id 要检查的 ID
* @return true 如果匹配，否则 false
*/
bool Student::hasId(std::string_view id) const {
    return m_id == id;
}


/**
* @brief 获取学生详细信息字符串
* @return 格式化后的学生信息 (ID - Name)
*/
std::string Student::student_info() const {
    return std::format("[Student] {} - {}", m_id, m_name);
}

```

---

### File: domain/domain.cppm
```cppm
/**
* @file    src/CourseSelectionSystem/domain/domain.cppm
* @date    2026-01-06
* @author  GY
* @brief   Domain layer aggregator
*
* domain:领域层聚合模块
* 聚合学生（Student）和课程（Course）实体模块
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
export module course_system:domain;

export import :domain.student;
export import :domain.course;

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

### File: infrastructure/infra.db_adapter.cppm
```cppm
/**
* @file    src/CourseSelectionSystem/infrastructure/infra.db_adapter.cppm
* @date    2026-01-07
* @author  GY
* @brief   Infrastructure partition: Database Adapter
*
* infra.db_adapter:基础设施层数据库适配器模块
* 封装 PostgreSQL 数据库访问逻辑，提供通用的执行 (execute) 和查询 (query) 接口。
*
* 设计说明 (C++23 Modules Integration):
* 1. 采用 Global Module Fragment (#include <pqxx/pqxx>) 以兼容非模块化的 pqxx 库。
* 2. 为了规避 C++ 模块严格的导出规则 (TU-local entity exposure)，本模块不在类成员中直接持有
*    pqxx::connection 实例，而是采用连接配置驱动的模式（或 PIMPL 的替代方案）。
* 3. 这种设计确保了模块接口的纯净性，同时解决了 GCC 15 等现代编译器对模块边界检查的严格限制。
*
* Change Log:
* [v1.0] GY   2026-01-07
* * 初始创建，实现基于 libpqxx 的基础 CRUD 封装
* * 适配 C++23 Modules 语法，解决模块环境下 pqxx 的包含冲突问题
*/
module;
#include <pqxx/pqxx>
#include <print>
#include <iostream>

export module course_system:infrastructure;

import std;

export namespace db {

    using Row = std::vector<std::string>;
    using Result = std::vector<Row>;

    class DBAdapter {
    public:
        DBAdapter() = default;
        ~DBAdapter() = default;

        // 保存连接信息
        void set_credentials(std::string conn_str) {
            m_conn_str = conn_str;
        }

        // 兼容旧接口，这里 connect 只是保存字符串并测试一次连接
        bool connect(const std::string& conn_str) {
            m_conn_str = conn_str;
            try {
                pqxx::connection C(m_conn_str);
                if (C.is_open()) {
                    std::print("[DB] Connection check passed: {}\n", C.dbname());
                    return true;
                }
            } catch (const std::exception& e) {
                std::print(stderr, "[DB Exception] Connect check: {}\n", e.what());
            }
            return false;
        }

        bool execute(const std::string& sql) {
            try {
                pqxx::connection C(m_conn_str);
                pqxx::work W(C);
                W.exec(sql);
                W.commit();
                return true;
            } catch (const std::exception& e) {
                std::print(stderr, "[DB Exception] Execute: {}\nSQL: {}\n", e.what(), sql);
                return false;
            }
        }

        std::optional<Result> query(const std::string& sql) {
            try {
                pqxx::connection C(m_conn_str);
                pqxx::nontransaction N(C);
                pqxx::result R(N.exec(sql));

                Result result_set;
                result_set.reserve(R.size());

                for (const auto& row : R) {
                    Row current_row;
                    current_row.reserve(row.size());
                    for (const auto& field : row) {
                        if (field.is_null()) current_row.push_back("");
                        else current_row.push_back(field.c_str());
                    }
                    result_set.push_back(std::move(current_row));
                }
                return result_set;
            } catch (const std::exception& e) {
                std::print(stderr, "[DB Exception] Query: {}\nSQL: {}\n", e.what(), sql);
                return std::nullopt;
            }
        }

        bool is_connected() const {
            return !m_conn_str.empty();
        }

    private:
        std::string m_conn_str;
        // 关键：不持有 pqxx::connection 成员，避免模块导出问题
    };

} // namespace db

```

---

