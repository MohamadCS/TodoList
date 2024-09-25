#include "../include/TodoList/AppCore.hpp"
#include "../include/TodoList/Defines.hpp"
#include "../include/TodoList/Utils.hpp"
#include <cstdlib>
#include <memory>
#include <optional>
#include <string>
#include <utility>

static TodoList::Core::TblEntry taskToTblEntry(const TodoList::Core::Task*);
static TodoList::Core::TblEntry projectToTblEntry(const TodoList::Core::TaskList*);

namespace TodoList::Core {

App::App()
    : m_tasks(),
      m_taskLists(),
      m_tasksDb(Utility::TASKS_DB_NAME,
                Utility::makePath({std::getenv("HOME"), "Library/Application Support/", Utility::TASKS_DB_NAME})) {

    std::vector<std::pair<std::string, std::string>> tasksTblCols = {
        {Utility::tasksColsToStr.at(Utility::TasksTblCols::ID), "INTEGER PRIMARY KEY"},
        {Utility::tasksColsToStr.at(Utility::TasksTblCols::TEXT), "TEXT"},
        {Utility::tasksColsToStr.at(Utility::TasksTblCols::PROJECT_ID), "INTEGER UNIQUE"}};

    std::vector<std::pair<std::string, std::string>> projectTblCols = {
        {Utility::projectsColsToStr.at(Utility::ProjectsTblCols::ID), "INTEGER PRIMARY KEY"},
        {Utility::projectsColsToStr.at(Utility::ProjectsTblCols::NAME), "TEXT"},
    };

    m_tasksDb.newTable(Utility::TASKS_TBL_NAME, std::move(tasksTblCols));
    m_tasksDb.newTable(Utility::PROJECTS_TBL_NAME, std::move(projectTblCols));
}
App& App::instance() {
    static App app;
    return app;
}

Task* App::newTask(const std::string& taskText, const std::string& taskDesc, bool checked, TaskList* pTaskList,
                   bool addToDb, const std::optional<TimePoint>& duoDate, const std::optional<TimePoint>& deadLine) {

    Task task = {duoDate, deadLine, taskText, taskDesc, checked, m_taskIdCtr++};
    auto taskUniquePtr = std::make_unique<Task>(std::move(task));

    auto* taskPtr = taskUniquePtr.get();
    taskPtr->taskList = pTaskList;
    if (addToDb) {
        m_tasksDb.insertEntry(Utility::TASKS_TBL_NAME, taskToTblEntry(taskPtr));
    }
    m_tasks.push_back(std::move(taskUniquePtr));
    return taskPtr;
}

TaskList* App::newTaskList(bool addToDb) {
    auto taskList = std::make_unique<TaskList>();
    auto* retVal = taskList.get();
    taskList->taskListId = generateProjectId();
    m_taskLists.push_back(std::move(taskList));
    if (addToDb) {
        m_tasksDb.insertEntry(Utility::PROJECTS_TBL_NAME, projectToTblEntry(retVal));
    }
    return retVal;
}

TaskList* App::newTaskList(TaskList&& taskList, bool addToDb) {
    auto taskListPtr = std::make_unique<TaskList>(std::move(taskList));
    auto* retVal = taskListPtr.get();
    m_taskLists.push_back(std::move(taskListPtr));
    if (addToDb) {
        m_tasksDb.insertEntry(Utility::PROJECTS_TBL_NAME, projectToTblEntry(retVal));
    }

    return retVal;
}

ID App::generateProjectId() {
    return m_projectIdCtr++;
}

void App::setCurrentProjectId(ID newProjectId) {
    m_currentProjectId = newProjectId;
}

ID App::getCurrentProjectId() const {
    return m_currentProjectId;
}

void App::syncTask(const Task* pTask) {

    if (pTask == nullptr) {
        LOG("Nullptr");
    }

    auto getCol = [](Utility::TasksTblCols col) { return Utility::tasksColsToStr.at(col); };

    m_tasksDb.updateEntry(Utility::TASKS_TBL_NAME,
                          {{getCol(Utility::TasksTblCols::ID), "=", std::to_string(pTask->taskId)}},
                          {{getCol(Utility::TasksTblCols::TEXT), std::format("\'{}\'", pTask->taskText)},
                           {getCol(Utility::TasksTblCols::PROJECT_ID), std::to_string(pTask->taskList->taskListId)}});
}

void App::syncProject(const TaskList* pTaskList) {

    if (pTaskList == nullptr) {
        LOG("Nullptr");
    }

    auto getCol = [](Utility::ProjectsTblCols col) { return Utility::projectsColsToStr.at(col); };

    m_tasksDb.updateEntry(Utility::PROJECTS_TBL_NAME,
                          {{getCol(Utility::ProjectsTblCols::ID), "=", std::to_string(pTaskList->taskListId)}},
                          {
                              {getCol(Utility::ProjectsTblCols::NAME), std::format("\'{}\'", pTaskList->name)},
                          });
}

void App::loadDatabases() {
    // load projects
    std::string query = std::format("SELECT * FROM {}", TodoList::Utility::PROJECTS_TBL_NAME);
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(m_tasksDb.getDb(), query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        LOG("Error: {}", sqlite3_errmsg(m_tasksDb.getDb()));
        exit(0);
    }

    std::map<ID, TaskList*> idToTaskList;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int projectId = sqlite3_column_int(stmt, 0);
        const char* projectName = (const char*)sqlite3_column_text(stmt, 1);

        auto* pTaskList = newTaskList(false);
        pTaskList->name = projectName;
        pTaskList->taskListId = projectId;
        idToTaskList.insert_or_assign(projectId, pTaskList);
    }
    sqlite3_finalize(stmt);

    query = std::format("SELECT * FROM {}", TodoList::Utility::TASKS_TBL_NAME);
    sqlite3_stmt* stmt2;

    if (sqlite3_prepare_v2(m_tasksDb.getDb(), query.c_str(), -1, &stmt2, nullptr) != SQLITE_OK) {
        LOG("Error: {}", sqlite3_errmsg(m_tasksDb.getDb()));
        exit(0);
    }

    while (sqlite3_step(stmt2) == SQLITE_ROW) {
        int taskId = sqlite3_column_int(stmt2, 0);
        const char* taskText = (const char*)sqlite3_column_text(stmt2, 1);
        int taskProjectId = sqlite3_column_int(stmt2, 2);
        auto* pTask = newTask(taskText, "", false, idToTaskList.at(taskProjectId), false);
    }

    for (int i = 0; i < m_taskLists.size(); ++i) {
        LOG("{}", m_taskLists[i].get()->name);
    }

    for (int i = 0; i < m_tasks.size(); ++i) {
        LOG("{}", m_tasks[i].get()->taskText);
    }

    sqlite3_finalize(stmt2);
}

const std::deque<std::unique_ptr<Task>>& App::getTasksList() const {
    return m_tasks;
}
const std::deque<std::unique_ptr<TaskList>>& App::getTaskLists() const {
    return m_taskLists;
}

} // namespace TodoList::Core

static TodoList::Core::TblEntry taskToTblEntry(const TodoList::Core::Task* pTask) {
    if (pTask == nullptr) {
        LOG("Nullptr");
    }

    auto getCol = [](TodoList::Utility::TasksTblCols col) { return TodoList::Utility::tasksColsToStr.at(col); };

    TodoList::Core::TblEntry entry = {
        {getCol(TodoList::Utility::TasksTblCols::ID), std::to_string(pTask->taskId)},
        {getCol(TodoList::Utility::TasksTblCols::TEXT), std::format("\'{}\'", pTask->taskText)},
        {getCol(TodoList::Utility::TasksTblCols::PROJECT_ID), std::to_string(pTask->taskList->taskListId)},
    };

    return entry;
}

static TodoList::Core::TblEntry projectToTblEntry(const TodoList::Core::TaskList* pTaskList) {
    if (pTaskList == nullptr) {
        LOG("Nullptr");
    }

    auto getCol = [](TodoList::Utility::ProjectsTblCols col) { return TodoList::Utility::projectsColsToStr.at(col); };

    TodoList::Core::TblEntry entry = {
        {getCol(TodoList::Utility::ProjectsTblCols::ID), std::to_string(pTaskList->taskListId)},
        {getCol(TodoList::Utility::ProjectsTblCols::NAME), std::format("\'{}\'", pTaskList->name)},
    };

    return entry;
}
