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
