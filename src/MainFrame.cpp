#include "../include/TodoList/MainFrame.hpp"
#include "../include/TodoList/AppCore.hpp"
#include "../include/TodoList/TaskComp.hpp"

#include <chrono>
#include <ctime>
#include <format>
#include <string>
#include <utility>

#include "wx/calctrl.h"
#include "wx/event.h"
#include "wx/gdicmn.h"
#include "wx/log.h"
#include "wx/sizer.h"
#include "wx/textctrl.h"
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/scrolwin.h>
#include <wx/statline.h>

static TimePoint wxDateTimeToChrono(const wxDateTime& wxDate);

void MainFrame::refreshSidebar() {
    m_sidebar.homePanel->Layout();
    m_sidebar.homePanel->Refresh();
    m_sidebar.projectsPanel->Layout();
    m_sidebar.projectsPanel->Refresh();
    m_sidebar.sideBarPanel->Layout();
    m_sidebar.sideBarPanel->Refresh();
}

void MainFrame::refreshTaskPanel() {
    wxLogDebug("Adding Cal Dialog");
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

    m_taskPanel.bottomPanel->SetScrollRate(10, 10); // change to 0 horizontal scroll

    m_taskPanel.taskPanel->SetName("Task Panel");

    m_taskPanel.bottomPanel->SetMinSize(wxSize(200, 200));

    m_taskPanel.taskPanelBoxSizer->Add(m_taskPanel.topPanel, wxSizerFlags(1).Expand().FixedMinSize());
    m_taskPanel.taskPanelBoxSizer->Add(m_taskPanel.bottomPanel, wxSizerFlags(7).Expand());

    m_taskPanel.taskPanel->SetSizerAndFit(m_taskPanel.taskPanelBoxSizer);
    m_taskPanel.topPanel->SetSizerAndFit(m_taskPanel.topBoxSizer);
    m_taskPanel.bottomPanel->SetSizerAndFit(m_taskPanel.bottomBoxSizer);

    m_taskPanel.addTaskButton = new wxButton(m_taskPanel.topPanel, wxID_ANY, "Add Task");
    // m_taskPanel.projectNameText = new wxStaticText(m_taskPanel.topPanel, wxID_ANY, "");
    auto textStyle = wxBORDER_NONE | wxTE_WORDWRAP | wxTE_PROCESS_ENTER;
    m_taskPanel.projectNameTextCtrl =
        new wxTextCtrl(m_taskPanel.topPanel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, textStyle);

    m_taskPanel.addTaskButton->Bind(wxEVT_BUTTON, &MainFrame::onAddTaskButtonClicked, this);

    wxFont font(24, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    m_taskPanel.projectNameTextCtrl->SetFont(font);

    m_taskPanel.topBoxSizer->AddSpacer(10);
    m_taskPanel.topBoxSizer->Add(m_taskPanel.projectNameTextCtrl, 1, wxALIGN_CENTER_VERTICAL | wxALL, 10);
    m_taskPanel.topBoxSizer->Add(m_taskPanel.addTaskButton, wxSizerFlags(0).CenterVertical());
    m_taskPanel.topBoxSizer->AddSpacer(10);

    m_taskPanel.taskPanel->SetBackgroundColour(wxColor(255, 255, 255));
    m_taskPanel.projectNameTextCtrl->Bind(wxEVT_TEXT, &MainFrame::onProjectNameChanged, this);

    m_taskPanel.projectNameTextCtrl->Bind(wxEVT_TEXT_ENTER, [this](wxCommandEvent&) {
        m_taskPanel.currentTaskCompList->setProjectName(m_taskPanel.projectNameTextCtrl->GetValue());
        m_taskPanel.currentTaskCompList->SetFocus();
    });

    m_taskPanel.projectNameTextCtrl->Bind(wxEVT_TEXT, &MainFrame::onProjectNameChanged, this);

    // m_taskPanel.projectNameTextCtrl->Bind(wxEVT_KILL_FOCUS, [this](wxFocusEvent&){
    //     m_taskPanel.currentTaskCompList->setProjectName(m_taskPanel.currentTaskCompList->projectName);
    //     m_taskPanel.projectNameTextCtrl->SetValue(m_taskPanel.currentTaskCompList->projectName);
    //     m_taskPanel.currentTaskCompList->SetFocus();
    // });

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

    m_sidebar.projectsPanel->SetScrollRate(10, 10); // change to 0 horizontal scroll

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
    m_sidebar.homeBoxSizer->AddSpacer(20);
    m_sidebar.homeBoxSizer->Add(m_sidebar.myProjectsText, wxSizerFlags().Border(wxLEFT, 10));
    m_sidebar.homeBoxSizer->AddSpacer(10);

    refreshSidebar();
    wxLogDebug("Finished Sidebar");
}

void MainFrame::addCalDialog() {
    wxLogDebug("Adding Cal Dialog");
    m_calDialog.dialog = new wxDialog(this, wxID_ANY, "Set Date", wxDefaultPosition, wxDefaultSize);
    m_calDialog.mainSizer = new wxBoxSizer(wxVERTICAL);
    m_calDialog.calender = new wxCalendarCtrl(m_calDialog.dialog, wxID_ANY);
    m_calDialog.doneButton = new wxButton(m_calDialog.dialog, wxID_ANY, "Done");
    m_calDialog.cancelButton = new wxButton(m_calDialog.dialog, wxID_ANY, "Cancel");

    auto* buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    m_calDialog.mainSizer->Add(m_calDialog.calender, wxSizerFlags().Expand().Proportion(2));
    m_calDialog.mainSizer->Add(buttonSizer, wxSizerFlags().Expand().Proportion(1));

    m_calDialog.mainSizer->AddStretchSpacer(1);

    buttonSizer->AddStretchSpacer(2);
    buttonSizer->Add(m_calDialog.doneButton, wxSizerFlags(1));
    buttonSizer->AddStretchSpacer(1);
    buttonSizer->Add(m_calDialog.cancelButton, wxSizerFlags(1));
    buttonSizer->AddStretchSpacer(2);

    m_calDialog.dialog->SetSizer(m_calDialog.mainSizer);
    m_calDialog.calender->SetDate(wxDateTime::Today());

    m_calDialog.doneButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent& ev) {
        std::invoke(&MainFrame::onCalDialogDonePressed, this, std::ref(ev));
    });
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
    m_taskPanel.projectNameTextCtrl->SetValue(m_taskPanel.currentTaskCompList->projectName);
    m_taskPanel.projectNameTextCtrl->SetValue(m_taskPanel.currentTaskCompList->projectName);

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
    setProject(newProject);
    m_taskPanel.projectNameTextCtrl->SetFocus();
    m_taskPanel.projectNameTextCtrl->SetInsertionPointEnd();
    refreshSidebar();
}

void MainFrame::onTaskChecked(wxCommandEvent& ev) {
    wxLogDebug("Handling Checkbox checked");
    auto* taskCompPtr = static_cast<TaskComp*>(ev.GetClientData());

    if (taskCompPtr == nullptr) {
        wxLogDebug("Nullptr from %s", __FUNCTION__);
    }

    m_taskPanel.bottomBoxSizer->Detach(taskCompPtr);
    taskCompPtr->Hide();
    refreshTaskPanel();
    refreshSidebar();
}

void MainFrame::onCalDialogRequest(wxCommandEvent& ev) {
    wxLogDebug("Cal Dialog Requested");
    auto* taskCompPtr = static_cast<std::pair<TaskComp*, TaskComp::ChangingDate>*>(ev.GetClientData());

    if (taskCompPtr == nullptr || taskCompPtr->first == nullptr) {
        wxLogDebug("Task must not be null");
    }
    m_calDialog.currentTaskPair = taskCompPtr;
    m_calDialog.calender->SetDate(wxDateTime::Today());
    m_calDialog.dialog->ShowModal();
}

void MainFrame::onCalDialogDonePressed(wxCommandEvent& ev) {
    wxLogDebug("Date change done button pressed");
    if (m_calDialog.currentTaskPair == nullptr) {
        wxLogDebug("nullptr");
    }

    auto [taskCompPtr, dateChanging] = *m_calDialog.currentTaskPair;
    auto calDate = m_calDialog.calender->GetDate();
    auto calDateChrono = wxDateTimeToChrono(calDate);
    wxStaticText* staticTextPtr = nullptr;
    TimePoint* timePointPtr = nullptr;

    switch (dateChanging) {
    case TaskComp::ChangingDate::DUO_DATE:
        staticTextPtr = taskCompPtr->duoDateText;
        timePointPtr = &taskCompPtr->task->duoDate;
        break;
    case TaskComp::ChangingDate::DEADLINE_DATE:
        staticTextPtr = taskCompPtr->deadLineText;
        timePointPtr = &taskCompPtr->task->deadLine;
        break;
    default:
        std::unreachable();
        break;
    };

    if (staticTextPtr == nullptr || timePointPtr == nullptr) {
        wxLogDebug("Got nullptr");
        exit(0);
    }

    *timePointPtr = calDateChrono;
    staticTextPtr->SetLabel(std::format("{:%d %B}", *timePointPtr));

    staticTextPtr->Refresh();
    staticTextPtr->Layout();

    taskCompPtr->Layout();
    taskCompPtr->Refresh();

    m_calDialog.dialog->EndModal(0);
    ev.Skip();
}

void MainFrame::onProjectNameChanged(wxCommandEvent&) {

    const int maxSize = 25;
    auto name = m_taskPanel.projectNameTextCtrl->GetValue();
    if (m_taskPanel.projectNameTextCtrl->GetValue().size() > maxSize) {
        name = name.SubString(0, maxSize - 1);           // Trim to 30 characters
        m_taskPanel.projectNameTextCtrl->SetValue(name); // Update the text control
        m_taskPanel.projectNameTextCtrl->SetInsertionPoint(maxSize);
    }
    m_taskPanel.currentTaskCompList->projectNameText->SetLabel(name);
    m_taskPanel.currentTaskCompList->Refresh();
    m_taskPanel.currentTaskCompList->Layout();
}

static TimePoint wxDateTimeToChrono(const wxDateTime& wxDate) {
    wxLogDebug("Converting time to chrono");
    std::tm tm{};

    tm.tm_year = wxDate.GetYear();
    tm.tm_mon = wxDate.GetMonth();
    tm.tm_mday = wxDate.GetDay();

    std::time_t time_t_date = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(time_t_date);
}
