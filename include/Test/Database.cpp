#include "../include/TodoList/Database.hpp"
#include <functional>
#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <vector>
namespace TodoList::Core {

template <class Iterator>
static std::string convertItemListToStr(Iterator begin, Iterator end, std::function<std::string(Iterator)> itemFormat) {
    std::stringstream columnsQuery;
    for (Iterator it = begin; it != end; ++it) {

        columnsQuery << itemFormat(it);

        if (std::next(it) != end) {
            columnsQuery << ",";
        }
    }

    return columnsQuery.str();
}

Database::Database(const std::string& name, const std::filesystem::path& path)
    : m_name(name),
      m_path(path) {

    // check wether
    std::string dbPath = m_path.append(m_name);

    if (!std::filesystem::exists(dbPath)) {
        std::ofstream newFile(dbPath);
        if (!newFile) {
            std::cerr << "Could not open the file" << '\n';
        }
    }

    int err = sqlite3_open(std::move(dbPath).c_str(), &m_db);
    if (err) {
        std::cerr << "Could not open database" << '\n';
    }
}

void Database::newTable(const std::string& name, std::vector<std::pair<std::string, std::string>>&& columnsAndSettings,
                        bool ifNotExist) {
    std::string ifNotExistsStr = ifNotExist ? "IF NOT EXISTS" : "";

    auto columnsQueryStr = convertItemListToStr<decltype(columnsAndSettings.begin())>(
        columnsAndSettings.begin(), columnsAndSettings.end(), [](decltype(columnsAndSettings.begin()) it) {
            auto [column, setting] = *it;
            return column + "," + setting;
        });

    std::vector<std::string> columns, settings;

    for (auto& [column, setting] : columnsAndSettings) {
        columns.emplace_back(std::move(column));
        settings.emplace_back(std::move(setting));
    }

    std::string query =
        std::format("CREATE TABLE {} {} ({})", std::move(ifNotExistsStr), name, std::move(columnsAndSettings));

    char* errorMsg;
    int rc = sqlite3_exec(m_db, query.c_str(), nullptr, 0, &errorMsg);
    if (rc != SQLITE_OK) {
        std::cerr << std::format("Creating table {} failed: {}", name, errorMsg);
    }

    // Adding a new table
    m_tables.insert_or_assign(std::move(name), std::move(columns));
}

void Database::insertEntry(const std::string& tableName, const Entry& entry) {
    auto it = m_tables.find(tableName);
    if (it == m_tables.end()) {
        std::cerr << std::format("Table {} not found", tableName);
        return;
    }

    auto& [_, columns] = *it;

    std::string entryList = convertItemListToStr<decltype(entry.begin())>(
        entry.begin(), entry.end(), [](decltype(entry.begin()) it) { return *it; });

    std::string query = std::format("INSERT INTO {} VALUES ({})", tableName, std::move(entryList));
    char* errorMsg;
    int rc = sqlite3_exec(m_db, query.c_str(), nullptr, 0, &errorMsg);
    if (rc != SQLITE_OK) {
    }
}

} // namespace TodoList::Core
