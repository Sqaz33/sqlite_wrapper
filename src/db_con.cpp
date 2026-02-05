#include "db_con.hpp"

namespace {

std::vector<std::vector<std::string>> rows;

int callback(void*, int argc, char** argv, char** azColName) {
    rows.emplace_back();
    rows.back().reserve(argc);
    for (int i = 0; i < argc; ++i) {
        rows.back().push_back(argv[i] ? argv[i] : "NULL");
    }
    return 0;
}

}  // namespace

namespace sqlite3_wrapper::detail__ {

DBCon::DBCon(const std::filesystem::path& path) {
    auto notOk = sqlite3_open(path.c_str(), &con_);
    if (notOk) {
        throw std::runtime_error("Cant't open DB: " + path.string());
    }
}

DBCon::~DBCon() {
    sqlite3_close(con_);
}

std::vector<std::vector<std::string>> DBCon::exec(const std::string& stm) {
    char* errMsg = nullptr;
    int ok = sqlite3_exec(con_, stm.c_str(), callback, nullptr, &errMsg);
    if (ok != SQLITE_OK) {
        std::runtime_error re(errMsg);
        sqlite3_free(errMsg);
        throw re;
    }

    decltype(rows) res;
    res.swap(rows);
    return res;
}

}  // namespace sqlite3_wrapper::detail__