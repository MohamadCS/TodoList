#include "../include/TodoList/MainFrame.hpp"
#include "../include/TodoList/AppCore.hpp"
#include "../include/TodoList/TaskComp.hpp"

#include "wx/app.h"
#include "wx/event.h"
#include "wx/log.h"
#include "wx/sizer.h"
#include "wx/window.h"
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/scrolwin.h>
#include <wx/statline.h>

void MainFrame::setup() {

    setupSideBar();
    setupTaskPanel();
    // test();
    m_sidebar.inboxProject->select(m_taskPanel.taskPanelBoxSizer);
    m_taskPanel.currentTaskCompList = m_sidebar.inboxProject;
}

void MainFrame::setupSideBar() {

    auto& appCore = AppCore::instance();

    m_sidebar.inboxProject = new TaskProjectComp(m_sidebar.homePanel, wxID_ANY, appCore.generateProjectId(), "Inbox");
    m_sidebar.todayProject = new TaskProjectComp(m_sidebar.homePanel, wxID_ANY, appCore.generateProjectId(), "Today");

    m_sidebar.homeBoxSizer->Add(m_sidebar.inboxProject);
    m_sidebar.homeBoxSizer->Add(m_sidebar.todayProject);

    m_sidebar.projectList.insert_or_assign(m_sidebar.inboxProject->projectId, m_sidebar.inboxProject);
    m_sidebar.projectList.insert_or_assign(m_sidebar.todayProject->projectId, m_sidebar.todayProject);
}

void MainFrame::setupTaskPanel() {
}

void MainFrame::onProjectChange(wxCommandEvent& ev) {
    auto* taskProjectCompPtr = static_cast<TaskProjectComp*>(ev.GetClientData());

    if (taskProjectCompPtr == nullptr) {
        wxLogDebug("Got nullptr");
        wxExit();
    }

    m_taskPanel.currentTaskCompList->unselect(m_taskPanel.taskPanelBoxSizer);
    m_taskPanel.currentTaskCompList = taskProjectCompPtr;
    m_taskPanel.currentTaskCompList->select(m_taskPanel.taskPanelBoxSizer);
    m_taskPanel.taskPanelBoxSizer->Layout();
}

void MainFrame::test() {
    auto& appCore = AppCore::instance();
    auto& inbox = m_sidebar.inboxProject;
    auto& today = m_sidebar.todayProject;

    for (int i = 0; i < 5; ++i) {
        auto* task = appCore.newTask({}, {}, "inbox task", "", false, inbox->taskListComp->m_taskList);
        inbox->addTask(task, m_taskPanel.taskPanel);
    }

    for (int i = 0; i < 5; ++i) {
        auto* task2 = appCore.newTask({}, {}, " today task", "", false, today->taskListComp->m_taskList);
        today->addTask(task2, m_taskPanel.taskPanel);
    }
}
