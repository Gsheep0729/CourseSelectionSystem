/**
* @file    src/CourseSelectionSystem/application/app.controller.cppm
* @date    2026-01-07
* @author  GY
* @brief   应用层分区：系统控制器
*
* 负责管理整个选课系统的运行流程
* 维护学生和课程列表，提供选课、退课等核心业务功能
* 协调领域层实体之间的交互
*
* Change Log:
* [v1.0] GY   2026-01-06
* * 初始化项目架构 (基于 C++23 Modules)
* [v1.5] GY   2026-01-07
* * 重构 SystemController，移除内存存储，接入 DBAdapter
* [v2.0] GY   2026-01-07
* * 完成 SystemController 与 DBAdapter 的深度集成
* * 移除内存容器 (std::vector)，全量迁移至 SQL 事务操作
* * 实现基于数据库的实时选课/退课业务逻辑（含并发安全/容量检查）
* [v3.0] GY   2026-01-10
    本次提交完成了核心架构的重构，解决了 Controller 直接依赖数据库的违规问题，并实现了关键业务逻辑。
    [架构重构]
    - Infrastructure: 新增 StudentProxy, CourseProxy, EnrollmentProxy 分区模块。
    - Refactor: 将 SystemController 中的 SQL 语句全部迁移至代理层，初步实现 4 层架构的关注点分离。
    [领域逻辑]
    - Domain: 新增 Timeslot 模块，并在 Student 实体中实现 hasTimeConflict 冲突检测算法。
    - Entity: 扩展 Course 实体，支持学分、教师、时间槽等完整字段。
    [业务功能]
    - Secretary: 完成“创建课程”功能的后端链路与 CLI 对接。
    - Teacher: 完成“录入成绩”功能的数据库交互。
    [构建系统]
    - CMake: 更新配置以支持新增的 Infrastructure 分区文件。
* [v4.5] GY   2026-01-16
* * 实现 login 方法与基于 users 表的身份验证
* * 引入 m_currentUser 维护登录会话
* * 在选课、退课、评分等业务逻辑中集成细粒度权限校验
* * 新增 getMySchedule, getCourseStudentList 等查询桥接接口供 UI 调用
* * [Fix] 修复 createCourse 自动创建教师账户逻辑
* [v5.0] Zhang Tao 2026-01-17
* * 新增 getMyGrades 接口，支持学生查询个人成绩
* [v5.0.2] Zhang Tao 2026-01-17
* * 新增 assignTeacherToCourse 接口，支持教学秘书为课程分配教师
* [v5.1] GY 2026-01-18
* * 新增 setCourseTimeSlot 接口，支持教学秘书修改课程时间槽
* * 新增 getCourseById 接口，封装数据库访问，避免外部直接访问私有成员 m_db
* * 补充 updateGrade 接口实现，支持教师录入/修改成绩
* [v5.5] GY 2026-01-18
* * 新增 removeCourse 和 unassignTeacher 接口，支持课程与教师的解绑管理
* * 重构业务方法以调用 Proxy 静态接口，移除 Controller 内冗余 SQL
* [v5.6] GY   2026-01-19
* * 重构：重命名查询方法 (移除 'get' 前缀)
* * 分离函数声明与实现
* [v5.7] GY   2026-01-19
* * 修复 Bug：在创建课程时增加教师时间冲突检测
* [v5.8] GY   2026-01-19
* * 测试数据升级：导入真实的课程数据列表 (42门课程)
* [v5.9] GY   2026-01-19
* * 测试数据升级：导入真实的学生名单 (178名学生)
* [v6.0] GY   2026-01-19
* * 经终期检查：全业务流程通过大规模真实数据验证，代码标准、逻辑完备
* [v6.1] GY   2026-01-29
* * 新增：支持自定义数据库连接配置，优化 initialize 接口以接受外部连接字符串
*/
export module application;
import domain;
import infrastructure;
import std;

export class SystemController {
public:
    struct User {
        std::string id;
        std::string name;
        std::string role; // "student", "teacher", "secretary"
        bool isValid() const { return !id.empty(); }
    };

    SystemController(); // 构造函数：初始化数据库适配器
<<<<<<< HEAD
    void initialize(); // 系统初始化：建立连接、创建表结构
=======
    void initialize(const std::string& conn_str = ""); // 系统初始化：建立连接、创建表结构
>>>>>>> dev
    void cleanup(); // 系统清理：删除表结构 (用于测试)
    void run(); // 启动系统运行逻辑

    // 用户认证
    bool login(std::string id, std::string password);
    User sessionUser() const { return m_currentUser; }

    // 核心业务功能
    void performEnrollment(std::string sid, std::string cid); // 执行选课业务逻辑
    void performDrop(std::string sid, std::string cid); // 执行退课业务逻辑

    // 教学秘书功能
    bool createCourse(std::string id, std::string name, int capacity, double credit,
                      std::string teacherId, std::string teacherName, int weekday, int timeslot); // 创建新课程
    bool removeCourse(std::string courseId); // 新增：删除课程
    bool assignTeacherToCourse(std::string courseId, std::string teacherId, std::string teacherName); // 为课程分配教师
    bool unassignTeacher(std::string courseId); // 新增：解绑教师
    bool setCourseTimeSlot(std::string courseId, int weekday, int timeslot); // 新增：修改课程时间槽

    // 教师功能
    bool updateGrade(std::string sid, std::string cid, int score); // 录入/修改学生成绩

    // --- 数据查询接口 (供 UI 调用) ---
    // 获取当前登录学生的课表
    std::vector<Course> queryMySchedule();

    // 获取当前登录学生的成绩
    std::vector<infra::GradeDTO> queryMyGrades();

    // 获取某门课程的学生名单 (仅教师/管理员可用)
    std::vector<infra::CourseStudentDTO> queryCourseStudentList(std::string courseId);

    // 获取所有课程列表 (公共查询)
    std::vector<std::unique_ptr<Course>> queryAllCourses();

    // 新增：根据课程ID获取课程对象 (封装数据库访问，避免外部访问m_db)
    std::unique_ptr<Course> findCourseById(std::string courseId);

private:
    std::unique_ptr<db::DBAdapter> m_db; // 数据库适配器指针
    User m_currentUser; // 当前登录用户
};

// --- Implementation ---
// @brief 构造函数
SystemController::SystemController() : m_db(std::make_unique<db::DBAdapter>()) {}

/**
 * @brief 系统环境初始化
 * 建立数据库连接，执行 DDL 语句重置表结构，并导入初始的用户及课程数据。
 */
void SystemController::initialize(const std::string& custom_conn_str) {
    // 使用 PostgreSQL的 CourseSelectionSystem数据库，登录管理员账号为postgres，密码为123，ip地址为127.0.0.1，端口号为5432
    std::string conn_str = "dbname=CourseSelectionSystem user=postgres password=123 hostaddr=127.0.0.1 port=5432";

    if (!custom_conn_str.empty()) {
        conn_str = custom_conn_str;
    }

    if (!m_db->connect(conn_str)) {
        std::print("Error: Failed to connect to database.\n");
        return;
    }

    // DDL: 重置数据库结构
    m_db->execute("DROP TABLE IF EXISTS enrollment CASCADE");
    m_db->execute("DROP TABLE IF EXISTS course CASCADE");
    m_db->execute("DROP TABLE IF EXISTS student CASCADE");
    m_db->execute("DROP TABLE IF EXISTS users CASCADE");

    // 创建用户表 (用于认证)
    m_db->execute(R"(
        CREATE TABLE users (
            user_id VARCHAR(50) PRIMARY KEY,
            name VARCHAR(50) NOT NULL,
            password VARCHAR(50) DEFAULT '123',
            role VARCHAR(20) CHECK (role IN ('student','teacher','secretary'))
        )
    )");

    m_db->execute(R"(
        CREATE TABLE course (
            id VARCHAR(50) PRIMARY KEY,
            name TEXT NOT NULL,
            capacity INT DEFAULT 120,
            enrolled INT DEFAULT 0,
            credit REAL DEFAULT 0.0,
            teacher_id TEXT,
            teacher_name TEXT,
            weekday INT,
            timeslot INT
        )
    )");

    m_db->execute("CREATE TABLE student (id VARCHAR(50) PRIMARY KEY, name TEXT)");

    m_db->execute(R"(
        CREATE TABLE enrollment (
            student_id VARCHAR(50) REFERENCES student(id),
            course_id VARCHAR(50) REFERENCES course(id),
            score INTEGER DEFAULT NULL,
            PRIMARY KEY (student_id, course_id)
        )
    )");

    std::print("Database initialized.\n");

    // --- 录入真实学生数据 ---
    struct RawStudent { std::string id; std::string name; };
    std::vector<RawStudent> students = {
        {"2024051604001", "白佳旭"}, {"2024051604002", "闫培恒"}, {"2024051604003", "易俊辰"}, {"2024051604004", "黄奕萌"},
        {"2024051604005", "明雅桐"}, {"2024051604006", "胡艺舰"}, {"2024051604007", "王耀"}, {"2024051604008", "蒋汶江"},
        {"2024051604009", "江钒"}, {"2024051604010", "周城伟"}, {"2024051604011", "黄奕萌"}, {"2024051604012", "周宇欣"},
        {"2024051604013", "陈谊"}, {"2024051604014", "何孟韩"}, {"2024051604015", "谢贤豪"}, {"2024051604016", "李成"},
        {"2024051604018", "熊俸莹"}, {"2024051604019", "周代森"}, {"2024051604020", "马浩楠"}, {"2024051604021", "陈烁心"},
        {"2024051604022", "郑鑫康"}, {"2024051604023", "何志远"}, {"2024051604024", "李超"}, {"2024051604025", "吴凝梦"},
        {"2024051604026", "崔程江"}, {"2024051604027", "陈科宇"}, {"2024051604028", "陈彤"}, {"2024051604029", "苏茜"},
        {"2024051604030", "鲁凤莹"}, {"2024051604031", "程业宸"}, {"2024051604032", "尚文哲"}, {"2024051604033", "颜世沛"},
        {"2024051604034", "陈雨欣"}, {"2024051604035", "张志豪"}, {"2024051604036", "吴永超"}, {"2024051604037", "张迅福"},
        {"2024051604038", "蔡汶宏"}, {"2024051604039", "李思思"}, {"2024051604040", "廖文杰"}, {"2024051604041", "唐晟庆"},
        {"2024051604042", "杨光"}, {"2024051604043", "熊闲伟"}, {"2024051604044", "谭晶晶"}, {"2024051604045", "张雨欣"},
        {"2024051604046", "何继雄"}, {"2024051604047", "龙玟吉"}, {"2024051604048", "段恩来"}, {"2024051604049", "冉恒益"},
        {"2024051604050", "陈月行"}, {"2024051604051", "刘城"}, {"2024051604052", "涂钧锋"}, {"2024051604053", "高歌"},
        {"2024051604054", "卓杰"}, {"2024051604055", "刘寅丞"}, {"2024051604056", "何俊霖"}, {"2024051604057", "陈龙"},
        {"2024051604058", "陈涛"}, {"2024051604059", "罗剑秋"}, {"2024051604060", "郑凯文"}, {"2024051604061", "杨宗霖"},
        {"2024051604062", "杜若贤"}, {"2024051604063", "何婷"}, {"2024051604064", "陈奕帆"}, {"2024051604065", "余浩然"},
        {"2024051604066", "梁钊睿"}, {"2024051604068", "文桔梨"}, {"2024051604069", "杨朝凯"}, {"2024051604070", "欧橼"},
        {"2024051604071", "廖伟程"}, {"2024051604072", "刘年杰"}, {"2024051604073", "杨钰玲"}, {"2024051604074", "杨红鑫"},
        {"2024051604075", "邬明亮"}, {"2024051604076", "孙海婷"}, {"2024051604077", "谭博文"}, {"2024051604078", "向治蓉"},
        {"2024051604079", "李卫"}, {"2024051604080", "邹洪荣"}, {"2024051604081", "龚清涛"}, {"2024051604082", "姚智耀"},
        {"2024051604083", "陈文静"}, {"2024051604084", "张涛"}, {"2024051604085", "高扬"}, {"2024051604087", "高菲"},
        {"2024051604088", "钟宇洋"}, {"2024051604089", "唐吴汗"}, {"2024051604090", "周姝含"}, {"2024051604091", "王子煜"},
        {"2024051604092", "王清明"}, {"2024051604093", "董镁洁"}, {"2024051604095", "白靖媛"}, {"2024051604096", "李渝平"},
        {"2024051604097", "谢明添"}, {"2024051604098", "李林"}, {"2024051604099", "庞群"}, {"2024051604100", "古航宇"},
        {"2024051604101", "黄琰尧"}, {"2024051604102", "庞文浩"}, {"2024051604103", "曹杰"}, {"2024051604104", "黄钰琳"},
        {"2024051604105", "王涛"}, {"2024051604106", "张文耀"}, {"2024051604107", "贺超"}, {"2024051604108", "何家欢"},
        {"2024051604109", "王骋"}, {"2024051604110", "朱芮娴"}, {"2024051604111", "吴林峰"}, {"2024051604112", "林健华"},
        {"2024051604113", "王璨"}, {"2024051604114", "胡君燕"}, {"2024051604115", "苟曦月"}, {"2024051604116", "郭海峰"},
        {"2024051604117", "帅沂伶"}, {"2024051604118", "苟金津"}, {"2024051604119", "冯春霖"}, {"2024051604120", "舒月"},
        {"2024051611001", "严豪毅"}, {"2024051611002", "罗缘"}, {"2024051611003", "马文杰"}, {"2024051611004", "王松涛"},
        {"2024051611005", "柯力菲"}, {"2024051611007", "柴富翔"}, {"2024051611008", "郑杰"}, {"2024051611009", "李锐"},
        {"2024051611010", "李宜美"}, {"2024051611011", "张瑞霖"}, {"2024051611013", "喻源慧"}, {"2024051611014", "李欢"},
        {"2024051611015", "皮沁灵"}, {"2024051611016", "黄俊豪"}, {"2024051611017", "喻美龄"}, {"2024051611018", "周泰安"},
        {"2024051611019", "幸家林"}, {"2024051611020", "田赟豪"}, {"2024051611021", "黄成彰"}, {"2024051611022", "罗超"},
        {"2024051611023", "王乐霏"}, {"2024051611024", "马政"}, {"2024051611025", "陈晗"}, {"2024051611026", "范绍斌"},
        {"2024051611027", "喻乐"}, {"2024051611028", "贺宇轩"}, {"2024051611029", "唐文杰"}, {"2024051611030", "樊邱悦"},
        {"2024051611031", "杨力豪"}, {"2024051611032", "唐靖淞"}, {"2024051611033", "姚译博"}, {"2024051611034", "刘倩"},
        {"2024051611035", "廖子昂"}, {"2024051611036", "李程轩"}, {"2024051611037", "李嘉奇"}, {"2024051611038", "董川涌"},
        {"2024051611039", "王新"}, {"2024051611040", "刘俊伶"}, {"2024051611041", "徐婧怡"}, {"2024051611042", "张思怡"},
        {"2024051611043", "曾祥洪"}, {"2024051611044", "杨佳昊"}, {"2024051611045", "青柏炀"}, {"2024051611046", "文玲"},
        {"2024051611047", "熊尚峰"}, {"2024051611048", "谢江"}, {"2024051611049", "陈厚禹"}, {"2024051611050", "刘书宏"},
        {"2024051611051", "陈劲宏"}, {"2024051611052", "王佳琴"}, {"2024051611053", "代杉杉"}, {"2024051611054", "张涛"},
        {"2024051611055", "谭钰寅"}, {"2024051611056", "李旨都"}, {"2024051611057", "胡波"}, {"2024051611058", "何奕颖"},
        {"2024051611059", "谢阿凤"}, {"2024051611060", "许文聪"}, {"2024051611061", "詹靖丞"}, {"2024051611062", "宋驭池"},
        {"2024051611063", "卢宗义"}, {"2024051611064", "刘娅雪"}, {"2024051611065", "刘雨彤"}, {"2024051611066", "黄馨扬"},
        {"2024051611067", "赵康德"}, {"2024051611068", "杨雪玲"}, {"2024051611069", "古伟熠"}, {"2024051611070", "李晓璐"}
    };

    for (const auto& s : students) {
        m_db->execute(std::format(
            "INSERT INTO users (user_id, name, password, role) VALUES ('{}', '{}', '123', 'student') ON CONFLICT (user_id) DO NOTHING",
            s.id, s.name
        ));
        m_db->execute(std::format(
            "INSERT INTO student (id, name) VALUES ('{}', '{}') ON CONFLICT (id) DO NOTHING",
            s.id, s.name
        ));
    }

    // 2. 录入默认教师账户
    // 教师工号: 20131672, 用户名: 龚伟 , 默认密码: 123
    m_db->execute("INSERT INTO users VALUES ('20131672', '龚伟', '123', 'teacher')");

    // 3. 录入默认教学秘书账户
    // 账号: admin, 用户名: Secretary, 默认密码: 123
    m_db->execute("INSERT INTO users VALUES ('admin', 'Secretary', '123', 'secretary')");

    // --- 导入真实课程数据 ---
    struct RawCourse {
        std::string id; std::string name; int cap; double cr;
        std::string tid; std::string tname; int w; int t;
    };
    std::vector<RawCourse> courses = {
        {"0000002564", "国家安全教育（网络视频课）", 60, 1.0, "00000000", "网络教师", 1, 1},
        {"0000006001", "军事理论（网络视频课）", 60, 2.0, "22000708", "张国清", 1, 1},
        {"0000002209", "习近平总书记关于教育的重要论述专题（网络视频课）", 60, 1.0, "20131591", "毛宇", 1, 1},
        {"003AA8002A", "大学英语I", 60, 3.0, "20247023", "赖守浪", 1, 3},
        {"003A202027", "军事技能", 60, 2.0, "20131725", "任文霞", 1, 1},
        {"083BA50005", "计算机导论", 60, 2.0, "20220045", "孙晓宁", 3, 4},
        {"083BA8013A", "C语言程序设计", 60, 4.0, "20200085", "肖颗", 1, 1},
        {"003AA00001", "形势与政策I", 60, 0.0, "20131381", "黄玲", 2, 4},
        {"003AA600AA", "思想道德与法治", 60, 2.0, "20141113", "伍崇利", 1, 4},
        {"0504321004", "线性代数", 60, 4.0, "20210031", "莫长鑫", 2, 1},
        {"073BAA0033", "高等数学(1)", 60, 5.0, "20131925", "赵侯宇", 2, 1},
        {"003AA2003A", "体育I", 60, 1.0, "20180056", "魏胜辉", 3, 2},
        {"003AA40037", "大学生心理健康教育", 60, 2.0, "20200047", "彭臻", 2, 2},
        {"083BA70004", "数据结构", 60, 3.0, "20240120", "高丽萍", 1, 1},
        {"083BA70005", "计算机系统基础", 60, 3.0, "20230071", "郭桃林", 1, 2},
        {"083BA8024A", "Linux程序设计", 60, 4.0, "20170001", "冯骥", 2, 1},
        {"083FA10006", "C语言程序设计课程设计", 60, 0.0, "20200085", "肖颗", 1, 1},
        {"083FA20051", "数据结构课程设计", 60, 1.0, "20240120", "高丽萍", 1, 1},
        {"003AA00002", "形势与政策II", 60, 0.0, "20131675", "黎朝红", 1, 4},
        {"003AA3000A", "中国近现代史纲要", 60, 2.0, "20131930", "李虹辉", 2, 2},
        {"073BAA0034", "高等数学(2)", 60, 5.0, "20170051", "许秋菊", 2, 1},
        {"003AA2004A", "体育II", 60, 1.0, "20131344", "成平", 3, 2},
        {"003AA8003A", "大学英语II", 60, 3.0, "20130891", "沈纯", 1, 3},
        {"XSC2013011", "职业生涯规划与就业指导1", 60, 1.0, "20210030", "李黎", 5, 3},
        {"083CA70013", "数据库原理及应用", 60, 3.0, "20210071", "肖旋", 1, 4},
        {"083EB80135", "软件构建与实现", 60, 4.0, "20131672", "龚伟", 1, 1},
        {"083FA20056", "软件工程综合实训1-1（C++方向）", 60, 1.0, "20131672", "龚伟", 1, 1},
        {"113BA7002A", "计算机网络", 60, 3.0, "20200085", "肖颗", 2, 3},
        {"003AA00003", "形势与政策III", 60, 0.0, "20130679", "高德华", 2, 1},
        {"003EA4006A", "马克思主义基本原理", 60, 2.0, "20131977", "陈雪连", 2, 2},
        {"073BAR0034", "概率论与数理统计", 60, 4.0, "20131918", "吕美英", 1, 2},
        {"04A", "体育Ⅲ（羽毛球）", 60, 1.0, "20132168", "陈丽", 2, 4},
        {"003AA60026", "大学英语IV拓展课系列- AI辅助专门用途英语翻译", 60, 3.0, "20130442", "龙涛", 4, 1},
        {"083BA70007", "操作系统原理与实践", 60, 3.0, "20132108", "杜兴", 1, 2},
        {"083BA80006", "大学物理", 60, 4.0, "20250043", "孙川", 3, 2},
        {"083CA40010", "软件工程导论", 60, 2.0, "20130951", "魏延", 2, 1},
        {"083EB8040A", "Qt6软件开发", 60, 5.0, "20131672", "龚伟", 3, 1},
        {"083FB20057", "软件工程综合实训2-1（C++方向）", 60, 1.0, "20131672", "龚伟", 1, 1},
        {"003AA00004", "形势与政策IV", 60, 0.0, "20131977", "陈雪连", 2, 2},
        {"003AAC002A", "习近平新时代中国特色社会主义思想概论", 60, 3.0, "20220002", "徐琴", 3, 2},
        {"003EAC002B", "毛泽东思想和中国特色社会主义理论体系概论", 60, 2.0, "20150015", "沈乾飞", 3, 1},
        {"04B", "体育Ⅳ（羽毛球）", 60, 1.0, "20132168", "陈丽", 2, 4}
    };

    for (const auto& c : courses) {
        std::string sql = std::format(
            "INSERT INTO course VALUES ('{}', '{}', {}, 0, {}, '{}', '{}', {}, {})",
            c.id, c.name, c.cap, c.cr, c.tid, c.tname, c.w, c.t
        );
        m_db->execute(sql);

        // 自动为课程教师创建账号 (如果不存在)，密码统一为 123
        m_db->execute(std::format(
            "INSERT INTO users (user_id, name, password, role) VALUES ('{}', '{}', '123', 'teacher') ON CONFLICT (user_id) DO NOTHING",
            c.tid, c.tname
        ));
    }

    std::print("Initial data loaded.\n");
}

/**
 * @brief 系统清理
 * 删除所有表结构，用于测试环境复位。
 */
void SystemController::cleanup() {
    if (!m_db->is_connected()) return;

    m_db->execute("DROP TABLE IF EXISTS enrollment CASCADE");
    m_db->execute("DROP TABLE IF EXISTS course CASCADE");
    m_db->execute("DROP TABLE IF EXISTS student CASCADE");
    m_db->execute("DROP TABLE IF EXISTS users CASCADE");

    std::print("System cleanup: Tables dropped.\n");
}

/**
 * @brief 运行主逻辑
 */
void SystemController::run() {
    std::print("System Controller Ready. Please login via UI.\n");
}

/**
 * @brief 处理用户登录请求
 * @param id 用户唯一标识ID
 * @param password 用户密码
 * @return 登录成功返回 true，并保存用户信息至当前会话；否则返回 false
 */
bool SystemController::login(std::string id, std::string password) {
    // 简单的明文密码验证 (实际生产应使用哈希)
    std::string sql = std::format("SELECT name, role FROM users WHERE user_id = '{}' AND password = '{}'", id, password);
    auto res = m_db->query(sql);

    if (res && !res->empty()) {
        std::string name = (*res)[0][0];
        std::string role = (*res)[0][1];
        m_currentUser = User{id, name, role};
        std::print("Login successful: {} ({})\n", name, role);
        return true;
    }

    std::print("Login failed: Invalid credentials.\n");
    return false;
}

/**
 * @brief 执行选课业务逻辑
 * @param sid 学生ID
 * @param cid 课程ID
 * 校验流程：登录态 -> 角色权限 -> 身份匹配 -> 课程存在 -> 学生存在 -> 重复选课 -> 容量检查 -> 时间冲突检测 -> 持久化
 */
void SystemController::performEnrollment(std::string sid, std::string cid) {
    // 权限检查
    if (!m_currentUser.isValid()) {
        std::print("Error: Please login first.\n");
        return;
    }

    if (m_currentUser.role != "student") {
        std::print("Error: Only students can enroll in courses.\n");
        return;
    }

    if (m_currentUser.id != sid) {
        std::print("Error: You can only enroll for yourself.\n");
        return;
    }

    // 1. 获取课程对象
    auto course = infra::CourseProxy::findCourseById(*m_db, cid);
    if (!course) {
        std::print("Error: Course {} not found.\n", cid);
        return;
    }

    // 2. 获取学生对象 (含已选课程)
    auto student = infra::StudentProxy::findStudentById(*m_db, sid);
    if (!student) {
        std::print("Error: Student {} not found.\n", sid);
        return;
    }

    // 3. 执行业务规则校验
    // 3.1 检查是否已选
    if (student->isEnrolled(course.get())) {
        std::print("Error: Already enrolled in {}\n", course->course_info());
        return;
    }

    // 3.2 检查容量
    if (course->isFull()) {
        std::print("Error: Course is full: {}\n", course->course_info());
        return;
    }

    // 3.3 检查时间冲突
    if (student->hasTimeConflict(course.get())) {
        std::print("Error: Time conflict detected for course {}\n", course->course_info());
        return;
    }

    // 4. 持久化 (通过 Proxy)
    if (infra::StudentProxy::saveEnrollment(*m_db, sid, cid)) {
        // 重要：重新加载以显示更新后的人数
        auto updatedCourse = infra::CourseProxy::findCourseById(*m_db, cid);
        std::print("Enrollment successful! {}\n", updatedCourse->course_info());
    } else {
        std::print("Error: Failed to save enrollment to database.\n");
    }
}

/**
 * @brief 执行退课业务逻辑
 * @param sid 学生ID
 * @param cid 课程ID
 */
void SystemController::performDrop(std::string sid, std::string cid) {
    // 权限检查
    if (!m_currentUser.isValid()) {
        std::print("Error: Please login first.\n");
        return;
    }

    if (m_currentUser.role != "student") {
        std::print("Error: Only students can drop courses.\n");
        return;
    }

    if (m_currentUser.id != sid) {
        std::print("Error: You can only drop courses for yourself.\n");
        return;
    }

    // 1. 检查选课关系是否存在
    if (!infra::StudentProxy::isEnrolled(*m_db, sid, cid)) {
        std::print("Error: Student {} is not enrolled in course {}\n", sid, cid);
        return;
    }

    // 2. 执行退课持久化
    if (infra::StudentProxy::removeEnrollment(*m_db, sid, cid)) {
        // 重新加载课程以显示更新后的人数
        auto updatedCourse = infra::CourseProxy::findCourseById(*m_db, cid);
        std::print("Drop successful! {}\n", updatedCourse->course_info());
    } else {
        std::print("Error: Failed to drop course from database.\n");
    }
}

/**
 * @brief 创建新课程 (仅教学秘书可用)
 */
bool SystemController::createCourse(std::string id, std::string name, int capacity, double credit,
                                    std::string teacherId, std::string teacherName, int weekday, int timeslot) {
    // 权限检查
    if (!m_currentUser.isValid() || m_currentUser.role != "secretary") {
        std::print("Error: Only secretary can create courses.\n");
        return false;
    }

    // 检查课程ID是否已存在
    auto existingCourse = infra::CourseProxy::findCourseById(*m_db, id);
    if (existingCourse) {
        std::print("Error: Course ID {} already exists.\n", id);
        return false;
    }

    // 检查教师时间冲突
    if (infra::CourseProxy::hasTeacherTimeConflict(*m_db, teacherId, weekday, timeslot)) {
        std::print("Error: Teacher {} already has a course at Weekday {} Slot {}.\n", teacherName, weekday, timeslot);
        return false;
    }

    // 构造课程对象并调用 Proxy 持久化
    Course newCourse(id, name, capacity, 0, credit, teacherId, teacherName, Timeslot(weekday, timeslot));
    if (infra::CourseProxy::addCourse(*m_db, newCourse)) {
        // 自动为教师创建账号 (如果不存在)
        m_db->execute(std::format(
            "INSERT INTO users (user_id, name, password, role) VALUES ('{}', '{}', '123', 'teacher') ON CONFLICT (user_id) DO NOTHING",
            teacherId, teacherName
        ));

        std::print("Course {} created successfully!\n", name);
        return true;
    } else {
        std::print("Error: Failed to create course {}\n", name);
        return false;
    }
}

/**
 * @brief 删除课程 (仅教学秘书可用)
 */
bool SystemController::removeCourse(std::string courseId) {
    if (!m_currentUser.isValid() || m_currentUser.role != "secretary") {
        std::print("Error: Only secretary can remove courses.\n");
        return false;
    }
    return infra::CourseProxy::deleteCourse(*m_db, courseId);
}

/**
 * @brief 为课程分配教师 (仅教学秘书可用)
 */
bool SystemController::assignTeacherToCourse(std::string courseId, std::string teacherId, std::string teacherName) {
    // 权限检查
    if (!m_currentUser.isValid() || m_currentUser.role != "secretary") {
        std::print("Error: Only secretary can assign teachers to courses.\n");
        return false;
    }

    // 执行更新
    if (infra::CourseProxy::updateTeacher(*m_db, courseId, teacherId, teacherName)) {
        // 自动为教师创建账号
        m_db->execute(std::format(
            "INSERT INTO users (user_id, name, password, role) VALUES ('{}', '{}', '123', 'teacher') ON CONFLICT (user_id) DO NOTHING",
            teacherId, teacherName
        ));
        std::print("Teacher {} assigned to course {} successfully!\n", teacherName, courseId);
        return true;
    }
    return false;
}

/**
 * @brief 解绑教师 (仅教学秘书可用)
 */
bool SystemController::unassignTeacher(std::string courseId) {
    if (!m_currentUser.isValid() || m_currentUser.role != "secretary") {
        std::print("Error: Only secretary can unassign teachers.\n");
        return false;
    }
    if (infra::CourseProxy::updateTeacher(*m_db, courseId, "", "未分配")) {
        std::print("Teacher unassigned from course {} successfully!\n", courseId);
        return true;
    }
    return false;
}

/**
 * @brief 修改课程时间槽 (仅教学秘书可用)
 * @param courseId 课程ID
 * @param weekday 星期 (1-7, 0=无固定时间)
 * @param timeslot 节次
 * @return 修改成功返回true，否则返回false
 */
bool SystemController::setCourseTimeSlot(std::string courseId, int weekday, int timeslot) {
    // 权限检查
    if (!m_currentUser.isValid() || m_currentUser.role != "secretary") {
        std::print("Error: Only secretary can modify course timeslot.\n");
        return false;
    }

    // 参数合法性校验
    if (weekday < 0 || weekday > 7) {
        std::print("Error: Invalid weekday (must be 0-7).\n");
        return false;
    }

    if (timeslot < 0) {
        std::print("Error: Invalid timeslot (must be >= 0).\n");
        return false;
    }

    // 检查课程是否存在
    auto course = infra::CourseProxy::findCourseById(*m_db, courseId);
    if (!course) {
        std::print("Error: Course {} not found.\n", courseId);
        return false;
    }

    // 执行更新
    std::string sql = std::format(
        "UPDATE course SET weekday = {}, timeslot = {} WHERE id = '{}'",
        weekday, timeslot, courseId
    );

    if (m_db->execute(sql)) {
        std::print("Course timeslot updated: {}\n", course->course_info());
        return true;
    } else {
        std::print("Error: Failed to update timeslot for course {}\n", courseId);
        return false;
    }
}

/**
 * @brief 录入/修改学生成绩 (仅教师可用)
 * @param sid 学生ID
 * @param cid 课程ID
 * @param score 成绩 (0-100)
 * @return 操作成功返回true，否则返回false
 */
bool SystemController::updateGrade(std::string sid, std::string cid, int score) {
    // 权限检查
    if (!m_currentUser.isValid() || m_currentUser.role != "teacher") {
        std::print("Error: Only teachers can update grades.\n");
        return false;
    }

    // 成绩范围校验
    if (score < 0 || score > 100) {
        std::print("Error: Invalid score (must be 0-100).\n");
        return false;
    }

    // 检查课程是否存在且为当前教师授课
    auto course = infra::CourseProxy::findCourseById(*m_db, cid);
    if (!course) {
        std::print("Error: Course {} not found.\n", cid);
        return false;
    }

    if (!course->isTaughtBy(m_currentUser.id)) {
        std::print("Error: You are not the teacher of course {}.\n", cid);
        return false;
    }

    // 检查学生是否选修该课程
    if (!infra::StudentProxy::isEnrolled(*m_db, sid, cid)) {
        std::print("Error: Student {} is not enrolled in course {}.\n", sid, cid);
        return false;
    }

    // 执行成绩更新 (存在则更新，不存在则插入)
    std::string sql = std::format(
        "INSERT INTO enrollment (student_id, course_id, score) VALUES ('{}', '{}', {}) "
        "ON CONFLICT (student_id, course_id) DO UPDATE SET score = EXCLUDED.score",
        sid, cid, score
    );

    if (m_db->execute(sql)) {
        std::print("Grade updated successfully! Student {}: Course {} score = {}\n", sid, cid, score);
        return true;
    } else {
        std::print("Error: Failed to update grade for student {} in course {}\n", sid, cid);
        return false;
    }
}

// --- Query Methods Implementation ---

// 获取当前登录学生的课表
std::vector<Course> SystemController::queryMySchedule() {
    if (!m_currentUser.isValid() || m_currentUser.role != "student") return {};
    return infra::StudentProxy::findSchedule(*m_db, m_currentUser.id);
}

// 获取当前登录学生的成绩
std::vector<infra::GradeDTO> SystemController::queryMyGrades() {
    if (!m_currentUser.isValid() || m_currentUser.role != "student") return {};
    return infra::EnrollmentProxy::findGradesByStudent(*m_db, m_currentUser.id);
}

// 获取某门课程的学生名单 (仅教师/管理员可用)
std::vector<infra::CourseStudentDTO> SystemController::queryCourseStudentList(std::string courseId) {
    // 简单权限校验
    if (!m_currentUser.isValid()) return {};
    if (m_currentUser.role == "student") return {}; // 学生不可见
    return infra::CourseProxy::findStudentsByCourse(*m_db, courseId);
}

// 获取所有课程列表 (公共查询)
std::vector<std::unique_ptr<Course>> SystemController::queryAllCourses() {
    return infra::CourseProxy::findAllCourses(*m_db);
}

// 根据课程ID获取课程对象
std::unique_ptr<Course> SystemController::findCourseById(std::string courseId) {
    if (!m_currentUser.isValid()) return nullptr;
    return infra::CourseProxy::findCourseById(*m_db, courseId);
}
