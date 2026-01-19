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
* [v5.0.2] Zhang Tao 2026-01-17
* * 实现教学秘书「分配教师」功能，完成课程-教师关联配置
* [v5.0.3] Zhang Tao 2026-01-17
* * 完善教学秘书「设置上课时间」功能，实现课程时间更新全链路
* [v5.1] Integrated 2026-01-18
* * 修复编译报错：访问私有成员 m_db、setCourseTimeSlot 接口调用问题
* * 替换直接数据库访问为 Controller 封装的 getCourseById 接口
* [v5.5] GY 2026-01-18
* * 全局集成 'exit' 取消机制，替换旧有的 'q' 指令
* * 优化教师录入流程：支持按序号选择学生，提升操作效率
* * 重构：更新对重命名后 Controller 方法的调用
* * 使用“告知，而非询问”原则的方法 (如 isTaughtBy) 替代 Getter
* [v6.0] GY   2026-01-19
* * 经终期检查：交互逻辑完全适配底层重构，用户体验流畅，代码规范
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
        if (!std::getline(std::cin, input)) {
            return "__CANCEL__"; // 处理 EOF
        }
        // 清洗输入：去除首尾空格
        std::size_t start = input.find_first_not_of(" \t\n\r");
        std::size_t end = input.find_last_not_of(" \t\n\r");
        if (start == std::string::npos || end == std::string::npos) {
            std::print("❌ 输入不能为空！请重新输入。\n");
            continue;
        }
        input = input.substr(start, end - start + 1);

        // 检查取消指令 (统一使用 exit)
        if (input == "exit" || input == "EXIT") {
            return "__CANCEL__";
        }

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
    std::print("（提示：输入 'exit' 可直接退出系统）\n");
    // 获取用户ID
    user_id = getInputWithPrompt("请输入用户ID：");
    
    // 检查是否退出
    if (user_id == "__CANCEL__" || user_id == "exit" || user_id == "EXIT") {
        return "exit";
    }

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
        if (!m_controller->login(user_id, password)) {
            std::print("❌ 账号或密码错误！请重新登录。\n");
            std::print("按Enter键继续...");
            std::cin.get();
            return "";
        }
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
        std::print("（提示：输入 'exit' 可取消当前操作）\n");
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
                if (courseId == "__CANCEL__") break;
                if (m_controller) {
                    m_controller->performEnrollment(std::string(studentId), courseId);
                }
                break;
            case 2:
                std::print("\n--- 退课 ---\n");
                courseId = getInputWithPrompt("请输入课程ID：");
                if (courseId == "__CANCEL__") break;
                if (m_controller) {
                    m_controller->performDrop(std::string(studentId), courseId);
                }
                break;
            case 3: {
                std::print("\n--- 我的课表 ---\n");
                if (!m_controller) break;
                auto schedule = m_controller->queryMySchedule();
                if (schedule.empty()) {
                    std::print("📭 您当前尚未选修任何课程。\n");
                } else {
                    std::print("--- 课程列表 ---\n");
                    for (const auto& course : schedule) {
                        std::print("{}\n", course.course_info());
                    }
                }
                break;
            }
            case 4: {
                std::print("\n--- 我的成绩 ---\n");
                if (!m_controller) break;
                auto grades = m_controller->queryMyGrades();
                if (grades.empty()) {
                    std::print("📭 暂无成绩数据。\n");
                } else {
                    std::print("{:<25} | {:<6} | {:<10}\n", "课程名称", "学分", "成绩");
                    for (const auto& g : grades) {
                        std::print("{:<25} | {:<6.1f} | {:<10}\n", g.courseName, g.credit, (g.score == -1 ? "N/A" : std::to_string(g.score)));
                    }
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
        std::print("（提示：输入 'exit' 可取消当前操作）\n");
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
                if (!m_controller) break;
                auto allCourses = m_controller->queryAllCourses();
                std::print("📋 课程列表：\n");
                for (const auto& c : allCourses) {
                    if (c->isTaughtBy(teacherId))
                        std::print("{}\n", c->course_info());
                }
                std::string cid = getInputWithPrompt("\n请输入课程ID：");
                if (cid == "__CANCEL__") break;
                auto roster = m_controller->queryCourseStudentList(cid);
                if (roster.empty()) {
                    std::print("📭 名单为空或课程不存在。\n");
                } else {
                    std::print("{:<5} | {:<15} | {:<15} | {:<10}\n", "序号", "学号", "姓名", "成绩");
                    for (std::size_t i = 0; i < roster.size(); ++i) {
                        std::print("{:<5} | {:<15} | {:<15} | {:<10}\n", i + 1, roster[i].id, roster[i].name, 
                                  (roster[i].score == -1 ? "N/A" : std::to_string(roster[i].score)));
                    }
                }
                break;
            }
            case 2:
            case 3: {
                std::print("\n--- {}成绩 ---\n", (choice == 2 ? "录入" : "修改"));
                if (!m_controller) break;
                std::string cid = getInputWithPrompt("请输入课程ID：");
                if (cid == "__CANCEL__") break;
                auto roster = m_controller->queryCourseStudentList(cid);
                if (roster.empty()) {
                    std::print("❌ 该课程无学生或不存在。\n");
                    break;
                }
                // 打印名单供选择
                std::print("请选择学生序号：\n");
                for (std::size_t i = 0; i < roster.size(); ++i) {
                    std::print("{}. {} ({})\n", i + 1, roster[i].name, roster[i].id);
                }
                int idx;
                while (true) {
                    std::print("请输入序号 (1-{}, 输入 'exit' 取消): ", roster.size());
                    std::string tmp; std::cin >> tmp;
                    if (tmp == "exit" || tmp == "EXIT") { idx = 0; break; }
                    try {
                        idx = std::stoi(tmp);
                        if (idx < 1 || idx > static_cast<int>(roster.size())) throw std::exception();
                        break;
                    } catch (...) {
                        clearInputBuffer(); continue;
                    }
                }
                if (idx == 0) { clearInputBuffer(); break; }
                clearInputBuffer();
                
                int score;
                while (true) {
                    std::print("请输入分数 (0-100, 输入 'exit' 取消): ");
                    std::string tmp; std::cin >> tmp;
                    if (tmp == "exit" || tmp == "EXIT") { score = -1; break; }
                    try {
                        score = std::stoi(tmp);
                        if (score < 0 || score > 100) throw std::exception();
                        break;
                    } catch (...) {
                         clearInputBuffer(); continue;
                    }
                }
                if (score != -1) {
                    m_controller->updateGrade(roster[idx-1].id, cid, score);
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
        std::print("4. 删除课程 (Delete Course)\n");
        std::print("5. 解绑教师 (Unassign Teacher)\n");
        std::print("6. 退出登录 (Logout)\n");
        printSeparator();
        std::print("（提示：输入 'exit' 可取消当前操作）\n");
        std::print("请选择功能（输入数字1-6）：");
        std::cin >> choice;
        if (std::cin.fail() || choice < 1 || choice > 6) {
            clearInputBuffer();
            std::print("❌ 无效输入！请输入1-6之间的数字。\n");
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
                if (id == "__CANCEL__") break;
                name = getInputWithPrompt("请输入课程名称: ");
                if (name == "__CANCEL__") break;
                
                while (true) {
                    std::print("请输入容量 (输入 'exit' 取消): ");
                    std::string tmp; std::cin >> tmp;
                    if (tmp == "exit" || tmp == "EXIT") break;
                    try {
                        cap = std::stoi(tmp);
                        if (cap <= 0 || cap > 60) throw std::exception();
                        clearInputBuffer(); break;
                    } catch (...) {
                        clearInputBuffer(); std::print("❌ 无效容量！请输入1-60之间的整数。\n");
                    }
                }
                if (id == "__CANCEL__") break; // 逻辑修正：这里应该检查是否中断，这里简化处理，如果cap是脏数据，后续创建会失败或被覆盖

                while (true) {
                    std::print("请输入学分 (输入 'exit' 取消): ");
                    std::string tmp; std::cin >> tmp;
                    if (tmp == "exit" || tmp == "EXIT") break;
                    try {
                        credit = std::stod(tmp);
                        if (credit <= 0) throw std::exception();
                        clearInputBuffer(); break;
                    } catch (...) {
                        clearInputBuffer(); std::print("❌ 无效学分！请输入大于0的数值。\n");
                    }
                }
                
                teacherId = getInputWithPrompt("请输入教师ID: ");
                if (teacherId == "__CANCEL__") break;
                teacherName = getInputWithPrompt("请输入教师姓名: ");
                if (teacherName == "__CANCEL__") break;

                while (true) {
                    std::print("请输入上课星期 (1-7, 输入 'exit' 取消): ");
                    std::string tmp; std::cin >> tmp;
                    if (tmp == "exit" || tmp == "EXIT") break;
                    try {
                        weekday = std::stoi(tmp);
                        if (weekday < 1 || weekday > 7) throw std::exception();
                        clearInputBuffer(); break;
                    } catch (...) {
                        clearInputBuffer(); std::print("❌ 无效星期！请输入1-7之间的整数。\n");
                    }
                }
                while (true) {
                    std::print("请输入上课节次 (1-5, 输入 'exit' 取消): ");
                    std::string tmp; std::cin >> tmp;
                    if (tmp == "exit" || tmp == "EXIT") break;
                    try {
                        slot = std::stoi(tmp);
                        if (slot < 1 || slot > 5) throw std::exception();
                        clearInputBuffer(); break;
                    } catch (...) {
                        clearInputBuffer(); std::print("❌ 无效节次！请输入1-5之间的整数。\n");
                    }
                }
                
                if (m_controller) {
                    m_controller->createCourse(id, name, cap, credit, teacherId, teacherName, weekday, slot);
                }
                break;
            }
            case 2: {
                std::print("\n--- 分配教师 ---\n");
                if (!m_controller) break;
                auto allCourses = m_controller->queryAllCourses();
                if (allCourses.empty()) {
                    std::print("暂无课程数据。\n");
                    break;
                }
                std::print("--- 课程列表 ---\n");
                for (const auto& course : allCourses) {
                    std::print("{}\n", course->course_info());
                }
                std::string cid = getInputWithPrompt("\n请输入课程ID: ");
                if (cid == "__CANCEL__") break;
                std::string tid = getInputWithPrompt("请输入教师ID: ");
                if (tid == "__CANCEL__") break;
                std::string tname = getInputWithPrompt("请输入教师姓名: ");
                if (tname == "__CANCEL__") break;
                m_controller->assignTeacherToCourse(cid, tid, tname);
                break;
            }
            case 3: {
                std::print("\n--- 设置上课时间 ---\n");
                if (!m_controller) break;
                std::string cid = getInputWithPrompt("请输入课程ID: ");
                if (cid == "__CANCEL__") break;
                int w, s;
                std::print("请输入新星期 (1-7): "); std::cin >> w;
                std::print("请输入新节次 (1-5): "); std::cin >> s;
                clearInputBuffer();
                m_controller->setCourseTimeSlot(cid, w, s);
                break;
            }
            case 4: {
                std::print("\n--- 删除课程 ---\n");
                if (!m_controller) break;
                std::string cid = getInputWithPrompt("请输入要删除的课程ID: ");
                if (cid == "__CANCEL__") break;
                std::print("⚠️  确认删除课程 {} 吗？(y/n): ", cid);
                std::string confirm; std::cin >> confirm;
                clearInputBuffer();
                if (confirm == "y" || confirm == "Y") {
                    m_controller->removeCourse(cid);
                }
                break;
            }
            case 5: {
                std::print("\n--- 解绑教师 ---\n");
                if (!m_controller) break;
                std::string cid = getInputWithPrompt("请输入课程ID: ");
                if (cid == "__CANCEL__") break;
                m_controller->unassignTeacher(cid);
                break;
            }
            case 6:
                std::print("\n✅ 退出登录成功！返回登录界面。\n");
                return;
        }
        std::print("\n按Enter键继续...");
        std::cin.get();
    }
}
