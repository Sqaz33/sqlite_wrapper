#pragma once

#include <filesystem>
#include <string>
#include <utility>
#include <vector>
#include <tuple>

#include <sqlite3.h>

namespace detail__ {
class DBCon {
public:
    DBCon(const std::filesystem::path& path);

    ~DBCon();

    DBCon(const DBCon&) = delete;
    DBCon& operator=(const DBCon&) = delete;
 
public:
    std::vector<
        std::vector<std::pair<std::string, std::string>>>
    exec(const std::string& stm);

private:    
    sqlite3* con_;  
};

} // namespace detail__

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
            res.push_back(rows_<T...>(row, 0));
        }
        return res;
    }

    void addRow(
        const std::string& table,
        const std::vector<std::string>& columns,
        const std::vector<std::string>& values);
        
private:    
    template <class T>
    std::tuple<T> rows_(
        const std::vector<
            std::pair<
                std::string, std::string>>& rows, 
        int idx
    ) 
    {
        auto&& [_, val] = rows[idx];
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
    std::tuple<T, U...> rows_(
        const std::vector<
            std::pair<
                std::string, std::string>>& rows, 
        int idx
    ) 
    {   
        auto r1 = rows_<T>(rows, idx++);
        auto r2 = rows_<U...>(rows, idx);
        return std::tuple_cat(std::move(r1), 
                              std::move(r2));
    }
    
private:
    detail__::DBCon con_;
};

} // namespace sqlite3_wrapper