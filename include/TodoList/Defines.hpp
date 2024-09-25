#pragma once
#include "AppCore.hpp"
#include <cstdlib>
#include <map>

namespace TodoList::Utility {

constexpr char TASKS_DB_NAME[] = "todolistapp";
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

const std::map<TasksTblCols, std::string> tasksColsToStr = {
    {TasksTblCols::ID, "id"},
    {TasksTblCols::TEXT, "text"},
    {TasksTblCols::PROJECT_ID, "project_id"},
};

const std::map<ProjectsTblCols, std::string> projectsColsToStr = {
    {ProjectsTblCols::ID, "id"},
    {ProjectsTblCols::NAME, "name"},
};

enum class Views {
    MAIN_FRAME = 0,
    SETTINGS_FRAME,
    LOGIN_FRAME
};

// Project idx
constexpr Core::ID TODAY_IDX = 0;
constexpr Core::ID INBOX_IDX = 1;

} // namespace TodoList::Utility
