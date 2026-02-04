#include <exception>
#include <fstream>
#include <iostream>

#include <gtest/gtest.h>

#include "sqlite3_wrapper.hpp"

#ifndef TEST_DB_FILE_PATH
#error "It is necessary to declare"
        " the file name of the test db"
#endif

    static void
    clearDB() {
    std::ofstream f(TEST_DB_FILE_PATH, std::ios::trunc);
}

TEST(DbCreation, DefaultCreation) {
    clearDB();
    try {
        sqlite3_wrapper::DB db(TEST_DB_FILE_PATH);
    } catch (const std::exception& e) {
        ASSERT_FALSE(true) << e.what();
    } catch (...) {
        ASSERT_FALSE(true) << "Unknown error";
    }
}

TEST(TableCreation, NoExceptTableCreation) {
    clearDB();
    sqlite3_wrapper::DB db(TEST_DB_FILE_PATH);
    ASSERT_NO_THROW(
        db.createTable("table1", {std::string("id"), std::string("INTEGER")},
                       {{std::string("name"), std::string("TEXT")}}));
}

TEST(ValueInsertion, DefaultInsertion) {
    clearDB();
    sqlite3_wrapper::DB db(TEST_DB_FILE_PATH);
    db.createTable("table1", {std::string("id"), std::string("INTEGER")},
                   {{std::string("name"), std::string("TEXT")}});

    ASSERT_NO_THROW(db.addRow("table1", {"name"}, {"Ivan"}););
}

TEST(GettingValue, DefaultGetting) {
    clearDB();
    sqlite3_wrapper::DB db(TEST_DB_FILE_PATH);

    std::string table = "table1";
    std::string name = "name";

    std::string expectValue = "Ivan";

    db.createTable(table, {std::string("id"), std::string("INTEGER")},
                   {{name, std::string("TEXT")}});
    db.addRow("table1", {name}, {expectValue});

    try {
        auto res = db.getAllRows<int, std::string>(table);
        ASSERT_TRUE(res.size() == 1);
        auto actualValue = std::get<1>(res[0]);
        ASSERT_EQ(actualValue, expectValue);

    } catch (const std::exception& e) {
        ASSERT_FALSE(true) << e.what();
    } catch (...) {
        ASSERT_FALSE(true) << "Unknown error";
    }
}
