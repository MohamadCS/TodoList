#include "../include/TodoList/AppCore.hpp"
#include "../include/TodoList/Defines.hpp"
#include "../include/TodoList/Utils.hpp"
#include <assert.h>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <optional>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <utility>

static TodoList::Core::TblEntry taskToTblEntry(const TodoList::Core::Task*);
static TodoList::Core::TblEntry projectToTblEntry(const TodoList::Core::TaskList*);
static TodoList::Core::TblEntry accountToTblEntry(const TodoList::Core::Account&);

namespace TodoList::Core {

App::App()
    : m_tasks(),
      m_taskLists() {

    m_accountsDb =
        Database(Utility::ACCOUNTS_DB_NAME,
                 Utility::makePath({std::getenv("HOME"), "Library/Application Support/", Utility::APP_SUPPORT_FOLDER}));

    std::vector<std::pair<std::string, std::string>> accountTblCols = {
        {Utility::accountsColToStr.at(Utility::AccountTblCols::ID), "INTEGER PRIMARY KEY AUTOINCREMENT"},
        {Utility::accountsColToStr.at(Utility::AccountTblCols::FIRST_NAME), "TEXT"},
        {Utility::accountsColToStr.at(Utility::AccountTblCols::LAST_NAME), "TEXT"},
        {Utility::accountsColToStr.at(Utility::AccountTblCols::EMAIL), "TEXT UNIQUE"},
        {Utility::accountsColToStr.at(Utility::AccountTblCols::PASSWORD), "TEXT"}};

    m_accountsDb.newTable(Utility::ACCOUNTS_TBL_NAME, std::move(accountTblCols));
}
App& App::instance() {
    static App app;
    return app;
}

Task* App::newTask(const std::string& taskText, const std::string& taskDesc, std::uint8_t checked, TaskList* pTaskList,
                   bool addToDb, const std::optional<TimePoint>& duoDate, const std::optional<TimePoint>& deadLine) {

    Task task = Task(taskText, taskDesc, m_taskIdCtr++, checked, pTaskList, duoDate, deadLine);
    auto taskUniquePtr = std::make_unique<Task>(std::move(task));
    auto* taskPtr = taskUniquePtr.get();
    taskPtr->setTaskList(pTaskList);
    if (addToDb) {
        m_tasksDb.insertEntry(Utility::TASKS_TBL_NAME, taskToTblEntry(taskPtr));
    }
    m_tasks.push_back(std::move(taskUniquePtr));
    return taskPtr;
}

TaskList* App::newTaskList(bool addToDb) {
    auto pTaskList = std::make_unique<TaskList>();
    auto* retVal = pTaskList.get();
    pTaskList->setId(generateProjectId());
    m_taskLists.insert_or_assign(retVal->getId(), std::move(pTaskList));
    if (addToDb) {
        m_tasksDb.insertEntry(Utility::PROJECTS_TBL_NAME, projectToTblEntry(retVal));
        LOG("Adding Project to DB");
    }
    return retVal;
}

TaskList* App::newTaskList(TaskList&& taskList, bool addToDb) {
    auto pTaskList = std::make_unique<TaskList>(std::move(taskList));
    auto* retVal = pTaskList.get();
    m_taskLists.insert_or_assign(retVal->getId(), std::move(pTaskList));
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
                          {{getCol(Utility::TasksTblCols::ID), "=", std::to_string(pTask->getId())}},
                          {{getCol(Utility::TasksTblCols::TEXT), std::format("\'{}\'", pTask->getTaskText())},
                           {getCol(Utility::TasksTblCols::PROJECT_ID), std::to_string(pTask->getTaskList()->getId())}});
}

void App::syncProject(const TaskList* pTaskList) {

    if (pTaskList == nullptr) {
        LOG("Nullptr");
    }

    auto getCol = [](Utility::ProjectsTblCols col) { return Utility::projectsColsToStr.at(col); };

    m_tasksDb.updateEntry(Utility::PROJECTS_TBL_NAME,
                          {{getCol(Utility::ProjectsTblCols::ID), "=", std::to_string(pTaskList->getId())}},
                          {
                              {getCol(Utility::ProjectsTblCols::NAME), std::format("\'{}\'", pTaskList->getName())},
                          });
}

void App::loadDatabases() {

    m_tasksDb =
        Database(m_currentAccount.getEmail(), Utility::makePath({std::getenv("HOME"), "Library/Application Support/",
                                                                 Utility::APP_SUPPORT_FOLDER, "Databases"}));

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

    std::string query = std::format("SELECT * FROM {}", TodoList::Utility::PROJECTS_TBL_NAME);
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(m_tasksDb.getDb(), query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        LOG("Error: {}", sqlite3_errmsg(m_tasksDb.getDb()));
        exit(0);
    }

    TaskList* pTodayList = nullptr;
    TaskList* pInboxList = nullptr;

    auto todayList = TaskList(Utility::TODAY_IDX, "Today", {});
    syncProject(newTaskList(std::move(todayList), true));

    auto inboxList = TaskList(Utility::INBOX_IDX, "Inbox", {});
    syncProject(newTaskList(std::move(inboxList), true));

    int i = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int projectId = sqlite3_column_int(stmt, 0);

        if (projectId <= MAX_DEFAULT_ID) {
            continue;
        }

        const char* projectName = (const char*)sqlite3_column_text(stmt, 1);
        LOG("Loading Project {}", projectName);
        auto* pTaskList = newTaskList(false);
        pTaskList->setName(projectName);
        pTaskList->setId(projectId);
        ++i;
    }

    LOG("Added {} Projects", i);

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
        LOG("ProjectId {}", taskProjectId);
        LOG("Loading Task {}", taskId);

        if (m_taskLists.find(taskProjectId) == m_taskLists.end()) {
            LOG("Task List does not exists");
        }

        auto* pTask = newTask(taskText, "", false, m_taskLists.at(taskProjectId).get(), false);
        LOG("Task Loaded");
    }

    LOG("Finished Loading Database");

    sqlite3_finalize(stmt2);
}

const std::deque<std::unique_ptr<Task>>& App::getTasksList() const {
    return m_tasks;
}
const std::map<ID, std::unique_ptr<TaskList>>& App::getTaskLists() const {
    return m_taskLists;
}

// PERF: Fix this naive impl.
bool App::login(const std::string& loginEmail, const std::string& loginPassword) {

    std::string query = std::format("SELECT * FROM {}", TodoList::Utility::ACCOUNTS_TBL_NAME);
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(m_accountsDb.getDb(), query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        LOG("Error: {}", sqlite3_errmsg(m_tasksDb.getDb()));
        exit(0);
    }

    std::string firstName, lastName;

    bool found = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const auto* email = (const char*)(sqlite3_column_text(stmt, static_cast<int>(Utility::AccountTblCols::EMAIL)));
        if (email == loginEmail) {
            const auto* password =
                (const char*)(sqlite3_column_text(stmt, static_cast<int>(Utility::AccountTblCols::PASSWORD)));
            if (password == loginPassword) {
                found = true;
                firstName =
                    (const char*)(sqlite3_column_text(stmt, static_cast<int>(Utility::AccountTblCols::FIRST_NAME)));
                lastName =
                    (const char*)(sqlite3_column_text(stmt, static_cast<int>(Utility::AccountTblCols::LAST_NAME)));
                break;
            }
        }
    }

    if (!found) {
        LOG("Account with email {} and password {} not found", loginEmail,
            loginPassword); // TODO: Seperate error messages for each wrong entry.
        return false;
    }

    m_currentAccount = Account(firstName, lastName, loginEmail, loginPassword);
    sqlite3_finalize(stmt);
    return true;
}

bool App::signup(const Account& account) {

    std::string query = std::format("SELECT * FROM {}", TodoList::Utility::ACCOUNTS_TBL_NAME);

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_accountsDb.getDb(), query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        LOG("Error: {}", sqlite3_errmsg(m_tasksDb.getDb()));
        exit(0);
    }

    const auto& accountEmail = account.getEmail();

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const auto* email = (const char*)(sqlite3_column_text(stmt, static_cast<int>(Utility::AccountTblCols::EMAIL)));
        if (email == accountEmail) {
            LOG("Email {} already exists", accountEmail);
            sqlite3_finalize(stmt);
            return false;
        }
    }
    sqlite3_finalize(stmt);

    m_accountsDb.insertEntry(Utility::ACCOUNTS_TBL_NAME, accountToTblEntry(account));

    return true;
}

} // namespace TodoList::Core

static TodoList::Core::TblEntry taskToTblEntry(const TodoList::Core::Task* pTask) {
    if (pTask == nullptr) {
        LOG("Nullptr");
    }

    auto getCol = [](TodoList::Utility::TasksTblCols col) { return TodoList::Utility::tasksColsToStr.at(col); };

    return TodoList::Core::TblEntry{
        {getCol(TodoList::Utility::TasksTblCols::ID), std::to_string(pTask->getId())},
        {getCol(TodoList::Utility::TasksTblCols::TEXT), std::format("\'{}\'", pTask->getTaskText())},
        {getCol(TodoList::Utility::TasksTblCols::PROJECT_ID), std::to_string(pTask->getTaskList()->getId())},
    };
}

static TodoList::Core::TblEntry projectToTblEntry(const TodoList::Core::TaskList* pTaskList) {
    if (pTaskList == nullptr) {
        LOG("Nullptr");
    }

    auto getCol = [](TodoList::Utility::ProjectsTblCols col) { return TodoList::Utility::projectsColsToStr.at(col); };

    return TodoList::Core::TblEntry{
        {getCol(TodoList::Utility::ProjectsTblCols::ID), std::to_string(pTaskList->getId())},
        {getCol(TodoList::Utility::ProjectsTblCols::NAME), std::format("\'{}\'", pTaskList->getName())},
    };
}

static TodoList::Core::TblEntry accountToTblEntry(const TodoList::Core::Account& account) {
    auto getCol = [](TodoList::Utility::AccountTblCols col) { return TodoList::Utility::accountsColToStr.at(col); };
    return TodoList::Core::TblEntry{
        {getCol(TodoList::Utility::AccountTblCols::FIRST_NAME), std::format("\'{}\'", account.getFirstName())},
        {getCol(TodoList::Utility::AccountTblCols::LAST_NAME), std::format("\'{}\'", account.getLastName())},
        {getCol(TodoList::Utility::AccountTblCols::EMAIL), std::format("\'{}\'", account.getEmail())},
        {getCol(TodoList::Utility::AccountTblCols::PASSWORD), std::format("\'{}\'", account.getPassword())},
    };
}

namespace TodoList::Core {

Task::Task(const std::string& text, const std::string& desc, ID id, std::uint8_t checked, TaskList* pTaskList,
           const std::optional<TimePoint>& duoDate, const std::optional<TimePoint>& deadLine)
    : m_taskId(id),
      m_checked(checked),
      m_taskText(text),
      m_taskDesc(desc),
      m_taskList(pTaskList),
      m_duoDate(duoDate),
      m_deadLine(deadLine) {
}

std::optional<TimePoint> Task::getDuoDate() const {
    return m_duoDate;
}
std::optional<TimePoint> Task::getDeadLine() const {
    return m_deadLine;
}

std::string Task::getTaskDesc() const {
    return m_taskDesc;
}
std::string Task::getTaskText() const {
    return m_taskText;
}

std::uint8_t Task::isChecked() const {
    return m_checked;
}

ID Task::getId() const {
    return m_taskId;
}

TaskList* Task::getTaskList() const {
    return m_taskList;
}

void Task::setDuoDate(const std::optional<TimePoint>& tp) {
    m_duoDate = tp;
    App::instance().syncTask(this);
}
void Task::setDeadLine(const std::optional<TimePoint>& tp) {
    m_deadLine = tp;
    App::instance().syncTask(this);
}

void Task::setTaskDesc(const std::string& taskDesc) {
    m_taskDesc = taskDesc;
    App::instance().syncTask(this);
}
void Task::setTaskText(const std::string& taskText) {
    m_taskText = taskText;
    App::instance().syncTask(this);
}

void Task::setChecked(std::uint8_t checked) {
    m_checked = checked;
    App::instance().syncTask(this);
}

void Task::setTaskList(TaskList* pTaskList) {
    m_taskList = pTaskList;
    App::instance().syncTask(this);
}

void Task::setId(ID id) {
    m_taskId = id;
    App::instance().syncTask(this);
}

TaskList::TaskList(ID id, const std::string& name, const std::vector<Task*>& tasks)
    : m_taskListId(id),
      m_name(name),
      m_tasks(tasks) {
}

std::string TaskList::getName() const {
    return m_name;
}
const std::vector<Task*>& TaskList::getTasks() const {
    return m_tasks;
}
std::vector<Task*>& TaskList::getTasks() {
    return m_tasks;
}
ID TaskList::getId() const {
    return m_taskListId;
}

void TaskList::setId(ID newId) {
    m_taskListId = newId;
    App::instance().syncProject(this);
}
void TaskList::setTasks(const std::vector<Task*> newTasks) {
    m_tasks = newTasks;
    App::instance().syncProject(this);
}
void TaskList::setName(const std::string& name) {
    m_name = name;
    App::instance().syncProject(this);
}

} // namespace TodoList::Core
