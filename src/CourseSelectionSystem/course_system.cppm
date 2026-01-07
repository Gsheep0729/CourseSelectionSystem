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
* [v1.1] Zhang Tao   2026-01-07
* * 导入表现层（presentation）模块，支持CLI交互测试
*/
export module course_system;
export import :domain;
export import :app.controller;
// export import :infrastructure; // 后续添加
export import :presentation; // 新增：导出表现层模块
