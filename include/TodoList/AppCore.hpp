#pragma once

#include "Database.hpp"
#include "Account.hpp"

#include <chrono>
#include <cstdint>
#include <deque>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace TodoList::Core {

struct Task;
struct TaskList;
class AppCore;

using ID = std::int32_t;
using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

// TODO: Put in a seperate file.
struct Task {
    std::optional<TimePoint> duoDate;
    std::optional<TimePoint> deadLine;

    std::string taskText;
    std::string taskDesc;

    std::uint8_t checked;
    ID taskId;

    TaskList* taskList;
    // std::map<ID, TaskList*> taskListsMap;
};

// TODO: Put in a seperate file.
struct TaskList {
    std::vector<Task*> tasks;
    std::string name;
    ID taskListId;
};

class App {
public:
    inline static constexpr ID MAX_DEFAULT_ID = 10;

    static App& instance();

    Task* newTask(const std::string& taskText, const std::string& taskDesc, bool checked, TaskList* taskListPtr,
                  bool addToDb = true, const std::optional<TimePoint>& duoDate = std::nullopt,
                  const std::optional<TimePoint>& deadLine = std::nullopt);

    TaskList* newTaskList(bool addToDB = true);
    TaskList* newTaskList(TaskList&& taskList, bool addToDB = true);

    void syncTask(const Task*);
    void syncProject(const TaskList*);
    bool login(const std::string& email, const std::string& password);
    bool signup(const Account&);
    void loadDatabases();

    const std::deque<std::unique_ptr<Task>>& getTasksList() const;
    const std::map<ID,std::unique_ptr<TaskList>>& getTaskLists() const;

    ID generateProjectId();
    ID getCurrentProjectId() const;

    void setCurrentProjectId(ID);

private:
    std::deque<std::unique_ptr<Task>> m_tasks;
    std::map<ID,std::unique_ptr<TaskList>> m_taskLists;

    Account m_currentAccount;

    Database m_tasksDb;
    Database m_accountsDb;

    ID m_currentProjectId; // TODO: Move this as a flag in TaskProjectComp
    ID m_taskIdCtr = 1;
    ID m_projectIdCtr = MAX_DEFAULT_ID + 1;

    App();
};

} // namespace TodoList::Core
