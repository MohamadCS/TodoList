#include "../include/TodoList/MainFrame.hpp"
#include "../include/TodoList/AppCore.hpp"
#include "../include/TodoList/TaskComp.hpp"

#include "wx/app.h"
#include "wx/event.h"
#include "wx/log.h"
#include "wx/osx/stattext.h"
#include "wx/sizer.h"
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/scrolwin.h>
#include <wx/statline.h>

void MainFrame::setup() {

    setupSideBar();
    setupTaskPanel();
    test();
    setProject(m_sidebar.inboxProject);
}

void MainFrame::setupSideBar() {

    auto& appCore = AppCore::instance();

    m_sidebar.inboxProject = new TaskProjectComp(m_sidebar.homePanel, wxID_ANY, appCore.generateProjectId(), "Inbox");
    m_sidebar.todayProject = new TaskProjectComp(m_sidebar.homePanel, wxID_ANY, appCore.generateProjectId(), "Today");

    m_sidebar.homeBoxSizer->Add(m_sidebar.inboxProject, wxSizerFlags(0).FixedMinSize().Border(wxALL,10).Expand());
    m_sidebar.homeBoxSizer->Add(m_sidebar.todayProject, wxSizerFlags(0).FixedMinSize().Border(wxALL,10).Expand());

    m_sidebar.projectList.insert_or_assign(m_sidebar.inboxProject->projectId, m_sidebar.inboxProject);
    m_sidebar.projectList.insert_or_assign(m_sidebar.todayProject->projectId, m_sidebar.todayProject);

    m_sidebar.addProjectButton = new wxButton(m_sidebar.homePanel, wxID_ANY, "Add Project");
    m_sidebar.homeBoxSizer->AddSpacer(10);
    m_sidebar.homeBoxSizer->Add(m_sidebar.addProjectButton, wxSizerFlags().CenterHorizontal());
    m_sidebar.homeBoxSizer->AddSpacer(10);
}

void MainFrame::setupTaskPanel() {
}

void MainFrame::onProjectChange(wxCommandEvent& ev) {
    auto* taskProjectCompPtr = static_cast<TaskProjectComp*>(ev.GetClientData());

    if (taskProjectCompPtr == nullptr) {
        wxLogDebug("Nullptr");
    }

    setProject(taskProjectCompPtr);
}

void MainFrame::test() {
    auto& appCore = AppCore::instance();
    auto& inbox = m_sidebar.inboxProject;
    auto& today = m_sidebar.todayProject;

    for (int i = 0; i < 10; ++i) {
        auto* task = appCore.newTask({}, {}, "inbox task", "", false, inbox->taskListComp->m_taskList);
        inbox->addTask(task, m_taskPanel.bottomPanel);
    }

    for (int i = 0; i < 10; ++i) {
        auto* task2 = appCore.newTask({}, {}, " today task", "", false, today->taskListComp->m_taskList);
        today->addTask(task2, m_taskPanel.bottomPanel);
    }
}

void MainFrame::addTaskPanel() {
    wxLogDebug("Adding Task Panel");
    m_taskPanel.taskPanelBoxSizer = new wxBoxSizer(wxVERTICAL);
    m_taskPanel.topBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    m_taskPanel.bottomBoxSizer = new wxBoxSizer(wxVERTICAL);

    m_taskPanel.taskPanel = new wxPanel(this);
    m_taskPanel.topPanel = new wxPanel(m_taskPanel.taskPanel);
    m_taskPanel.bottomPanel = new wxScrolled<wxPanel>(m_taskPanel.taskPanel);

    m_taskPanel.bottomPanel->SetScrollRate(10, 10);

    m_taskPanel.taskPanel->SetName("Task Panel");

    m_taskPanel.bottomPanel->SetMinSize(wxSize(200, 200));

    m_taskPanel.taskPanelBoxSizer->Add(m_taskPanel.topPanel, wxSizerFlags(1).Expand().FixedMinSize());
    m_taskPanel.taskPanelBoxSizer->Add(m_taskPanel.bottomPanel, wxSizerFlags(9).Expand());

    m_mainBoxSizer->Add(m_taskPanel.taskPanel, wxSizerFlags().Expand().Proportion(3));

    m_taskPanel.taskPanel->SetSizerAndFit(m_taskPanel.taskPanelBoxSizer);
    m_taskPanel.topPanel->SetSizerAndFit(m_taskPanel.topBoxSizer);
    m_taskPanel.bottomPanel->SetSizerAndFit(m_taskPanel.bottomBoxSizer);

    m_taskPanel.addTaskButton = new wxButton(m_taskPanel.topPanel, wxID_ANY, "Add Task");
    m_taskPanel.projectNameText = new wxStaticText(m_taskPanel.topPanel, wxID_ANY, "");

    m_taskPanel.topBoxSizer->AddSpacer(10);
    m_taskPanel.topBoxSizer->Add(m_taskPanel.projectNameText, wxSizerFlags(0).CenterVertical());
    m_taskPanel.topBoxSizer->AddStretchSpacer(3);
    m_taskPanel.topBoxSizer->Add(m_taskPanel.addTaskButton, wxSizerFlags(0).CenterVertical());
    m_taskPanel.topBoxSizer->AddSpacer(10);
    wxLogDebug("Finished Task Panel");
}

void MainFrame::addSidebar() {
    wxLogDebug("Adding Sidebar Panel");
    m_sidebar.sidebarBoxSizer = new wxBoxSizer(wxVERTICAL);
    m_sidebar.projectsBoxSizer = new wxBoxSizer(wxVERTICAL);
    m_sidebar.homeBoxSizer = new wxBoxSizer(wxVERTICAL);

    m_sidebar.sideBarPanel = new wxPanel(this);
    m_sidebar.homePanel = new wxPanel(m_sidebar.sideBarPanel, wxID_ANY, wxDefaultPosition);
    m_sidebar.projectsPanel = new wxPanel(m_sidebar.sideBarPanel, wxID_ANY, wxDefaultPosition);

    m_sidebar.projectsPanel->SetName("Projects Panel");
    m_sidebar.homePanel->SetName("Home Panel");
    m_sidebar.sideBarPanel->SetName("Sidebar Panel");

    m_mainBoxSizer->Add(m_sidebar.sideBarPanel, wxSizerFlags().Expand().Proportion(1));

    m_sidebar.sidebarBoxSizer->Add(m_sidebar.homePanel, wxSizerFlags().Expand().Proportion(1).FixedMinSize());
    m_sidebar.sidebarBoxSizer->Add(m_sidebar.projectsPanel, wxSizerFlags().Expand().Proportion(2));

    m_sidebar.homePanel->SetSizerAndFit(m_sidebar.homeBoxSizer);
    m_sidebar.projectsPanel->SetSizerAndFit(m_sidebar.projectsBoxSizer);
    m_sidebar.sideBarPanel->SetSizerAndFit(m_sidebar.sidebarBoxSizer);


    wxLogDebug("Finished Sidebar Panel");
}

void MainFrame::setProject(TaskProjectComp* newProject) {
    wxLogDebug("Changing Project");

    if (newProject == nullptr) {
        wxLogDebug("Nullptr");
    }

    if (m_taskPanel.currentTaskCompList != nullptr) {
        m_taskPanel.currentTaskCompList->unselect(m_taskPanel.bottomBoxSizer);
    }
    m_taskPanel.currentTaskCompList = newProject;
    m_taskPanel.currentTaskCompList->select(m_taskPanel.bottomBoxSizer);
    m_taskPanel.taskPanelBoxSizer->Layout();
    m_taskPanel.projectNameText->SetLabel(m_taskPanel.currentTaskCompList->projectName);
    m_taskPanel.bottomPanel->Scroll(0, 0);
    wxLogDebug("Changed Project");
}
