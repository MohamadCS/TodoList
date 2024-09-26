#include "../include/TodoList/Database.hpp"
#include "../include/TodoList/Utils.hpp"
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace TodoList::Core {

sqlite3* Database::getDb() const {
    return m_db;
}

Database::Database(const std::string& name, const std::filesystem::path& path)
    : m_name(name),
      m_path(path) {

    std::string dbPath = m_path.append(m_name);
    LOG("Creating a new database in {}", dbPath);

    if (!std::filesystem::exists(dbPath)) {
        std::ofstream newFile(dbPath);
        if (!newFile) {
            LOG("Could not open the file");
        }
        newFile.close();
    } else {
        LOG("Path exists !");
    }

    int err = sqlite3_open(std::move(dbPath).c_str(), &m_db);
    if (err) {
        LOG("Could not open database");
    }
}

void Database::newTable(const std::string& name, std::vector<std::pair<std::string, std::string>>&& columnsAndSettings,
                        bool ifNotExist) {
    std::string ifNotExistsStr = ifNotExist ? "IF NOT EXISTS" : "";

    auto columnsQueryStr = Utility::convertContainerToListStr<decltype(columnsAndSettings.begin())>(
        columnsAndSettings.begin(), columnsAndSettings.end(), [](decltype(columnsAndSettings.begin()) it) {
            auto [column, setting] = *it;
            return column + " " + setting;
        });

    LOG("{}", columnsQueryStr);
    std::vector<std::string> columns, settings;

    for (auto& [column, setting] : columnsAndSettings) {
        columns.emplace_back(std::move(column));
        settings.emplace_back(std::move(setting));
    }

    std::string query =
        std::format("CREATE TABLE {} {} ({});", std::move(ifNotExistsStr), name, std::move(columnsQueryStr));

    LOG("Executing: {}", query);

    char* errorMsg;
    int rc = sqlite3_exec(m_db, query.c_str(), nullptr, 0, &errorMsg);

    if (rc != SQLITE_OK) {
       LOG("Error: {} {}", name, errorMsg);
    }

    // Adding a new table
    m_tables.insert_or_assign(std::move(name), std::move(columns));
    LOG("Added Table {}", name);
}

void Database::insertEntry(const std::string& tableName, const TblEntry& entry) {
    auto it = m_tables.find(tableName);

    if (it == m_tables.end()) {
        LOG("Table {} not found", tableName);
        return;
    }

    std::string columnsListStr = Utility::convertContainerToListStr<decltype(entry.begin())>(
        entry.begin(), entry.end(), [](decltype(entry.begin()) it) { return it->first; });

    std::string valuesListStr = Utility::convertContainerToListStr<decltype(entry.begin())>(
        entry.begin(), entry.end(), [](decltype(entry.begin()) it) { return it->second; });

    std::string query =
        std::format("INSERT INTO {} ({}) VALUES ({})", tableName, std::move(columnsListStr), std::move(valuesListStr));
    LOG("Executing : {}", query);

    char* errorMsg;
    int rc = sqlite3_exec(m_db, query.c_str(), nullptr, 0, &errorMsg);
    if (rc != SQLITE_OK) {
        LOG("Error: {}", errorMsg);
    }
}

void Database::updateEntry(const std::string& tableName,
                           const std::vector<std::tuple<std::string, std::string, std::string>>& whereVec,
                           const TblEntry& colValUpdateVec, const std::vector<std::string> binopOpVec) {
    auto it = m_tables.find(tableName);

    if (it == m_tables.end()) {
        LOG("Table {} not found", tableName);
        return;
    }

    if (whereVec.size() - 1 != binopOpVec.size()) {
        LOG("Size mismatch");
    }

    std::string whereListStr = Utility::convertContainerToListStr<decltype(whereVec.begin())>(
        whereVec.begin(), whereVec.end(),
        [](decltype(whereVec.begin()) it) {
            auto& [column, op, val] = *it;
            return column + op + val;
        },
        "", binopOpVec);

    std::string newValListStr = Utility::convertContainerToListStr<decltype(colValUpdateVec.begin())>(
        colValUpdateVec.begin(), colValUpdateVec.end(),
        [](decltype(colValUpdateVec.begin()) it) { return it->first + " = " + it->second; });

    std::string query =
        std::format("UPDATE {} SET {} WHERE {};", tableName, std::move(newValListStr), std::move(whereListStr));

    LOG("Executing : {}", query);

    char* errorMsg;
    int rc = sqlite3_exec(m_db, query.c_str(), nullptr, 0, &errorMsg);
    if (rc != SQLITE_OK) {
        LOG("Error: {}", errorMsg);
    }
}

} // namespace TodoList::Core
