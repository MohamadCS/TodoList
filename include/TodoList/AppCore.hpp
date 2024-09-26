#pragma once

#include "Account.hpp"
#include "Database.hpp"

#include <chrono>
#include <cstdint>
#include <deque>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace TodoList::Core {

struct Task;
struct TaskList;
class AppCore;

using ID = std::int32_t;
using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

// TODO: Put in a seperate file.
class Task {
public:
    Task(const std::string& text, const std::string& desc, ID id, std::uint8_t checked,
         TaskList*,const std::optional<TimePoint>& duoDate, const std::optional<TimePoint>& deadLine);
    std::optional<TimePoint> getDuoDate() const;
    std::optional<TimePoint> getDeadLine() const;

    std::string getTaskDesc() const;
    std::string getTaskText() const;

    std::uint8_t isChecked() const;

    ID getId() const;

    TaskList* getTaskList() const;

    void setDuoDate(const std::optional<TimePoint>&);
    void setDeadLine(const std::optional<TimePoint>&);

    void setTaskDesc(const std::string&);
    void setTaskText(const std::string&);

    void setChecked(std::uint8_t);

    void setTaskList(TaskList*);

    void setId(ID);

private:
    ID m_taskId;

    std::uint8_t m_checked;

    std::string m_taskText;
    std::string m_taskDesc;

    TaskList* m_taskList;

    std::optional<TimePoint> m_duoDate;
    std::optional<TimePoint> m_deadLine;


    // std::map<ID, TaskList*> taskListsMap;
};

// TODO: Put in a seperate file.
class TaskList {
public:
    TaskList() = default;
    TaskList(ID id, const std::string& name, const std::vector<Task*>& tasks);
    std::string getName() const;
    const std::vector<Task*>& getTasks() const;
    std::vector<Task*>& getTasks();
    ID getId() const;

    void setId(ID newId);
    void setTasks(const std::vector<Task*> newTasks);
    void setName(const std::string& name);

private:
    ID m_taskListId;
    std::string m_name;
    std::vector<Task*> m_tasks;
};

class App {
public:
    inline static constexpr ID MAX_DEFAULT_ID = 10;

    static App& instance();

    Task* newTask(const std::string& taskText, const std::string& taskDesc, std::uint8_t checked, TaskList* taskListPtr,
                  bool addToDb = true, const std::optional<TimePoint>& duoDate = std::nullopt,
                  const std::optional<TimePoint>& deadLine = std::nullopt);

    TaskList* newTaskList(bool addToDB = true);
    TaskList* newTaskList(TaskList&& taskList, bool addToDB = true);

    void syncTask(const Task*);
    void syncProject(const TaskList*);

    bool login(const std::string& email, const std::string& password);
    bool signup(const Account&);
    void loadDatabases();

    const std::deque<std::unique_ptr<Task>>& getTasksList() const;
    const std::map<ID, std::unique_ptr<TaskList>>& getTaskLists() const;

    ID generateProjectId();
    ID getCurrentProjectId() const;

    void setCurrentProjectId(ID);

private:
    std::deque<std::unique_ptr<Task>> m_tasks;
    std::map<ID, std::unique_ptr<TaskList>> m_taskLists;

    Account m_currentAccount;

    Database m_tasksDb;
    Database m_accountsDb;

    ID m_currentProjectId; // TODO: Move this as a flag in TaskProjectComp
    ID m_taskIdCtr = 1;
    ID m_projectIdCtr = MAX_DEFAULT_ID + 1;

    App();
};

} // namespace TodoList::Core
