#include "../include/TodoList/MainFrame.hpp"
#include "../include/TodoList/AppCore.hpp"
#include "../include/TodoList/TaskComp.hpp"
#include "../include/TodoList/Utils.hpp"

#include <chrono>
#include <ctime>
#include <functional>
#include <tuple>
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

static TodoList::Core::TimePoint wxDateTimeToChrono(const wxDateTime& wxDate);

namespace TodoList::Gui {

void MainFrame::setup() {


    Core::App::instance().loadDatabases();
    addSidebar();
    addTaskPanel();
    addCalDialog();
    setProject(m_sidebar.projectsList[Utility::INBOX_IDX]);
    loadProjects();

    m_mainSplitter->SplitVertically(m_sidebar.sideBarPanel, m_taskPanel.taskPanel);
    m_mainSplitter->SetMinimumPaneSize(200);
    m_mainSplitter->SetSashPosition(200);
    m_mainSplitter->SetSashGravity(0);

    Bind(EVT_CHANGE_PROJECT, &MainFrame::onProjectChange, this);
    Bind(EVT_TASK_FINISHED, &MainFrame::onTaskChecked, this);
    Bind(EVT_REQUEST_CAL_DIALOG, &MainFrame::onCalDialogRequest, this);
    Layout();
}

void MainFrame::refreshSidebar() {
    Utility::refresh(std::make_tuple(m_sidebar.homePanel, m_sidebar.projectsPanel, m_sidebar.sideBarPanel));
}

void MainFrame::refreshTaskPanel() {
    Utility::refresh(std::make_tuple(m_taskPanel.taskPanel));
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
        m_taskPanel.currentTaskCompList->hideProject(m_taskPanel.bottomPanel->GetSizer());
    }
    m_taskPanel.currentTaskCompList = newProject;
    m_taskPanel.currentTaskCompList->showProject(m_taskPanel.bottomPanel->GetSizer());
    m_taskPanel.projectNameTextCtrl->SetValue(m_taskPanel.currentTaskCompList->getProjectName());

    m_taskPanel.bottomPanel->Scroll(0, 0);

    refreshSidebar();
    refreshTaskPanel();

    wxLogDebug("Changed Project");
}

void MainFrame::onAddTaskButtonClicked(wxCommandEvent& ev) {
    // auto mouseEvent = wxMouseEvent();

    if (auto taskPtr = m_taskPanel.currentTaskCompList->addTask(m_taskPanel.bottomPanel); taskPtr) {
        taskPtr.value()->setStateChangingText();
    } else {
        std::unreachable();
    }

    refreshTaskPanel();
}

void MainFrame::onAddProjectButtonClicked(wxCommandEvent& ev) {
    auto* newProject{new TaskProjectComp(m_sidebar.projectsPanel, wxID_ANY, "Empty Project")};

    wxLogDebug("Added Project with ID: %d", newProject->getProjectId());

    m_sidebar.projectsList.insert_or_assign(newProject->getProjectId(), newProject);
    m_sidebar.projectsPanel->GetSizer()->Add(newProject, wxSizerFlags(0).FixedMinSize().Border(wxALL, 5).Expand());

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

    m_taskPanel.bottomPanel->GetSizer()->Detach(taskCompPtr);
    taskCompPtr->Hide();
    refreshTaskPanel();
}

void MainFrame::onCalDialogRequest(wxCommandEvent& ev) {
    wxLogDebug("Cal Dialog Requested");
    auto* taskCompPtr{static_cast<std::pair<TaskComp*, TaskComp::Date>*>(ev.GetClientData())};

    if (taskCompPtr == nullptr || taskCompPtr->first == nullptr) {
        wxLogDebug("Task must not be null");
    }
    m_calDialog.currentTaskPair = taskCompPtr;
    m_calDialog.calenderCtrl->SetDate(wxDateTime::Today());
    m_calDialog.dialog->ShowModal();
}

void MainFrame::onCalDialogDonePressed(wxCommandEvent& ev) {
    wxLogDebug("Date change done button pressed");
    if (m_calDialog.currentTaskPair == nullptr) {
        wxLogDebug("nullptr");
    }

    auto [pTaskComp, dateChanging] = *m_calDialog.currentTaskPair;
    auto calDate = m_calDialog.calenderCtrl->GetDate();
    auto calDateChrono = wxDateTimeToChrono(calDate);

    pTaskComp->setDate(calDateChrono, dateChanging);

    m_calDialog.dialog->EndModal(0);

    ev.Skip();
}

void MainFrame::onCalDialogCancelPressed(wxCommandEvent& ev) {
    wxLogDebug("Dialog Cancel Button pressed");

    m_calDialog.dialog->EndModal(0);
    ev.Skip();
}

void MainFrame::updateToday(const Core::TimePoint& timePoint, Core::Task* pTask) {
    if (pTask == nullptr) {
        wxLogDebug("nullptr");
    }

    if (m_taskPanel.currentTaskCompList->getProjectId() == m_sidebar.projectsList[Utility::TODAY_IDX]->getProjectId() ||
        !Utility::isToday(timePoint)) {
        return;
    }

    auto* todayProjectComp = m_sidebar.projectsList[Utility::TODAY_IDX];

    if (!todayProjectComp->addTask(m_taskPanel.bottomPanel, pTask)) {
    }
}

void MainFrame::loadProjects() {
    LOG("Loading Projects");
    auto& appCore = Core::App::instance();
    auto& taskLists = appCore.getTaskLists();

    for (auto& [taskListId, pTaskList] : taskLists) {

        if (taskListId <= Core::App::MAX_DEFAULT_ID) {
            continue;
        }

        auto* pTaskProjectComp{new TaskProjectComp(m_sidebar.projectsPanel, wxID_ANY, std::nullopt, pTaskList.get())};
        m_sidebar.projectsPanel->GetSizer()->Add(pTaskProjectComp,
                                                 wxSizerFlags(0).FixedMinSize().Border(wxALL, 5).Expand());
        m_sidebar.projectsList.insert_or_assign(pTaskProjectComp->getProjectId(), pTaskProjectComp);
    }

    LOG("Addin tasks comp");

    auto& tasks = appCore.getTasksList();
    for (auto& pTask : tasks) {
        LOG("Adding TaskCompList to {}", pTask.get()->getTaskList()->getId());
        if (m_sidebar.projectsList.at(pTask.get()->getTaskList()->getId())
                ->addTask(m_taskPanel.bottomPanel, pTask.get())) {
        }
    }

    for (auto& [_, pTaskProjectComp] : m_sidebar.projectsList) {
        Utility::refresh(pTaskProjectComp);
        for (auto* pTaskComp : pTaskProjectComp->getTaskCompList()) {
            Utility::refresh(pTaskComp);
        }
        refreshSidebar();
        refreshTaskPanel();
    }
}

} // namespace TodoList::Gui

static TodoList::Core::TimePoint wxDateTimeToChrono(const wxDateTime& wxDate) {
    wxLogDebug("Converting time to chrono");

    std::tm tm{};

    tm.tm_year = wxDate.GetYear();
    tm.tm_mon = wxDate.GetMonth();
    tm.tm_mday = wxDate.GetDay();

    std::time_t time_t_date = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(time_t_date);
}

// Intial Panel Impl

namespace TodoList::Gui {

void MainFrame::onProjectNameChanged(wxCommandEvent&) {
    auto name = m_taskPanel.projectNameTextCtrl->GetValue();

    m_taskPanel.currentTaskCompList->setProjectName(name, true);
    Utility::refresh(m_taskPanel.currentTaskCompList);
}

void MainFrame::addTaskPanel() {
    wxLogDebug("Adding Task Panel");

    m_taskPanel.taskPanel = new wxPanel(m_mainSplitter);
    m_taskPanel.topPanel = new wxPanel(m_taskPanel.taskPanel);
    m_taskPanel.bottomPanel = new wxScrolled<wxPanel>(m_taskPanel.taskPanel);

    m_taskPanel.taskPanel->SetSizer(new wxBoxSizer(wxVERTICAL));
    m_taskPanel.topPanel->SetSizer(new wxBoxSizer(wxHORIZONTAL));
    m_taskPanel.bottomPanel->SetSizer(new wxBoxSizer(wxVERTICAL));

    m_taskPanel.bottomPanel->SetScrollRate(5, 5); // change to 0 horizontal scroll

    m_taskPanel.taskPanel->SetName("Task Panel");

    m_taskPanel.bottomPanel->SetMinSize(wxSize(200, 200));

    m_taskPanel.taskPanel->GetSizer()->Add(m_taskPanel.topPanel, wxSizerFlags(1).Expand().FixedMinSize());
    m_taskPanel.taskPanel->GetSizer()->Add(m_taskPanel.bottomPanel, wxSizerFlags(7).Expand());

    m_taskPanel.addTaskButton = new wxButton(m_taskPanel.topPanel, wxID_ANY, "Add Task");
    auto textStyle{wxBORDER_NONE | wxTE_WORDWRAP | wxTE_PROCESS_ENTER};
    m_taskPanel.projectNameTextCtrl =
        new wxTextCtrl(m_taskPanel.topPanel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, textStyle);

    m_taskPanel.addTaskButton->Bind(wxEVT_BUTTON, &MainFrame::onAddTaskButtonClicked, this);

    wxFont font(24, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    m_taskPanel.projectNameTextCtrl->SetFont(font);

    m_taskPanel.topPanel->GetSizer()->AddSpacer(10);
    m_taskPanel.topPanel->GetSizer()->Add(m_taskPanel.projectNameTextCtrl, 1, wxALIGN_CENTER_VERTICAL | wxALL, 10);
    m_taskPanel.topPanel->GetSizer()->Add(m_taskPanel.addTaskButton, wxSizerFlags(0).CenterVertical());
    m_taskPanel.topPanel->GetSizer()->AddSpacer(10);

    m_taskPanel.taskPanel->SetBackgroundColour(wxColor(255, 255, 255));
    m_taskPanel.projectNameTextCtrl->Bind(wxEVT_TEXT, &MainFrame::onProjectNameChanged, this);

    m_taskPanel.projectNameTextCtrl->Bind(wxEVT_TEXT_ENTER, [this](wxCommandEvent&) {
        m_taskPanel.currentTaskCompList->setProjectName(m_taskPanel.projectNameTextCtrl->GetValue());
        m_taskPanel.currentTaskCompList->SetFocus();
    });

    m_taskPanel.projectNameTextCtrl->Bind(wxEVT_TEXT, &MainFrame::onProjectNameChanged, this);

    refreshTaskPanel();
    wxLogDebug("Finished Task Panel");
}

void MainFrame::addSidebar() {
    wxLogDebug("Adding Sidebar Panel");

    m_sidebar.sideBarPanel = new wxPanel(m_mainSplitter);
    m_sidebar.homePanel = new wxPanel(m_sidebar.sideBarPanel, wxID_ANY, wxDefaultPosition);
    m_sidebar.projectsPanel = new wxScrolled<wxPanel>(m_sidebar.sideBarPanel, wxID_ANY, wxDefaultPosition);

    m_sidebar.sideBarPanel->SetSizerAndFit(new wxBoxSizer(wxVERTICAL));
    m_sidebar.projectsPanel->SetSizerAndFit(new wxBoxSizer(wxVERTICAL));
    m_sidebar.homePanel->SetSizerAndFit(new wxBoxSizer(wxVERTICAL));

    m_sidebar.projectsPanel->SetScrollRate(5, 5); // change to 0 horizontal scroll

    m_sidebar.projectsPanel->SetName("Projects Panel");
    m_sidebar.homePanel->SetName("Home Panel");
    m_sidebar.sideBarPanel->SetName("Sidebar Panel");

    m_sidebar.sideBarPanel->GetSizer()->Add(m_sidebar.homePanel, wxSizerFlags().Expand().Proportion(1));
    m_sidebar.sideBarPanel->GetSizer()->Add(m_sidebar.projectsPanel, wxSizerFlags().Expand().Proportion(2));

    m_sidebar.sideBarPanel->SetBackgroundColour(wxColour(250, 250, 250));

    auto& appCore = Core::App::instance();

    m_sidebar.projectsList[Utility::INBOX_IDX] = new TaskProjectComp(
        m_sidebar.homePanel, wxID_ANY, std::nullopt, appCore.getTaskLists().at(Utility::INBOX_IDX).get());

    m_sidebar.projectsList[Utility::TODAY_IDX] = new TaskProjectComp(
        m_sidebar.homePanel, wxID_ANY, std::nullopt, appCore.getTaskLists().at(Utility::TODAY_IDX).get());

    m_sidebar.homePanel->GetSizer()->Add(m_sidebar.projectsList[Utility::INBOX_IDX],
                                         wxSizerFlags(0).FixedMinSize().Border(wxALL, 5).Expand());
    m_sidebar.homePanel->GetSizer()->Add(m_sidebar.projectsList[Utility::TODAY_IDX],
                                         wxSizerFlags(0).FixedMinSize().Border(wxALL, 5).Expand());

    m_sidebar.myProjectsText = new wxStaticText(m_sidebar.homePanel, wxID_ANY, "My Projects");
    wxFont font(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    m_sidebar.myProjectsText->SetFont(font);

    m_sidebar.addProjectButton = new wxButton(m_sidebar.homePanel, wxID_ANY, "Add Project");
    m_sidebar.addProjectButton->Bind(wxEVT_BUTTON, &MainFrame::onAddProjectButtonClicked, this);

    m_sidebar.homePanel->GetSizer()->AddSpacer(10);
    m_sidebar.homePanel->GetSizer()->Add(m_sidebar.addProjectButton, wxSizerFlags().CenterHorizontal());
    m_sidebar.homePanel->GetSizer()->AddSpacer(20);
    m_sidebar.homePanel->GetSizer()->Add(m_sidebar.myProjectsText, wxSizerFlags().Border(wxLEFT, 10));
    m_sidebar.homePanel->GetSizer()->AddSpacer(10);

    refreshSidebar();
    wxLogDebug("Finished Sidebar");
}

void MainFrame::addCalDialog() {
    wxLogDebug("Adding Cal Dialog");
    m_calDialog.dialog = new wxDialog(this, wxID_ANY, "Set Date", wxDefaultPosition, wxDefaultSize);
    m_calDialog.calenderCtrl = new wxCalendarCtrl(m_calDialog.dialog, wxID_ANY);
    m_calDialog.doneButton = new wxButton(m_calDialog.dialog, wxID_ANY, "Done");
    m_calDialog.cancelButton = new wxButton(m_calDialog.dialog, wxID_ANY, "Cancel");
    m_calDialog.dialog->SetSizer(new wxBoxSizer(wxVERTICAL));

    auto* buttonSizer{new wxBoxSizer(wxHORIZONTAL)};

    m_calDialog.dialog->GetSizer()->Add(m_calDialog.calenderCtrl, wxSizerFlags().Expand().Proportion(2));
    m_calDialog.dialog->GetSizer()->Add(buttonSizer, wxSizerFlags().Expand().Proportion(1));
    m_calDialog.dialog->GetSizer()->AddStretchSpacer(1);

    buttonSizer->AddStretchSpacer(2);
    buttonSizer->Add(m_calDialog.doneButton, wxSizerFlags(1));
    buttonSizer->AddStretchSpacer(1);
    buttonSizer->Add(m_calDialog.cancelButton, wxSizerFlags(1));
    buttonSizer->AddStretchSpacer(2);

    m_calDialog.calenderCtrl->SetDate(wxDateTime::Today());

    m_calDialog.doneButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent& ev) {
        std::invoke(&MainFrame::onCalDialogDonePressed, this, std::ref(ev));
    });

    m_calDialog.cancelButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent& ev) {
        std::invoke(&MainFrame::onCalDialogCancelPressed, this, std::ref(ev));
    });

    m_calDialog.dialog->Fit();
}
} // namespace TodoList::Gui
