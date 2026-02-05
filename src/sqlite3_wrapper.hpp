#pragma once

#include <filesystem>
#include <format>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include <map>

#include "db_con.hpp"

namespace sqlite3_wrapper {

class DB {
   public:
    DB(const std::filesystem::path& path);

   public:
    void createTable(
        const std::string& name,
        const std::pair<std::string, std::string>& primaryKey,
        const std::vector<std::pair<std::string, std::string>>& entries,
        const std::map<std::string, std::string> defaultValues = {});

    template <class... T>
    std::vector<std::tuple<T...>> getRows(
        const std::string& table,
        const std::vector<std::pair<std::string, std::string>>& whereAnd) {
        std::stringstream stm;
        stm << "SELECT * FROM ";
        stm << table;

        stm << " WHERE ";
        std::size_t idx = 0;
        for (auto&& [name, val] : whereAnd) {
            stm << std::format("{} = '{}'", name, val);
            if (idx++ < whereAnd.size() - 1) {
                stm << " AND ";
            }
        }

        auto rows = con_.exec(stm.str());
        std::vector<std::tuple<T...>> res;
        res.reserve(rows.size());
        for (auto&& row : rows) {
            res.push_back(row_<T...>(row));
        }
        return res;
    }

    template <class... T>
    std::vector<std::tuple<T...>> getAllRows(const std::string& table) {
        auto rows = con_.exec("SELECT * FROM " + table);
        std::vector<std::tuple<T...>> res;
        res.reserve(rows.size());
        for (auto&& row : rows) {
            res.push_back(row_<T...>(row));
        }
        return res;
    }

    void addRow(const std::string& table,
                const std::vector<std::string>& columns = {},
                const std::vector<std::string>& values = {});

    void deleteRows(
        const std::string& table,
        const std::vector<std::pair<std::string, std::string>>& whereAnd);

    void setValuesInRows(
        const std::string& table,
        const std::vector<std::pair<std::string, std::string>>& newValues,
        const std::vector<std::pair<std::string, std::string>>& whereAnd);

   private:
    template <class T>
    static T convert_(const std::string& x) {
        if constexpr (std::is_same_v<T, int>) {
            return std::stoi(x);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return x;
        } else if constexpr (std::is_same_v<T, bool>) {
            return x.front() == 'T';
        } else {
            static_assert(false && "The type is not supported");
        }
    }

    template <class... U>
    std::tuple<U...> row_(const std::vector<std::string>& row) {
        auto it = row.rbegin();
        return std::tuple<U...>(convert_<U>(*(it++))...);
    }

   private:
    detail__::DBCon con_;
};

}  // namespace sqlite3_wrapper