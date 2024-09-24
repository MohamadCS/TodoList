#pragma once

#include "sqlite3.h"
#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace TodoList::Core {

using Table = std::vector<std::string>;
using Entry = std::vector<std::string>;
class Database {
public:
    Database(const std::string&, const std::filesystem::path& path);

    void newTable(const std::string& name, std::vector<std::pair<std::string, std::string>>&& columns,
                  bool ifNotExist = true);
    const Table& getTable(const std::string& name) const;

    void insertEntry(const std::string&, const Entry&);

private:
    std::string m_name;
    std::filesystem::path m_path;
    std::map<std::string, Table> m_tables;
    sqlite3* m_db;
};

}; // namespace TodoList::Core
