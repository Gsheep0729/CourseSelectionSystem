module;
#include <pqxx/pqxx>

module course_system;

import :infrastructure.student_proxy;
import :infrastructure.db_connection;
import :domain;
import std;

Student* StudentProxy::findById(std::string_view id) {
    try {
        auto& C = PostgresConnection::instance().get_connection();

        if (!C.is_open()) {
            std::print("[Proxy] Error: DB Connection is not open.\n");
            return nullptr;
        }

        pqxx::nontransaction N(C);
        std::string sql = std::format("SELECT name FROM users WHERE user_id = '{}'", id);
        pqxx::result R = N.exec(sql);

        if (R.empty()) {
            std::print("[Proxy] Student ID {} not found in DB.\n", id);
            return nullptr;
        }

        std::string name = R[0][0].as<std::string>();
        std::print("[Proxy] Loaded Student from DB: {} - {}\n", id, name);

        return new Student(std::string(id), name);

    } catch (const std::exception& e) {
        std::print("[Proxy] findById Exception: {}\n", e.what());
        return nullptr;
    }
}

void StudentProxy::save(const Student& s) {
    try {
        std::print("[Proxy] (Mock) Saving Student {} to DB...\n", s.student_info());
    } catch (const std::exception& e) {
        std::print("[Proxy] save Exception: {}\n", e.what());
    }
}