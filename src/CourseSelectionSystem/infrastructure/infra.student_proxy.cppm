module;
#include <string>
export module course_system:infrastructure.student_proxy;

import :domain;
import :infrastructure.db_connection;
// import std;

export class StudentProxy {
public:
    static Student* findById(std::string_view id);
    static void save(const Student& s);
};
