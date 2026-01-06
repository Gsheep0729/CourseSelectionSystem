module;
#include <pqxx/pqxx>

module course_system; // General implementation unit

import :infrastructure.db_connection;
import std;

PostgresConnection& PostgresConnection::instance() {
    static PostgresConnection instance;
    return instance;
}

pqxx::connection& PostgresConnection::get_connection() {
    return *m_conn;
}

PostgresConnection::PostgresConnection() {
    try {
        std::string conn_str = "dbname=course user=postgres password=root host=localhost port=5432";
        m_conn = std::make_unique<pqxx::connection>(conn_str);

        if (m_conn->is_open()) {
            std::print("[Infrastructure] Connected to database: {}\n", m_conn->dbname());
        } else {
            std::print("[Infrastructure] Failed to open database.\n");
        }
    } catch (const std::exception& e) {
        std::print("[Infrastructure] Connection Error: {}\n", e.what());
    }
}

PostgresConnection::~PostgresConnection() = default;