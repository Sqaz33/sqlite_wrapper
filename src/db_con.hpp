#include <filesystem>
#include <vector>
#include <string>

#include <sqlite3.h>

namespace detail__ {
    
class DBCon {
public:
    DBCon(const std::filesystem::path& path);

    ~DBCon();

    DBCon(const DBCon&) = delete;
    DBCon& operator=(const DBCon&) = delete;
 
public:
    std::vector<std::vector<std::string>>
    exec(const std::string& stm);

private:    
    sqlite3* con_;  
};

} // namespace detail__