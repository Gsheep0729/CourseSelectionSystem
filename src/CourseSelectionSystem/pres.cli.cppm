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
* * 定义 UserInterface 类及核心菜单接口
* * 实现登录菜单输入处理与角色选择
* * 实现学生、教师、教学秘书菜单循环逻辑
* * 添加输入校验与缓冲区清理，优化交互体验
* * 支持学生、教师、教学秘书三种角色的菜单交互
* [v1.1] Zhang Tao   2026-01-07
* * 优化交互页面空白大小（减少空行数量）
* * 调整菜单选项文字顺序（中文在前、英文在后）
*/
export module course_system:presentation;

import std;

// --- 类声明 ---
export class UserInterface {
public:
    /**
     * @brief 显示登录菜单，处理用户登录流程
     * @return 登录成功的用户ID（非空）
     * @note 仅做模拟登录：ID非空即通过，密码不校验，支持角色选择
     */
    std::string showLoginMenu();

    /**
     * @brief 显示学生主菜单（循环交互）
     * @param studentId 登录学生的ID
     * @note 提供选课、退课、查课表、查成绩功能选项，暂不绑定业务逻辑
     */
    void showStudentMenu(std::string_view studentId);

    /**
     * @brief 显示教师主菜单（循环交互）
     * @param teacherId 登录教师的ID
     * @note 提供查看授课名单、录入成绩、修改成绩功能选项，暂不绑定业务逻辑
     */
    void showTeacherMenu(std::string_view teacherId);

    /**
     * @brief 显示教学秘书主菜单（循环交互）
     * @param secretaryId 登录教学秘书的ID
     * @note 提供创建课程、分配教师、设置上课时间功能选项，暂不绑定业务逻辑
     */
    void showSecretaryMenu(std::string_view secretaryId);

private:
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

/**
* @brief 清除输入缓冲区，避免无效输入导致的交互异常
*/
void UserInterface::clearInputBuffer() const {
    std::cin.clear();                  // 清除错误状态标志
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 忽略缓冲区所有字符
}

/**
* @brief 显示角色选择菜单，返回用户选择的角色类型
* @return 1=学生，2=教师，3=教学秘书
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
* @brief 显示登录菜单，处理用户登录流程
* @return 登录成功的用户ID（非空）
* @note 仅做模拟登录：ID非空即通过，密码不校验，支持角色选择
*/
std::string UserInterface::showLoginMenu() {
    std::string user_id;
    std::string password;

    // 优化：减少空行数量，缩小页面空白
    std::print("\n\n\n");
    std::print("===============================================\n");
    std::print("========== 重庆师范大学选课系统 v1.0 ==========\n");
    std::print("===============================================\n");

    // 输入用户ID（非空校验）
    while (true) {
        std::print("请输入用户ID：");
        std::getline(std::cin, user_id);

        if (user_id.empty()) {
            std::print("❌ 用户ID不能为空！请重新输入。\n");
            continue;
        }
        break;
    }

    // 输入密码（暂不校验，仅做显示）
    // std::print("请输入密码：");
    // std::getline(std::cin, password);
    std::print("✅ 登录验证通过！\n");

    // 角色选择与菜单跳转
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
* @brief 显示学生主菜单（循环交互）
* @param studentId 登录学生的ID
* @note 提供选课、退课、查课表、查成绩功能选项，暂不绑定业务逻辑
*/
void UserInterface::showStudentMenu(std::string_view studentId) {
    int choice = 0;
    while (true) {
        // 优化：减少空行数量，缩小页面空白
        std::print("\n\n\n");
        std::print("===============================================\n");
        std::print("========== 学生菜单 - 用户名：{} ==========\n", studentId);
        std::print("===============================================\n");
        // 调整：中文在前、英文在后
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

        // 功能选中提示（暂不绑定业务逻辑）
        switch (choice) {
            case 1:
                std::print("\n✅ 已选择功能 [1]：选课 (Enroll in Course)\n");
                break;
            case 2:
                std::print("\n✅ 已选择功能 [2]：退课 (Drop Course)\n");
                break;
            case 3:
                std::print("\n✅ 已选择功能 [3]：查看课表 (View Schedule)\n");
                break;
            case 4:
                std::print("\n✅ 已选择功能 [4]：查看成绩 (View Grades)\n");
                break;
            case 5:
                std::print("\n✅ 退出登录成功！返回登录界面。\n");
                return;
        }

        // 停留提示，增强交互体验
        std::print("按Enter键继续...");
        std::cin.get();
    }
}

/**
* @brief 显示教师主菜单（循环交互）
* @param teacherId 登录教师的ID
* @note 提供查看授课名单、录入成绩、修改成绩功能选项，暂不绑定业务逻辑
*/
void UserInterface::showTeacherMenu(std::string_view teacherId) {
    int choice = 0;
    while (true) {
        // 优化：减少空行数量，缩小页面空白
        std::print("\n\n\n");
        std::print("===============================================\n");
        std::print("========== 教师菜单 - 用户名：{} ==========\n", teacherId);
        std::print("===============================================\n");
        // 调整：中文在前、英文在后
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

        // 功能选中提示（暂不绑定业务逻辑）
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

        // 停留提示
        std::print("按Enter键继续...");
        std::cin.get();
    }
}

/**
* @brief 显示教学秘书主菜单（循环交互）
* @param secretaryId 登录教学秘书的ID
* @note 提供创建课程、分配教师、设置上课时间功能选项，暂不绑定业务逻辑
*/
void UserInterface::showSecretaryMenu(std::string_view secretaryId) {
    int choice = 0;
    while (true) {
        // 优化：减少空行数量，缩小页面空白
        std::print("\n\n\n");
        std::print("===============================================\n");
        std::print("========== 教学秘书菜单 - 用户名：{} ==========\n", secretaryId);
        std::print("===============================================\n");
        // 调整：中文在前、英文在后
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

        // 功能选中提示（暂不绑定业务逻辑）
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

        // 停留提示
        std::print("按Enter键继续...");
        std::cin.get();
    }
}
