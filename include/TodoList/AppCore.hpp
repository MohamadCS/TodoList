#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <deque>
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

struct TaskList{
    std::vector<Task*> tasks;
};

class AppCore {
public:
    static AppCore& instance();

    Task *newTask(const TimePoint &duoDate, const TimePoint &deadLine,
                  const std::string &taskText, const std::string &taskDesc,
                  bool checked);

  private:
    std::deque<std::unique_ptr<Task>> m_tasks;
    inline static std::uint32_t m_taskIdCtr = 1;

    AppCore() = default;
};

