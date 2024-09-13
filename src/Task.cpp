
#include "../include/TodoList/Task.hpp"
Task::Task(const TimePoint& duoDate, const TimePoint& deadLine,
           const std::wstring& taskText, const std::wstring& taskDesc,
           bool checked)
    : m_duoDate(duoDate),
      m_deadLine(deadLine),
      m_taskText(taskText),
      m_taskDesc(taskDesc),
        m_checked(checked){
}
