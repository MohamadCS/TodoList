#pragma once

#include <chrono>
#include <cstdint>
#include <deque>
#include <map>
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
                  const std::optional<TimePoint>& duoDate = std::nullopt,
                  const std::optional<TimePoint>& deadLine = std::nullopt);

    TaskList* newTaskList();
    TaskList* newTaskList(TaskList&& taskList);

    ID generateProjectId();
    ID getCurrentProjectId() const;

    void setCurrentProjectId(ID);

private:
    std::deque<std::unique_ptr<Task>> m_tasks;
    std::deque<std::unique_ptr<TaskList>> m_taskLists;

    ID m_currentProjectId; // TODO: Move this as a flag in TaskProjectComp
    ID m_taskIdCtr = 1;
    ID m_projectIdCtr = 1;

    App() = default;
};

} // namespace TodoList::Core
