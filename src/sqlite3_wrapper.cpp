#include "sqlite3_wrapper.hpp"

#include <stdexcept>
#include <ranges>
#include <sstream>

namespace {

std::vector<
    std::vector<std::pair<std::string, std::string>>> rows;

int callback(
    void*, int argc, char** argv, char** azColName) 
{   
    rows.emplace_back();
    rows.back().reserve(argc);
    for (int i = 0; i < argc; ++i) {
        rows.back().push_back({azColName[i], 
                argv[i] ? argv[i] : "NULL"});
    }
    return 0;
}

} // namespace

namespace detail__ {

DBCon::DBCon(const std::filesystem::path& path) {
    auto notOk = sqlite3_open(path.c_str(), &con_);
    if (notOk) {
        throw std::runtime_error(
            "Cant't open DB: " + path.string());
    }
}

DBCon::~DBCon() {
    sqlite3_close(con_);
}

std::vector<
    std::vector<std::pair<std::string, std::string>>>
DBCon::exec(const std::string& stm) {    
    char* errMsg = nullptr;
    int ok = sqlite3_exec(
            con_, stm.c_str(), callback, nullptr, &errMsg);
    if (ok != SQLITE_OK) {
        std::runtime_error re(errMsg);
        sqlite3_free(errMsg);
        throw re;
    }
    return rows;
}

} // namespace detail__

namespace sqlite3_wrapper {
DB::DB(const std::filesystem::path& path) :
    con_(detail__::DBCon(path))
{}

void DB::createTable(
    const std::string& name, 
    const std::pair<std::string, std::string>& primaryKey,  
    const std::vector<std::pair<std::string, std::string>>& entries) 
{   
    std::stringstream stm;
    stm << "CREATE TABLE IF NOT EXISTS ";
    stm << name;
    
    stm << " (";

    stm << primaryKey.first 
        << ' '
        << primaryKey.second
        << " PRIMARY KEY"
        << " AUTOINCREMENT";
    
    for (auto&& [name, type] : entries) {
        stm << ", ";
        stm << name << ' ' << type;
    }

    stm << " )";

    auto rows = con_.exec(stm.str());
}

void DB::addRow(
    const std::string& table,
    const std::vector<std::string>& columns,
    const std::vector<std::string>& values)
{
    std::stringstream stm;
    stm << "INSERT INTO ";
    stm << table;

    stm << " (";
    std::size_t idx = 0;
    for (auto n : columns) {
        stm << n;
        if (idx++ < columns.size() - 1) {
            stm << ", ";
        }
    }
    stm << ")";

    stm << " VALUES";
    stm << " (";
    idx = 0;
    for (auto n : values) {
        stm << '\'' << n << '\'';
        if (idx++ < values.size() - 1) {
            stm << ", ";
        }
    }
    stm << ")";
    std::string s = stm.str();
    con_.exec(stm.str());
}

} // namespace sqlite3_wrapper