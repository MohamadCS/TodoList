#include "../include/TodoList/MainFrame.hpp"
#include "../include/TodoList/AppCore.hpp"
#include "../include/TodoList/TaskComp.hpp"

#include "wx/event.h"
#include "wx/log.h"
#include "wx/osx/stattext.h"
#include "wx/sizer.h"
#include <string>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/scrolwin.h>
#include <wx/statline.h>

void MainFrame::refreshSidebar() {
    m_sidebar.homePanel->Layout();
    m_sidebar.homePanel->Refresh();
    m_sidebar.projectsPanel->Layout();
    m_sidebar.projectsPanel->Refresh();
    m_sidebar.sideBarPanel->Layout();
    m_sidebar.sideBarPanel->Refresh();
}

void MainFrame::refreshTaskPanel() {
    m_taskPanel.taskPanel->Layout();
    m_taskPanel.taskPanel->Refresh();
}

void MainFrame::addTaskPanel() {
    wxLogDebug("Adding Task Panel");
    m_taskPanel.taskPanelBoxSizer = new wxBoxSizer(wxVERTICAL);
    m_taskPanel.topBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    m_taskPanel.bottomBoxSizer = new wxBoxSizer(wxVERTICAL);

    m_taskPanel.taskPanel = new wxPanel(m_mainSplitter);
    m_taskPanel.topPanel = new wxPanel(m_taskPanel.taskPanel);
    m_taskPanel.bottomPanel = new wxScrolled<wxPanel>(m_taskPanel.taskPanel);

    m_taskPanel.bottomPanel->SetScrollRate(10, 10);

    m_taskPanel.taskPanel->SetName("Task Panel");

    m_taskPanel.bottomPanel->SetMinSize(wxSize(200, 200));

    m_taskPanel.taskPanelBoxSizer->Add(m_taskPanel.topPanel, wxSizerFlags(1).Expand().FixedMinSize());
    m_taskPanel.taskPanelBoxSizer->Add(m_taskPanel.bottomPanel, wxSizerFlags(9).Expand());

    m_taskPanel.taskPanel->SetSizerAndFit(m_taskPanel.taskPanelBoxSizer);
    m_taskPanel.topPanel->SetSizerAndFit(m_taskPanel.topBoxSizer);
    m_taskPanel.bottomPanel->SetSizerAndFit(m_taskPanel.bottomBoxSizer);

    m_taskPanel.addTaskButton = new wxButton(m_taskPanel.topPanel, wxID_ANY, "Add Task");
    m_taskPanel.projectNameText = new wxStaticText(m_taskPanel.topPanel, wxID_ANY, "");

    m_taskPanel.addTaskButton->Bind(wxEVT_BUTTON, &MainFrame::onAddTaskButtonClicked, this);

    wxFont font(20, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    m_taskPanel.projectNameText->SetFont(std::move(font));

    m_taskPanel.topBoxSizer->AddSpacer(10);
    m_taskPanel.topBoxSizer->Add(m_taskPanel.projectNameText, wxSizerFlags(0).CenterVertical());
    m_taskPanel.topBoxSizer->AddStretchSpacer(3);
    m_taskPanel.topBoxSizer->Add(m_taskPanel.addTaskButton, wxSizerFlags(0).CenterVertical());
    m_taskPanel.topBoxSizer->AddSpacer(10);

    m_taskPanel.taskPanel->SetBackgroundColour(wxColor(255, 255, 255));
    refreshTaskPanel();
    wxLogDebug("Finished Task Panel");
}

void MainFrame::addSidebar() {
    wxLogDebug("Adding Sidebar Panel");

    m_sidebar.sidebarBoxSizer = new wxBoxSizer(wxVERTICAL);
    m_sidebar.projectsBoxSizer = new wxBoxSizer(wxVERTICAL);
    m_sidebar.homeBoxSizer = new wxBoxSizer(wxVERTICAL);

    m_sidebar.sideBarPanel = new wxPanel(m_mainSplitter);
    m_sidebar.homePanel = new wxPanel(m_sidebar.sideBarPanel, wxID_ANY, wxDefaultPosition);
    m_sidebar.projectsPanel = new wxScrolled<wxPanel>(m_sidebar.sideBarPanel, wxID_ANY, wxDefaultPosition);

    m_sidebar.projectsPanel->SetScrollRate(10, 10);

    m_sidebar.projectsPanel->SetName("Projects Panel");
    m_sidebar.homePanel->SetName("Home Panel");
    m_sidebar.sideBarPanel->SetName("Sidebar Panel");

    m_sidebar.sidebarBoxSizer->Add(m_sidebar.homePanel, wxSizerFlags().Expand().Proportion(1));
    m_sidebar.sidebarBoxSizer->Add(m_sidebar.projectsPanel, wxSizerFlags().Expand().Proportion(2));

    m_sidebar.homePanel->SetSizerAndFit(m_sidebar.homeBoxSizer);
    m_sidebar.projectsPanel->SetSizerAndFit(m_sidebar.projectsBoxSizer);
    m_sidebar.sideBarPanel->SetSizerAndFit(m_sidebar.sidebarBoxSizer);

    m_sidebar.sideBarPanel->SetBackgroundColour(wxColour(250, 250, 250));

    auto& appCore = AppCore::instance();
    m_sidebar.inboxProject = new TaskProjectComp(m_sidebar.homePanel, wxID_ANY, appCore.generateProjectId(), "Inbox");
    m_sidebar.todayProject = new TaskProjectComp(m_sidebar.homePanel, wxID_ANY, appCore.generateProjectId(), "Today");

    m_sidebar.homeBoxSizer->Add(m_sidebar.inboxProject, wxSizerFlags(0).FixedMinSize().Border(wxALL, 5).Expand());
    m_sidebar.homeBoxSizer->Add(m_sidebar.todayProject, wxSizerFlags(0).FixedMinSize().Border(wxALL, 5).Expand());

    m_sidebar.projectsList.push_back(m_sidebar.inboxProject);
    m_sidebar.projectsList.push_back(m_sidebar.todayProject);

    m_sidebar.myProjectsText = new wxStaticText(m_sidebar.homePanel, wxID_ANY, "My Projects");
    wxFont font(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    m_sidebar.myProjectsText->SetFont(font);

    m_sidebar.addProjectButton = new wxButton(m_sidebar.homePanel, wxID_ANY, "Add Project");
    m_sidebar.addProjectButton->Bind(wxEVT_BUTTON, &MainFrame::onAddProjectButtonClicked, this);

    m_sidebar.homeBoxSizer->AddSpacer(10);
    m_sidebar.homeBoxSizer->Add(m_sidebar.addProjectButton, wxSizerFlags().CenterHorizontal());
    m_sidebar.homeBoxSizer->AddStretchSpacer();
    m_sidebar.homeBoxSizer->Add(m_sidebar.myProjectsText, wxSizerFlags().Border(wxLEFT, 10));
    m_sidebar.homeBoxSizer->AddSpacer(10);

    refreshSidebar();
    wxLogDebug("Finished Sidebar");
}

void MainFrame::onProjectChange(wxCommandEvent& ev) {
    wxLogDebug("Got a change project event");
    auto* taskProjectCompPtr = static_cast<TaskProjectComp*>(ev.GetClientData());

    if (taskProjectCompPtr == nullptr) {
        wxLogDebug("Nullptr");
    }

    setProject(taskProjectCompPtr);
}

void MainFrame::setProject(TaskProjectComp* newProject) {
    wxLogDebug("Changing Project");

    if (newProject == nullptr) {
        wxLogDebug("Nullptr");
        exit(0);
    }

    if (m_taskPanel.currentTaskCompList != nullptr) {
        m_taskPanel.currentTaskCompList->unselect(m_taskPanel.bottomBoxSizer);
    }
    m_taskPanel.currentTaskCompList = newProject;
    m_taskPanel.currentTaskCompList->select(m_taskPanel.bottomBoxSizer);
    m_taskPanel.projectNameText->SetLabel(m_taskPanel.currentTaskCompList->projectName);

    m_taskPanel.bottomPanel->Scroll(0, 0);

    refreshSidebar();
    refreshTaskPanel();

    wxLogDebug("Changed Project");
}

void MainFrame::onAddTaskButtonClicked(wxCommandEvent& ev) {
    auto& appCore = AppCore::instance();
    auto* currentProject = m_taskPanel.currentTaskCompList;
    auto* task = appCore.newTask({}, {}, "", "", false, currentProject->taskList);
    auto* taskCompPtr = currentProject->addTask(task, m_taskPanel.bottomPanel);

    auto mouseEvent = wxMouseEvent();
    taskCompPtr->onPanelDoubleLeftClick(mouseEvent);

    refreshTaskPanel();
}

void MainFrame::onAddProjectButtonClicked(wxCommandEvent& ev) {
    auto& appCore = AppCore::instance();
    auto* newProject =
        new TaskProjectComp(m_sidebar.projectsPanel, wxID_ANY, appCore.generateProjectId(), "Empty Project");
    wxLogDebug("Added Project with ID: %d", newProject->projectId);
    m_sidebar.projectsList.push_back(newProject);
    m_sidebar.projectsBoxSizer->Add(newProject, wxSizerFlags(0).FixedMinSize().Border(wxALL, 5).Expand());
    refreshSidebar();
}

void MainFrame::onTaskChecked(wxCommandEvent& ev) {
    wxLogDebug("Handling Checkbox checked");
    auto* taskCompPtr = static_cast<TaskComp*>(ev.GetClientData());

    if (taskCompPtr == nullptr) {
        wxLogDebug("Nullptr");
    }

    m_taskPanel.bottomBoxSizer->Detach(taskCompPtr);
    taskCompPtr->Hide();
    refreshTaskPanel();
    refreshSidebar();
}
