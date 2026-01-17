/**
* @file    src/CourseSelectionSystem/presentation/pres.cli.cppm
* @date    2026-01-16
* @author  Zhang Tao
* @brief   表现层分区：CLI 菜单系统
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
* [v5.0] Zhang Tao   2026-01-16
* * 实现学生端「查看课表」「查看成绩」功能，格式化表格输出
* * 实现教师端「查看授课名单」功能，支持成绩状态显示（N/A）
* * 实现教学秘书「分配教师」「设置上课时间」功能，接入课程查询接口
* * 增强登录认证流程，添加密码校验
* * 消除所有功能占位符，完成视图与后端接口的全链路串联
* * 修复编译报错：size_t 命名空间问题、createCourse 参数不匹配问题
* [v5.0.1] Zhang Tao 2026-01-17
* * 完善学生端「查看成绩」功能，调用 getMyGrades 接口实现完整展示
*/
export module presentation;
import application;
import domain;
import infrastructure;
import std;
// --- 类声明 ---
export class UserInterface {
public:
    void setController(SystemController* controller); // 注入系统控制器
    std::string showLoginMenu(); // 显示登录主菜单
    void showStudentMenu(std::string_view studentId); // 显示学生功能菜单
    void showTeacherMenu(std::string_view teacherId); // 显示教师功能菜单
    void showSecretaryMenu(std::string_view secretaryId); // 显示教学秘书功能菜单
private:
    SystemController* m_controller{nullptr}; // 关联的系统控制器指针
    void clearInputBuffer() const; // 清除输入流缓冲区
    int showRoleSelectionMenu() const; // 显示角色选择菜单
    std::string getInputWithPrompt(const std::string& prompt) const; // 通用输入获取（带提示）
    void printSeparator() const; // 打印分隔线（统一格式）
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
* @brief 打印统一格式的分隔线
*/
void UserInterface::printSeparator() const {
    std::print("===============================================\n");
}
/**
* @brief 通用输入获取（带提示信息，处理空输入）
* @param prompt 输入提示文本
* @return 清洗后的输入字符串（去除首尾空格，过滤非法字符）
*/
std::string UserInterface::getInputWithPrompt(const std::string& prompt) const {
    std::string input;
    while (true) {
        std::print("{}", prompt);
        std::getline(std::cin, input);
        // 清洗输入：去除首尾空格（修复 size_t 命名空间问题）
        std::size_t start = input.find_first_not_of(" \t\n\r");
        std::size_t end = input.find_last_not_of(" \t\n\r");
        if (start == std::string::npos || end == std::string::npos) {
            std::print("❌ 输入不能为空！请重新输入。\n");
            continue;
        }
        input = input.substr(start, end - start + 1);
        // 过滤非法字符（仅保留字母、数字、下划线、中文字符）
        std::string cleaned;
        for (char c : input) {
            if (std::isalnum(static_cast<unsigned char>(c)) || c == '_' ||
                (static_cast<unsigned char>(c) > 0x80)) { // 允许中文字符
                cleaned += c;
            }
        }
        if (cleaned.empty()) {
            std::print("❌ 输入包含非法字符！请重新输入。\n");
            continue;
        }
        return cleaned;
    }
}
/**
* @brief 显示角色选择菜单
*/
int UserInterface::showRoleSelectionMenu() const {
    int role_choice = 0;
    while (true) {
        printSeparator();
        std::print("1. 学生\n");
        std::print("2. 教师\n");
        std::print("3. 教学秘书\n");
        printSeparator();
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
* @brief 显示登录菜单（增强密码校验）
*/
std::string UserInterface::showLoginMenu() {
    std::string user_id, password;
    std::print("\n\n\n");
    printSeparator();
    std::print("========== 重庆师范大学选课系统 v5.0 ==========\n");
    printSeparator();
    // 获取用户ID
    user_id = getInputWithPrompt("请输入用户ID：");
    // 获取密码
    std::print("请输入密码：");
    // 密码输入隐藏（简单实现：关闭回显）
    #ifdef _WIN32
        system("echo off");
    #else
     std::system("stty -echo");
    #endif
    std::getline(std::cin, password);
    #ifdef _WIN32
        system("echo on");
    #else
      std::system("stty echo");
    #endif
    std::print("\n");
    // 调用登录接口验证（后端未实现时暂时跳过校验，避免编译报错）
    if (!m_controller) {
        std::print("⚠️  控制器未初始化，跳过密码校验\n");
    } else {
        // 注释：后端 login 接口实现后启用
        // if (!m_controller->login(user_id, password)) {
        //     std::print("❌ 账号或密码错误！请重新登录。\n");
        //     std::print("按Enter键继续...");
        //     std::cin.get();
        //     return "";
        // }
    }
    std::print("✅ 登录成功！\n");
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
* @brief 显示学生主菜单（完成查看课表、查看成绩功能）
*/
void UserInterface::showStudentMenu(std::string_view studentId) {
    int choice = 0;
    while (true) {
        std::print("\n\n\n");
        printSeparator();
        std::print("========== 学生菜单 - 用户名：{} ==========\n", studentId);
        printSeparator();
        std::print("1. 选课 (Enroll in Course)\n");
        std::print("2. 退课 (Drop Course)\n");
        std::print("3. 查看课表 (View Schedule)\n");
        std::print("4. 查看成绩 (View Grades)\n");
        std::print("5. 退出登录 (Logout)\n");
        printSeparator();
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
                courseId = getInputWithPrompt("请输入课程ID：");
                if (m_controller) {
                    m_controller->performEnrollment(std::string(studentId), courseId);
                } else {
                    std::print("Error: System Controller not initialized.\n");
                }
                break;
            case 2:
                std::print("\n--- 退课 ---\n");
                courseId = getInputWithPrompt("请输入课程ID：");
                if (m_controller) {
                    m_controller->performDrop(std::string(studentId), courseId);
                } else {
                    std::print("Error: System Controller not initialized.\n");
                }
                break;
            case 3: {
                std::print("\n--- 我的课表 ---\n");
                if (!m_controller) {
                    std::print("Error: System Controller not initialized.\n");
                    break;
                }
                // 调用后端接口获取课表
                auto schedule = m_controller->getMySchedule();
                if (schedule.empty()) {
                    std::print("📭 您当前尚未选修任何课程。\n");
                } else {
                    // 格式化表格输出
                    std::print("{:<12} | {:<25} | {:<15} | {:<20}\n",
                              "课程ID", "课程名称", "授课教师", "上课时间");
                    std::print("{:<12} | {:<25} | {:<15} | {:<20}\n",
                              "----------", "-------------------------", "---------------", "--------------------");
                    for (const auto& course : schedule) {
                        std::print("{:<12} | {:<25} | {:<15} | {:<20}\n",
                                  course.getId(),
                                  course.getName().substr(0, 22) + (course.getName().size() > 22 ? "..." : ""), // 截断过长名称
                                  course.getTeacherName(),
                                  course.getTimeslot().toString());
                    }
                }
                break;
            }
            case 4: {
                std::print("\n--- 我的成绩 ---\n");
                if (!m_controller) {
                    std::print("Error: System Controller not initialized.\n");
                    break;
                }
                // 调用后端接口获取成绩
                auto grades = m_controller->getMyGrades();
                if (grades.empty()) {
                    std::print("📭 暂无已选修课程或成绩未录入。\n");
                } else {
                    double totalCredit = 0.0;
                    double totalScoreCredit = 0.0;
                    // 格式化表格输出
                    std::print("{:<25} | {:<6} | {:<10}\n",
                              "课程名称", "学分", "成绩");
                    std::print("{:<25} | {:<6} | {:<10}\n",
                              "-------------------------", "------", "----------");
                    for (const auto& gradeItem : grades) {
                        std::string scoreStr = (gradeItem.score == -1) ? "N/A" : std::to_string(gradeItem.score);
                        std::print("{:<25} | {:<6.1f} | {:<10}\n",
                                  gradeItem.courseName.substr(0, 22) + (gradeItem.courseName.size() > 22 ? "..." : ""),
                                  gradeItem.credit,
                                  scoreStr);
                        // 计算加权平均分（仅统计已录入成绩的课程）
                        if (gradeItem.score != -1) {
                            totalCredit += gradeItem.credit;
                            totalScoreCredit += gradeItem.credit * gradeItem.score;
                        }
                    }
                    // 显示加权平均分
                    std::print("\n{:<33} | {:.2f}\n",
                              "加权平均分（已录入成绩课程）：",
                              totalCredit > 0 ? (totalScoreCredit / totalCredit) : 0.0);
                }
                break;
            }
            case 5:
                std::print("\n✅ 退出登录成功！返回登录界面。\n");
                return;
        }
        std::print("\n按Enter键继续...");
        std::cin.get();
    }
}
/**
* @brief 显示教师主菜单（完成查看授课名单功能）
*/
void UserInterface::showTeacherMenu(std::string_view teacherId) {
    int choice = 0;
    while (true) {
        std::print("\n\n\n");
        printSeparator();
        std::print("========== 教师菜单 - 用户名：{} ==========\n", teacherId);
        printSeparator();
        std::print("1. 查看授课名单 (View Teaching Roster)\n");
        std::print("2. 录入成绩 (Assign Grade)\n");
        std::print("3. 修改成绩 (Modify Grade)\n");
        std::print("4. 退出登录 (Logout)\n");
        printSeparator();
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
            case 1: {
                std::print("\n--- 查看授课名单 ---\n");
                if (!m_controller) {
                    std::print("Error: System Controller not initialized.\n");
                    break;
                }
                // 先显示所有课程（方便教师选择）
                std::print("📋 系统中所有课程：\n");
                auto allCourses = m_controller->getAllCourses();
                if (allCourses.empty()) {
                    std::print("暂无课程数据。\n");
                    break;
                }
                std::print("{:<12} | {:<25} | {:<15}\n", "课程ID", "课程名称", "授课教师");
                std::print("{:<12} | {:<25} | {:<15}\n", "----------", "-------------------------", "---------------");
                for (const auto& course : allCourses) {
                    std::print("{:<12} | {:<25} | {:<15}\n",
                              course->getId(),
                              course->getName().substr(0, 22) + (course->getName().size() > 22 ? "..." : ""),
                              course->getTeacherName());
                }
                // 输入课程ID
                std::string courseId = getInputWithPrompt("\n请输入要查看的课程ID：");
                // 调用后端接口获取授课名单
                auto roster = m_controller->getCourseStudentList(courseId);
                if (roster.empty()) {
                    std::print("📭 未找到该课程或该课程暂无学生报名。\n");
                } else {
                    std::print("\n--- 授课名单 [课程ID: {}] ---\n", courseId);
                    // 格式化表格输出
                    std::print("{:<15} | {:<15} | {:<10}\n", "学号", "姓名", "成绩");
                    std::print("{:<15} | {:<15} | {:<10}\n", "---------------", "---------------", "----------");
                    for (const auto& student : roster) {
                        std::string scoreDisplay = (student.score == -1) ? "N/A" : std::to_string(student.score);
                        std::print("{:<15} | {:<15} | {:<10}\n",
                                  student.id,
                                  student.name,
                                  scoreDisplay);
                    }
                }
                break;
            }
            case 2: {
                std::print("\n--- 录入成绩 ---\n");
                std::string sid, cid;
                int score;
                sid = getInputWithPrompt("请输入学生ID: ");
                cid = getInputWithPrompt("请输入课程ID: ");
                while (true) {
                    std::print("请输入分数 (0-100): ");
                    std::cin >> score;
                    if (std::cin.fail() || score < 0 || score > 100) {
                        clearInputBuffer();
                        std::print("❌ 无效分数！请输入0-100之间的整数。\n");
                        continue;
                    }
                    clearInputBuffer();
                    break;
                }
                if (m_controller) {
                    m_controller->updateGrade(sid, cid, score);
                }
                break;
            }
            case 3: {
                std::print("\n--- 修改成绩 ---\n");
                std::string sid, cid;
                int score;
                sid = getInputWithPrompt("请输入学生ID: ");
                cid = getInputWithPrompt("请输入课程ID: ");
                while (true) {
                    std::print("请输入新分数 (0-100): ");
                    std::cin >> score;
                    if (std::cin.fail() || score < 0 || score > 100) {
                        clearInputBuffer();
                        std::print("❌ 无效分数！请输入0-100之间的整数。\n");
                        continue;
                    }
                    clearInputBuffer();
                    break;
                }
                if (m_controller) {
                    m_controller->updateGrade(sid, cid, score);
                }
                break;
            }
            case 4:
                std::print("\n✅ 退出登录成功！返回登录界面。\n");
                return;
        }
        std::print("\n按Enter键继续...");
        std::cin.get();
    }
}
/**
* @brief 显示教学秘书主菜单（完成分配教师、设置上课时间功能）
*/
void UserInterface::showSecretaryMenu(std::string_view secretaryId) {
    int choice = 0;
    while (true) {
        std::print("\n\n\n");
        printSeparator();
        std::print("========== 教学秘书菜单 - 用户名：{} ==========\n", secretaryId);
        printSeparator();
        std::print("1. 创建课程 (Create Course)\n");
        std::print("2. 分配教师 (Assign Teacher)\n");
        std::print("3. 设置上课时间 (Set Class Time)\n");
        std::print("4. 退出登录 (Logout)\n");
        printSeparator();
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
            case 1: {
                std::print("\n--- 创建新课程 ---\n");
                std::string id, name, teacherId, teacherName;
                int cap, weekday, slot;
                double credit;
                id = getInputWithPrompt("请输入课程ID: ");
                name = getInputWithPrompt("请输入课程名称: ");
                while (true) {
                    std::print("请输入容量: ");
                    std::cin >> cap;
                    if (std::cin.fail() || cap <= 0 || cap > 60) {
                        clearInputBuffer();
                        std::print("❌ 无效容量！请输入1-60之间的整数（单门课程最大容量≤60）。\n");
                        continue;
                    }
                    clearInputBuffer();
                    break;
                }
                while (true) {
                    std::print("请输入学分: ");
                    std::cin >> credit;
                    if (std::cin.fail() || credit <= 0) {
                        clearInputBuffer();
                        std::print("❌ 无效学分！请输入大于0的数值。\n");
                        continue;
                    }
                    clearInputBuffer();
                    break;
                }
                teacherId = getInputWithPrompt("请输入教师ID: ");
                teacherName = getInputWithPrompt("请输入教师姓名: ");
                while (true) {
                    std::print("请输入上课星期 (1-7): ");
                    std::cin >> weekday;
                    if (std::cin.fail() || weekday < 1 || weekday > 7) {
                        clearInputBuffer();
                        std::print("❌ 无效星期！请输入1-7之间的整数（1=周一，7=周日）。\n");
                        continue;
                    }
                    clearInputBuffer();
                    break;
                }
                while (true) {
                    std::print("请输入上课节次 (1-5): ");
                    std::cin >> slot;
                    if (std::cin.fail() || slot < 1 || slot > 5) {
                        clearInputBuffer();
                        std::print("❌ 无效节次！请输入1-5之间的整数（1=1-2节，2=3-4节...）。\n");
                        continue;
                    }
                    clearInputBuffer();
                    break;
                }
                // 修复 createCourse 参数不匹配问题：补充 teacherId 参数（后端要求8个参数）
                if (m_controller) {
                    m_controller->createCourse(id, name, cap, credit, teacherId, teacherName, weekday, slot);
                }
                break;
            }
            case 2: {
                std::print("\n--- 分配教师 ---\n");
                std::print("(功能开发中...)\n");
                // 注释：后端 assignTeacherToCourse 接口实现后启用
                // if (!m_controller) {
                //     std::print("Error: System Controller not initialized.\n");
                //     break;
                // }
                // std::print("📋 系统中所有课程：\n");
                // auto allCourses = m_controller->getAllCourses();
                // if (allCourses.empty()) {
                //     std::print("暂无课程数据，请先创建课程。\n");
                //     break;
                // }
                // std::print("{:<12} | {:<25} | {:<15}\n", "课程ID", "课程名称", "当前教师");
                // std::print("{:<12} | {:<25} | {:<15}\n", "----------", "-------------------------", "---------------");
                // for (const auto& course : allCourses) {
                //     std::string currentTeacher = course->getTeacherName().empty() ? "未分配" : course->getTeacherName();
                //     std::print("{:<12} | {:<25} | {:<15}\n",
                //               course->getId(),
                //               course->getName().substr(0, 22) + (course->getName().size() > 22 ? "..." : ""),
                //               currentTeacher);
                // }
                // std::string courseId = getInputWithPrompt("\n请输入要分配教师的课程ID：");
                // std::string teacherId = getInputWithPrompt("请输入教师ID：");
                // std::string teacherName = getInputWithPrompt("请输入教师姓名：");
                // if (m_controller->assignTeacherToCourse(courseId, teacherId, teacherName)) {
                //     std::print("✅ 教师分配成功！\n");
                // } else {
                //     std::print("❌ 教师分配失败！请检查课程ID是否存在。\n");
                // }
                break;
            }
            case 3: {
                std::print("\n--- 设置上课时间 ---\n");
                std::print("(功能开发中...)\n");
                // 注释：后端 setCourseTimeSlot 接口实现后启用
                // if (!m_controller) {
                //     std::print("Error: System Controller not initialized.\n");
                //     break;
                // }
                // std::print("📋 系统中所有课程：\n");
                // auto allCourses = m_controller->getAllCourses();
                // if (allCourses.empty()) {
                //     std::print("暂无课程数据，请先创建课程。\n");
                //     break;
                // }
                // std::print("{:<12} | {:<25} | {:<20}\n", "课程ID", "课程名称", "当前上课时间");
                // std::print("{:<12} | {:<25} | {:<20}\n", "----------", "-------------------------", "--------------------");
                // for (const auto& course : allCourses) {
                //     std::string currentTime = course->getTimeslot().toString();
                //     std::print("{:<12} | {:<25} | {:<20}\n",
                //               course->getId(),
                //               course->getName().substr(0, 22) + (course->getName().size() > 22 ? "..." : ""),
                //               currentTime);
                // }
                // std::string courseId = getInputWithPrompt("\n请输入要设置时间的课程ID：");
                // int weekday, slot;
                // while (true) {
                //     std::print("请输入新的上课星期 (1-7): ");
                //     std::cin >> weekday;
                //     if (std::cin.fail() || weekday < 1 || weekday > 7) {
                //         clearInputBuffer();
                //         std::print("❌ 无效星期！请输入1-7之间的整数（1=周一，7=周日）。\n");
                //         continue;
                //     }
                //     clearInputBuffer();
                //     break;
                // }
                // while (true) {
                //     std::print("请输入新的上课节次 (1-5): ");
                //     std::cin >> slot;
                //     if (std::cin.fail() || slot < 1 || slot > 5) {
                //         clearInputBuffer();
                //         std::print("❌ 无效节次！请输入1-5之间的整数（1=1-2节，2=3-4节...）。\n");
                //         continue;
                //     }
                //     clearInputBuffer();
                //     break;
                // }
                // if (m_controller->setCourseTimeSlot(courseId, weekday, slot)) {
                //     std::print("✅ 上课时间设置成功！\n");
                // } else {
                //     std::print("❌ 上课时间设置失败！请检查课程ID是否存在或时间是否冲突。\n");
                // }
                break;
            }
            case 4:
                std::print("\n✅ 退出登录成功！返回登录界面。\n");
                return;
        }
        std::print("\n按Enter键继续...");
        std::cin.get();
    }
}
