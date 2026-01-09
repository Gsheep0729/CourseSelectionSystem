/**
* @file    src/CourseSelectionSystem/domain/dom.timeslot.cppm
* @date    2026-01-10
* @author  GY
* @brief   领域层分区：时间槽值对象
*
* 定义时间槽值对象，用于表示课程的上课时间（星期 + 节次）
* 提供时间冲突检测逻辑
* 提供时间冲突检测逻辑
*
* Change Log:
* [v1.0] GY   2026-01-10
* * 初始版本：实现 Timeslot 类及 overlaps 方法
*/

export module domain:timeslot;

import std;

export class Timeslot {
public:
    // 构造函数
    // weekday: 1=周一, 7=周日, 0=网络/无固定时间
    // period: 1=第1-2节, 2=第3-4节, ...
    Timeslot(int weekday, int period);

    // 检查是否与另一个时间槽冲突
    bool overlaps(const Timeslot& other) const;

    // 获取格式化的时间字符串
    std::string toString() const;

    // Getters
    int getWeekday() const { return m_weekday; }
    int getPeriod() const { return m_period; }

private:
    int m_weekday;
    int m_period;
};

// --- Implementation ---

Timeslot::Timeslot(int weekday, int period)
    : m_weekday(weekday), m_period(period) {}

/**
* @brief 检查是否与另一个时间槽冲突
* @param other 另一个时间槽
* @return true 如果冲突（时间重叠），否则 false
*/
bool Timeslot::overlaps(const Timeslot& other) const {
    // 如果任意一个是网络课(0)，则不冲突
    if (m_weekday == 0 || other.m_weekday == 0) {
        return false;
    }

    // 简单策略：星期相同且节次相同则冲突
    return (m_weekday == other.m_weekday) && (m_period == other.m_period);
}

/**
* @brief 获取格式化的时间字符串
* @return 例如 "Mon Slot 1" 或 "Online"
*/
std::string Timeslot::toString() const {
    if (m_weekday == 0) {
        return "Online/No Set Time";
    }

    static const std::vector<std::string> weeks = {
        "", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
    };

    std::string w_str = (m_weekday >= 1 && m_weekday <= 7) ? weeks[m_weekday] : "Unknown";
    return std::format("{} Slot {}", w_str, m_period);
}
