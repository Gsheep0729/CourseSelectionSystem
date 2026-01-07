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

