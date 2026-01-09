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
*/

export module course_system;

export import domain;
export import infrastructure;
export import application;
export import presentation;
