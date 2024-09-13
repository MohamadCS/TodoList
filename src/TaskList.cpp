#include "../include/TodoList/TaskList.hpp"

TaskList::TaskList(const std::wstring& listName,
                   const std::vector<Task> tasksVec)
    : m_listName(listName),
      m_tasksVec(tasksVec) {
}
