#pragma once
#include "AppCore.hpp"
#include <cstdlib>
#include <map>

namespace TodoList::Utility {

constexpr char APP_SUPPORT_FOLDER[] = "todolistapp";
constexpr char ACCOUNTS_DB_NAME[] = "accounts";
constexpr char ACCOUNTS_TBL_NAME[] = "Accounts";
constexpr char TASKS_TBL_NAME[] = "Tasks";
constexpr char PROJECTS_TBL_NAME[] = "Projects";

enum class TasksTblCols {
    ID = 0,
    TEXT,
    PROJECT_ID
};

enum class ProjectsTblCols {
    ID = 0,
    NAME,
};

enum class AccountTblCols {
    ID = 0,
    FIRST_NAME,
    LAST_NAME,
    EMAIL,
    PASSWORD
};

const std::map<TasksTblCols, std::string> tasksColsToStr = {
    {TasksTblCols::ID, "id"},
    {TasksTblCols::TEXT, "text"},
    {TasksTblCols::PROJECT_ID, "project_id"},
};

const std::map<ProjectsTblCols, std::string> projectsColsToStr = {
    {ProjectsTblCols::ID, "id"},
    {ProjectsTblCols::NAME, "name"},
};

const std::map<AccountTblCols, std::string> accountsColToStr = {
    {AccountTblCols::ID, "id"},
    {AccountTblCols::EMAIL, "email"},
    {AccountTblCols::FIRST_NAME, "first_name"},
    {AccountTblCols::LAST_NAME, "last_name"},
    {AccountTblCols::PASSWORD, "password"},
};

enum class Views {
    MAIN_FRAME = 0,
    SETTINGS_FRAME,
    LOGIN_FRAME,
    SIGNUP_FRAME,
};

// Project idx
constexpr Core::ID TODAY_IDX = 0;
constexpr Core::ID INBOX_IDX = 1;

} // namespace TodoList::Utility
