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
// import std;
#include <print>
#include <exception>

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

