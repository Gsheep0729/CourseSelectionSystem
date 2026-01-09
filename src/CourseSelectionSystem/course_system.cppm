/**
* @file    src/CourseSelectionSystem/course_system.cppm
* @date    2026-01-09
* @author  GY
* @brief   System Aggregator Module
*
* course_system: 系统聚合模块
* 
* 架构重构 (v2.5):
* 为了解决 GCC 14/15 对模块分区聚合的编译崩溃问题，
* 我们将所有子系统重构为完全独立的 C++ Modules:
* - domain: 领域层实体
* - infrastructure: 基础设施层 (数据库适配)
* - application: 应用层逻辑 (控制器)
* - presentation: 表现层 (CLI)
*
* 本模块仅作为统一入口，导出所有子模块。
*/
export module course_system;

export import domain;
export import infrastructure;
export import application;
export import presentation;
