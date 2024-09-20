#pragma once

#include <chrono>
#include <cstdint>
#include <deque>
#include <memory>
#include <string>
#include <vector>

namespace TodoList::Core {

struct Task;
struct TaskList;
class AppCore;

using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

struct Task {
    TimePoint duoDate;
    TimePoint deadLine;

    std::string taskText;
    std::string taskDesc;

    std::uint8_t checked;
    std::uint32_t taskId;

    std::vector<TaskList*> taskLists;
};

struct TaskList {
    std::vector<Task*> tasks;
    std::string name;
    std::uint32_t taskListId;
};

class App {
public:
    static App& instance();

    Task* newTask(const TimePoint& duoDate, const TimePoint& deadLine, const std::string& taskText,
                  const std::string& taskDesc, bool checked, TaskList* taskListPtr);

    TaskList* newTaskList();
    TaskList* newTaskList(TaskList&& taskList);

    std::uint32_t generateProjectId();
    std::uint32_t getCurrentProjectId() const;

    void setCurrentProjectId(std::uint32_t);

private:
    std::deque<std::unique_ptr<Task>> m_tasks;
    std::deque<std::unique_ptr<TaskList>> m_taskLists;

    std::uint32_t m_currentProjectId; // TODO: Move this as a flag in TaskProjectComp

    std::uint32_t m_taskIdCtr = 1;
    std::uint32_t m_projectIdCtr = 1;

    App() = default;
};

} // namespace TodoList::Core
