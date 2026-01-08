/**
* @file    src/CourseSelectionSystem/domain/dom.timeslot.cppm
* @date    2026-01-08
* @brief   Domain partition: Timeslot value object
*/
export module course_system:domain.timeslot;

import std;

export struct Timeslot {
    int weekday;   // 1 (Monday) - 7 (Sunday)
    int timeslot;  // 1=1-2节, 2=3-4节...

    // 核心算法：检测两个时间段是否冲突
    // 逻辑：如果星期相同 且 节次相同，则冲突
    bool overlaps(const Timeslot& other) const {
        return (this->weekday == other.weekday) && (this->timeslot == other.timeslot);
    }

    // 辅助功能：转为字符串用于显示 (e.g., "Mon Section 1")
    std::string toString() const {
        static const std::vector<std::string> days = {"", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
        std::string day_str = (weekday >= 1 && weekday <= 7) ? days[weekday] : "Unknown";
        return std::format("{} Section {}", day_str, timeslot);
    }
};
