/**
* @file    src/CourseSelectionSystem/course_system.cppm
* @brief   Primary module interface
*/
export module course_system;

// 1. 领域层 (分文件，需分别导出)
export import :domain.timeslot;
export import :domain.course;
export import :domain.student;
export import :domain.teacher;
export import :domain;

// 2. 基础设施层 (合并文件，整体导出)
export import :infrastructure;

// 3. 应用层与表现层
export import :app.controller;
export import :presentation;
