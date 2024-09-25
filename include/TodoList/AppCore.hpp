#pragma once

#include "Database.hpp"

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
    static App& instance();

    Task* newTask(const std::string& taskText, const std::string& taskDesc, bool checked, TaskList* taskListPtr,
                  bool addToDb = true, const std::optional<TimePoint>& duoDate = std::nullopt,
                  const std::optional<TimePoint>& deadLine = std::nullopt);

    TaskList* newTaskList(bool addToDB = true);
    TaskList* newTaskList(TaskList&& taskList, bool addToDB = true);

    void syncTask(const Task*);
    void syncProject(const TaskList*);
    void loadDatabases();

    const std::deque<std::unique_ptr<Task>>& getTasksList() const;
    const std::deque<std::unique_ptr<TaskList>>& getTaskLists() const;

    ID generateProjectId();
    ID getCurrentProjectId() const;

    void setCurrentProjectId(ID);

private:
    std::deque<std::unique_ptr<Task>> m_tasks;
    std::deque<std::unique_ptr<TaskList>> m_taskLists;

    Database m_tasksDb;

    ID m_currentProjectId; // TODO: Move this as a flag in TaskProjectComp
    ID m_taskIdCtr = 1;
    ID m_projectIdCtr = 1;

    App();
};

} // namespace TodoList::Core
