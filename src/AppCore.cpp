#include "../include/TodoList/AppCore.hpp"
#include <memory>

AppCore& AppCore::instance() {
    static AppCore app;
    return app;
}

Task* AppCore::newTask(const TimePoint& duoDate, const TimePoint& deadLine,
                       const std::string& taskText, const std::string& taskDesc,
                       bool checked) {
    Task task = {duoDate, deadLine, taskText, taskDesc, checked, m_taskIdCtr++};
    auto taskPtr = std::make_unique<Task>(std::move(task));
    m_tasks.push_back(std::move(taskPtr));

    return m_tasks.back().get();
}
