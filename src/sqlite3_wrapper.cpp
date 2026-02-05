#include "sqlite3_wrapper.hpp"

#include <ranges>
#include <sstream>
#include <stdexcept>

namespace sqlite3_wrapper {
DB::DB(const std::filesystem::path& path) : con_(detail__::DBCon(path)) {}

void DB::createTable(
    const std::string& name,
    const std::pair<std::string, std::string>& primaryKey,
    const std::vector<std::pair<std::string, std::string>>& entries) {
    std::stringstream stm;
    stm << "CREATE TABLE IF NOT EXISTS ";
    stm << name;

    stm << " (";

    stm << primaryKey.first << ' ' << primaryKey.second << " PRIMARY KEY"
        << " AUTOINCREMENT";

    for (auto&& [name, type] : entries) {
        stm << ", ";
        stm << name << ' ' << type;
    }

    stm << " )";

    con_.exec(stm.str());
}

void DB::addRow(const std::string& table,
                const std::vector<std::string>& columns,
                const std::vector<std::string>& values) {
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

void DB::deleteRows(
    const std::string& table,
    const std::vector<std::pair<std::string, std::string>>& whereAnd) {
    std::stringstream stm;
    stm << "DELETE FROM ";
    stm << table;
    stm << " WHERE ";

    std::size_t idx = 0;
    for (auto&& [name, val] : whereAnd) {
        stm << std::format("{} = '{}'", name, val);
        if (idx++ < whereAnd.size() - 1) {
            stm << " AND ";
        }
    }

    con_.exec(stm.str());
}

void DB::setValuesInRows(
    const std::string& table,
    const std::vector<std::pair<std::string, std::string>>& newValues,
    const std::vector<std::pair<std::string, std::string>>& whereAnd) {
    std::stringstream stm;
    stm << "UPDATE ";
    stm << table;

    stm << " SET ";
    std::size_t idx = 0;
    for (auto&& [name, val] : newValues) {
        stm << std::format("{} = '{}'", name, val);
        if (idx++ < newValues.size() - 1) {
            stm << ", ";
        }
    }

    stm << " WHERE ";
    idx = 0;
    for (auto&& [name, val] : whereAnd) {
        stm << std::format("{} = '{}'", name, val);
        if (idx++ < whereAnd.size() - 1) {
            stm << " AND ";
        }
    }

    con_.exec(stm.str());
}

}  // namespace sqlite3_wrapper