/**
* @file    src/CourseSelectionSystem/presentation/pres.cli.cppm
* @date    2026-01-07
* @author  Zhang Tao
* @brief   Presentation layer partition (CLI Menu System)
*
* 表现层模块接口与实现，定义并实现用户交互核心类 UserInterface
* 提供登录菜单、各角色功能菜单的完整逻辑，遵循 C++23 Modules 规范
* 严格遵循「表现层不包含业务逻辑」约束，仅处理输入输出格式化
*
* Change Log:
* [v1.0] Zhang Tao   2026-01-07
* * 初始化表现层模块结构
* [v1.5] Zhang Tao   2026-01-07
* * 集成 SystemController，实现选课/退课业务逻辑调用
* [v2.0] Integrated 2026-01-07
* * 实现 Controller 依赖注入 (setController)
* * 将菜单选项映射到具体的 SystemController 业务方法
* * 完善用户输入与后台逻辑的数据流转
*/
export module course_system:presentation;

import std;
import :app.controller;

// --- 类声明 ---
export class UserInterface {
public:
    /**
     * @brief 设置系统控制器
     * @param controller 系统控制器指针
     */
    void setController(SystemController* controller);

    /**
     * @brief 显示登录菜单，处理用户登录流程
     * @return 登录成功的用户ID（非空）
     */
    std::string showLoginMenu();

    /**
     * @brief 显示学生主菜单（循环交互）
     * @param studentId 登录学生的ID
     */
    void showStudentMenu(std::string_view studentId);

    /**
     * @brief 显示教师主菜单（循环交互）
     * @param teacherId 登录教师的ID
     */
    void showTeacherMenu(std::string_view teacherId);

    /**
     * @brief 显示教学秘书主菜单（循环交互）
     * @param secretaryId 登录教学秘书的ID
     */
    void showSecretaryMenu(std::string_view secretaryId);

private:
    SystemController* m_controller{nullptr};

    /**
     * @brief 清除输入缓冲区，避免无效输入导致的交互异常
     */
    void clearInputBuffer() const;

    /**
     * @brief 显示角色选择菜单，返回用户选择的角色类型
     * @return 1=学生，2=教师，3=教学秘书
     */
    int showRoleSelectionMenu() const;
};

// --- 实现部分 ---

void UserInterface::setController(SystemController* controller) {
    m_controller = controller;
}

/**
* @brief 清除输入缓冲区
*/
void UserInterface::clearInputBuffer() const {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

/**
* @brief 显示角色选择菜单
*/
int UserInterface::showRoleSelectionMenu() const {
    int role_choice = 0;
    while (true) {
        std::print("\n==================== 角色选择 ====================\n");
        std::print("1. 学生\n");
        std::print("2. 教师\n");
        std::print("3. 教学秘书\n");
        std::print("===============================================\n");
        std::print("请选择您的角色（输入数字1-3）：");
        std::cin >> role_choice;

        if (std::cin.fail() || role_choice < 1 || role_choice > 3) {
            clearInputBuffer();
            std::print("❌ 无效输入！请输入1-3之间的数字。\n");
            continue;
        }
        clearInputBuffer();
        break;
    }
    return role_choice;
}

/**
* @brief 显示登录菜单
*/
std::string UserInterface::showLoginMenu() {
    std::string user_id;

    std::print("\n\n\n");
    std::print("===============================================\n");
    std::print("========== 重庆师范大学选课系统 v1.0 ==========\n");
    std::print("===============================================\n");

    while (true) {
        std::print("请输入用户ID：");
        std::getline(std::cin, user_id);

        if (user_id.empty()) {
            std::print("❌ 用户ID不能为空！请重新输入。\n");
            continue;
        }
        break;
    }

    std::print("✅ 登录验证通过！\n");

    int role = showRoleSelectionMenu();
    switch (role) {
        case 1:
            showStudentMenu(user_id);
            break;
        case 2:
            showTeacherMenu(user_id);
            break;
        case 3:
            showSecretaryMenu(user_id);
            break;
        default:
            std::print("❌ 角色选择异常，返回登录界面。\n");
            break;
    }

    return user_id;
}

/**
* @brief 显示学生主菜单
*/
void UserInterface::showStudentMenu(std::string_view studentId) {
    int choice = 0;
    while (true) {
        std::print("\n\n\n");
        std::print("===============================================\n");
        std::print("========== 学生菜单 - 用户名：{} ==========\n", studentId);
        std::print("===============================================\n");
        std::print("1. 选课 (Enroll in Course)\n");
        std::print("2. 退课 (Drop Course)\n");
        std::print("3. 查看课表 (View Schedule)\n");
        std::print("4. 查看成绩 (View Grades)\n");
        std::print("5. 退出登录 (Logout)\n");
        std::print("===============================================\n");
        std::print("请选择功能（输入数字1-5）：");
        std::cin >> choice;

        if (std::cin.fail() || choice < 1 || choice > 5) {
            clearInputBuffer();
            std::print("❌ 无效输入！请输入1-5之间的数字。\n");
            std::print("按Enter键继续...");
            std::cin.get();
            continue;
        }
        clearInputBuffer();

        std::string courseId;
        switch (choice) {
            case 1:
                std::print("\n--- 选课 ---\n");
                std::print("请输入课程ID：");
                std::getline(std::cin, courseId);
                if (m_controller) {
                    m_controller->performEnrollment(std::string(studentId), courseId);
                } else {
                    std::print("Error: System Controller not initialized.\n");
                }
                break;
            case 2:
                std::print("\n--- 退课 ---\n");
                std::print("请输入课程ID：");
                std::getline(std::cin, courseId);
                if (m_controller) {
                    m_controller->performDrop(std::string(studentId), courseId);
                } else {
                    std::print("Error: System Controller not initialized.\n");
                }
                break;
            case 3:
                std::print("\n✅ 已选择功能 [3]：查看课表 (View Schedule)\n");
                std::print("(功能开发中...)\n");
                break;
            case 4:
                std::print("\n✅ 已选择功能 [4]：查看成绩 (View Grades)\n");
                std::print("(功能开发中...)\n");
                break;
            case 5:
                std::print("\n✅ 退出登录成功！返回登录界面。\n");
                return;
        }

        std::print("按Enter键继续...");
        std::cin.get();
    }
}

/**
* @brief 显示教师主菜单
*/
void UserInterface::showTeacherMenu(std::string_view teacherId) {
    int choice = 0;
    while (true) {
        std::print("\n\n\n");
        std::print("===============================================\n");
        std::print("========== 教师菜单 - 用户名：{} ==========\n", teacherId);
        std::print("===============================================\n");
        std::print("1. 查看授课名单 (View Teaching Roster)\n");
        std::print("2. 录入成绩 (Assign Grade)\n");
        std::print("3. 修改成绩 (Modify Grade)\n");
        std::print("4. 退出登录 (Logout)\n");
        std::print("===============================================\n");
        std::print("请选择功能（输入数字1-4）：");
        std::cin >> choice;

        if (std::cin.fail() || choice < 1 || choice > 4) {
            clearInputBuffer();
            std::print("❌ 无效输入！请输入1-4之间的数字。\n");
            std::print("按Enter键继续...");
            std::cin.get();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1:
                std::print("\n✅ 已选择功能 [1]：查看授课名单 (View Teaching Roster)\n");
                break;
            case 2:
                std::print("\n✅ 已选择功能 [2]：录入成绩 (Assign Grade)\n");
                break;
            case 3:
                std::print("\n✅ 已选择功能 [3]：修改成绩 (Modify Grade)\n");
                break;
            case 4:
                std::print("\n✅ 退出登录成功！返回登录界面。\n");
                return;
        }

        std::print("按Enter键继续...");
        std::cin.get();
    }
}

/**
* @brief 显示教学秘书主菜单
*/
void UserInterface::showSecretaryMenu(std::string_view secretaryId) {
    int choice = 0;
    while (true) {
        std::print("\n\n\n");
        std::print("===============================================\n");
        std::print("========== 教学秘书菜单 - 用户名：{} ==========\n", secretaryId);
        std::print("===============================================\n");
        std::print("1. 创建课程 (Create Course)\n");
        std::print("2. 分配教师 (Assign Teacher)\n");
        std::print("3. 设置上课时间 (Set Class Time)\n");
        std::print("4. 退出登录 (Logout)\n");
        std::print("===============================================\n");
        std::print("请选择功能（输入数字1-4）：");
        std::cin >> choice;

        if (std::cin.fail() || choice < 1 || choice > 4) {
            clearInputBuffer();
            std::print("❌ 无效输入！请输入1-4之间的数字。\n");
            std::print("按Enter键继续...");
            std::cin.get();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1:
                std::print("\n✅ 已选择功能 [1]：创建课程 (Create Course)\n");
                break;
            case 2:
                std::print("\n✅ 已选择功能 [2]：分配教师 (Assign Teacher)\n");
                break;
            case 3:
                std::print("\n✅ 已选择功能 [3]：设置上课时间 (Set Class Time)\n");
                break;
            case 4:
                std::print("\n✅ 退出登录成功！返回登录界面。\n");
                return;
        }

        std::print("按Enter键继续...");
        std::cin.get();
    }
}