#include "../include/TodoList/AppCore.hpp"
#include <memory>
#include <optional>
#include <utility>

namespace TodoList::Core {

App& App::instance() {
    static App app;
    return app;
}

Task* App::newTask(const std::string& taskText, const std::string& taskDesc, bool checked, TaskList* pTaskList,
                   const std::optional<TimePoint>& duoDate, const std::optional<TimePoint>& deadLine) {

    Task task = {duoDate, deadLine, taskText, taskDesc, checked, m_taskIdCtr++};
    auto taskUniquePtr = std::make_unique<Task>(std::move(task));

    auto* taskPtr = taskUniquePtr.get();
    taskPtr->taskList = pTaskList;
    m_tasks.push_back(std::move(taskUniquePtr));
    return taskPtr;
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

ID App::generateProjectId() {
    return m_projectIdCtr++;
}

void App::setCurrentProjectId(ID newProjectId) {
    m_currentProjectId = newProjectId;
}

ID App::getCurrentProjectId() const {
    return m_currentProjectId;
}



} // namespace TodoList::Core
//
