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

TEST(TableCreation, WithDefaultValue) {
    clearDB();
    sqlite3_wrapper::DB db(TEST_DB_FILE_PATH);

    std::string table = "table1";
    std::string name = "name";

    std::string expectValue = "Ivan";

    db.createTable(table, {std::string("id"), std::string("INTEGER")},
                   {{name, std::string("TEXT")}}, {{name, expectValue}});
    db.addRow("table1");

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

TEST(GettingValue, GettingSpecificRow) {
    clearDB();
    sqlite3_wrapper::DB db(TEST_DB_FILE_PATH);

    std::string table = "table1";
    std::string name = "name";
    std::string value = "value";

    std::string expect1 = "Ivan";
    int expect2 = 10;

    db.createTable(
        table, {std::string("id"), std::string("INTEGER")},
        {{name, std::string("TEXT")}, {value, std::string("INTEGER")}});
    db.addRow("table1", {name, value}, {expect1, std::to_string(expect2)});

    try {
        auto res = db.getRows<int, std::string, int>(
            table, {{name, expect1}, {value, std::to_string(expect2)}});
        ASSERT_EQ(res.size(), 1);

        auto&& [_, actual1, actual2] = res[0];

        EXPECT_EQ(actual1, expect1);
        EXPECT_EQ(actual2, expect2);

    } catch (const std::exception& e) {
        ASSERT_FALSE(true) << e.what();
    } catch (...) {
        ASSERT_FALSE(true) << "Unknown error";
    }
}

TEST(DeleteRows, Default) {
    clearDB();
    sqlite3_wrapper::DB db(TEST_DB_FILE_PATH);

    std::string table = "table1";
    std::string name = "name";
    std::string value = "value";

    db.createTable(
        table, {std::string("id"), std::string("INTEGER")},
        {{name, std::string("TEXT")}, {value, std::string("INTEGER")}});

    std::string row1name = "Ivan";
    int row1value = 10;
    db.addRow("table1", {name, value}, {row1name, std::to_string(row1value)});

    std::string row2name = "Ivan2";
    int row2value = 102;
    db.addRow("table1", {name, value}, {row2name, std::to_string(row2value)});

    EXPECT_NO_THROW(db.deleteRows(
        table, {{name, row1name}, {value, std::to_string(row1value)}}));

    auto res = db.getAllRows<int, std::string, int>(table);
    EXPECT_EQ(res.size(), 1);

    auto&& [_, actualRow2name, actualRow2value] = res[0];
    EXPECT_EQ(actualRow2name, row2name);
    EXPECT_EQ(actualRow2value, row2value);
}

TEST(SetValues, Default) {
    clearDB();
    sqlite3_wrapper::DB db(TEST_DB_FILE_PATH);

    std::string table = "table1";
    std::string name = "name";
    std::string value = "value";

    db.createTable(
        table, {std::string("id"), std::string("INTEGER")},
        {{name, std::string("TEXT")}, {value, std::string("INTEGER")}});

    std::string curName = "Ivan";
    int curValue = 102;
    db.addRow("table1", {name, value}, {curName, std::to_string(curValue)});

    std::string expectName = "New Ivan";
    int expectValue = 103;

    EXPECT_NO_THROW(db.setValuesInRows(
        table, {{name, expectName}, {value, std::to_string(expectValue)}},
        {{name, curName}}));

    auto res = db.getAllRows<int, std::string, int>(table);
    EXPECT_EQ(res.size(), 1);

    auto&& [_, actualName, actualValue] = res[0];
    EXPECT_EQ(actualName, expectName);
    EXPECT_EQ(actualValue, expectValue);
}