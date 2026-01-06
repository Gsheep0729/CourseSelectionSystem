module;
namespace pqxx { class connection; }
export module course_system:infrastructure.db_connection;

import std;

export class PostgresConnection {
public:
    static PostgresConnection& instance();
    pqxx::connection& get_connection();

    PostgresConnection(const PostgresConnection&) = delete;
    PostgresConnection& operator=(const PostgresConnection&) = delete;

private:
    PostgresConnection();
    ~PostgresConnection();
    std::unique_ptr<pqxx::connection> m_conn;
};