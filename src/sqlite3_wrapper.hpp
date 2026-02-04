#pragma once

#include <filesystem>
#include <string>
#include <utility>
#include <vector>
#include <tuple>

#include "db_con.hpp"

namespace sqlite3_wrapper {

class DB {
public:
    DB(const std::filesystem::path& path);

public:
    void createTable(
        const std::string& name, 
        const std::pair<std::string, std::string>& primaryKey,  
        const std::vector<std::pair<std::string, std::string>>& entries);

    // template <class ... T>
    // std::vector<std::tuple<T...>>
    // getSpesificRows(
    //     const std::string& table, 
    //     const std::vector<std::pair<std::string, std::string>>& where)
    // {
    //     std::stringstream stm;
    //     stm << "SELECT * FROM ";
    //     stm << name;
    //     stm << "";
    // }

    template <class ... T>
    std::vector<std::tuple<T...>> 
    getAllRows(const std::string& table) {
        auto rows = con_.exec("SELECT * FROM " + table);
        std::vector<std::tuple<T...>> res;
        res.reserve(rows.size());
        for (auto&& row : rows) {
            res.push_back(row_<T...>(row, 0));
        }
        return res;
    }

    void addRow(
        const std::string& table,
        const std::vector<std::string>& columns,
        const std::vector<std::string>& values);
        
private:    
    template <class T>
    std::tuple<T> row_(
        const std::vector<std::string>& rows, 
        int idx
    ) 
    {
        auto&& val = rows[idx];
        if constexpr (std::is_same_v<T, int>) {
            return std::tuple<int>(std::stoi(val));
        } else if constexpr (std::is_same_v<T, std::string>)  {
            return std::tuple<std::string>(val);
        } else if constexpr (true) {
            static_assert(false && "The type is not supported");
        }
    }

    template <class T, class ... U>
    requires (sizeof...(U) > 0)
    std::tuple<T, U...> row_(
        const std::vector<std::string>& rows, 
        int idx
    ) 
    {   
        auto r1 = row_<T>(rows, idx++);
        auto r2 = row_<U...>(rows, idx);
        return std::tuple_cat(std::move(r1), 
                              std::move(r2));
    }
    
private:
    detail__::DBCon con_;
};

} // namespace sqlite3_wrapper