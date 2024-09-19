#pragma once

#include <chrono>
#include <cstdint>
#include <deque>
#include <memory>
#include <string>
#include <vector>

using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

struct Task;
struct TaskList;
class AppCore;

struct Task {
    TimePoint duoDate;
    TimePoint deadLine;
    std::string taskText;
    std::string taskDesc;
    bool checked;
    std::uint32_t taskId;
    std::vector<TaskList*> taskLists;
};

struct TaskList { // remo
    std::vector<Task*> tasks;
    std::string name;
};

class AppCore {
public:
    static AppCore& instance();

    Task* newTask(const TimePoint& duoDate, const TimePoint& deadLine, const std::string& taskText,
                  const std::string& taskDesc, bool checked, TaskList* taskListPtr);

    TaskList* newTaskList();
    TaskList* newTaskList(TaskList&& taskList);

    std::uint32_t generateProjectId();
    void setCurrentProjectId(std::uint32_t);
    std::uint32_t getCurrentProjectId() const;

private:
    std::deque<std::unique_ptr<Task>> m_tasks;
    std::deque<std::unique_ptr<TaskList>> m_taskLists;

    std::uint32_t m_currentProjectId;

    std::uint32_t m_taskIdCtr = 1;
    std::uint32_t m_projectIdCtr = 1;

    AppCore() = default;
};
