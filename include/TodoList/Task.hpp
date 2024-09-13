#pragma once

#include <chrono>
#include <string>

using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

class Task {
public:
    Task(const TimePoint& duoDate, const TimePoint& deadLine,
         const std::wstring& taskText, const std::wstring& taskDesc,
         bool checked);

    void setDutDate(const TimePoint&);
    void setDeadLine(const TimePoint&);
    void setTaskText(const std::wstring&);
    void setTaskDesc(const std::wstring&);
    void setCheck(bool state);

private:
    TimePoint m_duoDate;
    TimePoint m_deadLine;
    std::wstring m_taskText;
    std::wstring m_taskDesc;
    bool m_checked;
    // attachments
    // std::vector<Task*>;
    // Task* m_parentTask;
};
