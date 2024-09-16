#include "../include/TodoList/AppCore.hpp"
#include <memory>
#include <utility>

AppCore& AppCore::instance() {
    static AppCore app;
    return app;
}

Task* AppCore::newTask(const TimePoint& duoDate, const TimePoint& deadLine, const std::string& taskText,
                       const std::string& taskDesc, bool checked, TaskList* taskList) {

    Task task = {duoDate, deadLine, taskText, taskDesc, checked, m_taskIdCtr++};
    auto taskPtr = std::make_unique<Task>(std::move(task));
    m_tasks.push_back(std::move(taskPtr));
    return m_tasks.back().get();
}

TaskList* AppCore::newTaskList() {
    m_taskLists.push_back(std::make_unique<TaskList>());
    return m_taskLists.back().get();
}

TaskList* AppCore::newTaskList(TaskList&& taskList) {
    m_taskLists.push_back(std::make_unique<TaskList>(std::move(taskList)));
    return m_taskLists.back().get();
}

std::uint32_t AppCore::generateProjectId(){
    return m_projectIdCtr++;
}

void AppCore::setCurrentProjectId(std::uint32_t newProjectId){
    m_currentProjectId = newProjectId;
}

std::uint32_t AppCore::getCurrentProjectId() const{
    return m_currentProjectId;
}

