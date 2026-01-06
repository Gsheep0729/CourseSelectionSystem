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
* [v2.0] GY   2026-01-06
* * 新增 Infrastructure 导出
*/
// export module course_system;
//
// export import :domain;
// export import :app.controller;
//
// export import :infrastructure.db_connection;
// export import :infrastructure.student_proxy;

// Because I removed import std, I don't need to do anything here regarding std.
// But wait, this file exports partitions.
// Does it import std? No, I don't see `import std;` in the file content provided earlier.
// Let's check the file content again.
export module course_system;

export import :domain;
export import :app.controller;

// export import :infrastructure.db_connection;
// export import :infrastructure.student_proxy;
