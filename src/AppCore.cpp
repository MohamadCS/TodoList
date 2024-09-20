#include "../include/TodoList/AppCore.hpp"
#include <iostream>
#include <memory>
#include <utility>

namespace TodoList::Core {

App& App::instance() {
    static App app;
    return app;
}

Task* App::newTask(const TimePoint& duoDate, const TimePoint& deadLine, const std::string& taskText,
                    const std::string& taskDesc, bool checked, TaskList* taskList) {

    if (taskList == nullptr) {
        std::cerr << "Task Must Be in at least one task list" << '\n';
    }

    Task task = {duoDate, deadLine, taskText, taskDesc, checked, m_taskIdCtr++};
    auto taskPtr = std::make_unique<Task>(std::move(task));
    auto* retVal = taskPtr.get();
    m_tasks.push_back(std::move(taskPtr));
    return retVal;
}

TaskList* App::newTaskList() {
    auto taskList = std::make_unique<TaskList>();
    auto* retVal = taskList.get();
    taskList->taskListId = generateProjectId();
    m_taskLists.push_back(std::move(taskList));
    return retVal;
}

TaskList* App::newTaskList(TaskList&& taskList) {
    auto taskListPtr = std::make_unique<TaskList>(std::move(taskList));
    auto* retVal = taskListPtr.get();
    m_taskLists.push_back(std::move(taskListPtr));
    return retVal;
}

std::uint32_t App::generateProjectId() {
    return m_projectIdCtr++;
}

void App::setCurrentProjectId(std::uint32_t newProjectId) {
    m_currentProjectId = newProjectId;
}

std::uint32_t App::getCurrentProjectId() const {
    return m_currentProjectId;
}
} // namespace TodoList::AppCore
