/**
* @file    src/CourseSelectionSystem/infrastructure/infrastructure.cppm
* @date    2026-01-10
* @author  GY
* @brief   基础设施层主模块接口
*
* 导出基础设施层的所有分区模块。
* 包含数据库适配器 (DBAdapter) 和各业务实体的代理类 (StudentProxy, CourseProxy 等)。
*
* Change Log:
* [v1.0] GY   2026-01-10
* * 初始版本：整合基础设施层模块接口。
*/

export module infrastructure;

export import :db_adapter;
export import :course_proxy;
export import :student_proxy;
export import :enrollment_proxy;
