#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "Task.hpp"

class TaskList {
public:
    TaskList(const std::wstring& listName, const std::vector<Task> tasks = {});

    std::optional<Task&> getTaskFromIdx(uint32_t idx);
    std::vector<int>& getTasksVec();
    const std::vector<int>& getTVec() const;

private:
    std::wstring m_listName;
    std::vector<Task> m_tasksVec;
};
