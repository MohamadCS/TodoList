#include "../include/TodoList/MainFrame.hpp"
#include "../include/TodoList/AppCore.hpp"
#include "../include/TodoList/TaskComp.hpp"

#include "wx/app.h"
#include "wx/checkbox.h"
#include "wx/log.h"
#include "wx/sizer.h"
#include <wx/button.h>
#include <wx/scrolwin.h>
#include <wx/statline.h>

void MainFrame::setup() {

    if (m_ready) {
        wxLogMessage("Already allocated main frame");
        wxExit();
    }

    setupSideBar();
    setupTaskPanel();

    Bind(EVT_CHANGE_PROJECT, &MainFrame::onProjectChange, this);

    m_mainBoxSizer = new wxBoxSizer(wxHORIZONTAL);

    m_mainBoxSizer->Add(m_sidebar.sidebarBoxSizer, wxSizerFlags().Proportion(0).Expand());
    m_mainBoxSizer->Add(m_taskPanel.taskPanelBoxSizer, wxSizerFlags().Proportion(2).Expand());

    test();

    m_sidebar.inboxProject->select(m_taskPanel.taskPanelBoxSizer);
    m_taskPanel.currentTaskCompList = m_sidebar.inboxProject;

    SetSizerAndFit(m_mainBoxSizer);
    SetClientSize(DEFAULT_FRAME_DIMS);
}

void MainFrame::setupSideBar() {

    auto& appCore = AppCore::instance();
    m_sidebar.sidebarBoxSizer = new wxBoxSizer(wxVERTICAL);
    m_sidebar.projectsBoxSizer = new wxBoxSizer(wxVERTICAL);
    m_sidebar.homeBoxSizer = new wxBoxSizer(wxVERTICAL);

    m_sidebar.sidebarBoxSizer->Add(m_sidebar.homeBoxSizer, wxSizerFlags().Proportion(0).Expand().FixedMinSize());
    m_sidebar.sidebarBoxSizer->Add(m_sidebar.projectsBoxSizer, wxSizerFlags().Proportion(2).Expand());

    auto* inboxList = new TaskCompList;
    auto* todayList = new TaskCompList;

    m_sidebar.inboxProject = new TaskProjectComp(this, wxID_ANY, inboxList, appCore.generateProjectId());
    m_sidebar.todayProject = new TaskProjectComp(this, wxID_ANY, todayList, appCore.generateProjectId());

    m_sidebar.homeBoxSizer->Add(m_sidebar.inboxProject, wxSizerFlags().Border(wxALL, 20));
    m_sidebar.homeBoxSizer->Add(m_sidebar.todayProject, wxSizerFlags().Border(wxALL, 20));

    m_sidebar.projectList.insert_or_assign(m_sidebar.inboxProject->projectId, m_sidebar.inboxProject);
    m_sidebar.projectList.insert_or_assign(m_sidebar.todayProject->projectId, m_sidebar.todayProject);

}

void MainFrame::setupTaskPanel() {
    auto& appCore = AppCore::instance();
    m_taskPanel.taskPanelBoxSizer = new wxBoxSizer(wxVERTICAL);

    if (m_taskPanel.currentTaskCompList == nullptr) {
        return;
    }
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

    for (int i = 0; i < 5; ++i) {
        auto* task = appCore.newTask({}, {}, "inbox task", "", false, inbox->taskList);
        inbox->addTask(task);
    }

    for (int i = 0; i < 5; ++i) {
        auto& today = m_sidebar.todayProject;
        auto* task2 = appCore.newTask({}, {}, " today task", "", false, inbox->taskList);
        today->addTask(task2);
    }
}
