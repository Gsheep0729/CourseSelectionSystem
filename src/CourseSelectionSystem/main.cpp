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
* [v1.1] Zhang Tao   2026-01-07
* * 替换硬编码测试逻辑，新增CLI交互测试（调用UserInterface）
*/
import std;
import course_system; // 导入主模块（已包含表现层）

int main() {
    try {
        // 原有测试逻辑注释保留，后续集成时可恢复
        /*
        SystemController app;
        app.initialize();
        app.run();
        */

        // 新增：CLI交互测试（张涛负责的表现层测试）
        std::print("=== 选课系统 CLI 交互测试模式 ===\n");
        UserInterface cli;
        cli.showLoginMenu(); // 启动登录菜单，进入交互流程

    } catch (const std::exception& e) {
        std::print("Fatal Error: {}\n", e.what());
        return 1;
    }
    return 0;
}
