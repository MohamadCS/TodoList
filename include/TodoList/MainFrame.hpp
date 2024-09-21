#pragma once

#include "Events.hpp"
#include "TaskComp.hpp"
#include "TaskProjectComp.hpp"
#include "Defines.hpp"

#include <utility>
#include <cstdint>

#include "wx/colour.h"
#include "wx/gdicmn.h"
#include "wx/generic/panelg.h"
#include "wx/log.h"
#include "wx/scrolwin.h"
#include "wx/splitter.h"
#include <wx/calctrl.h>
#include <wx/dialog.h>

#include <wx/event.h>
#include <wx/frame.h>
#include <wx/sizer.h>

namespace TodoList::Gui {

struct Sidebar {
    wxPanel* sideBarPanel;
    wxPanel* homePanel;
    wxScrolled<wxPanel>* projectsPanel;
    wxButton* addProjectButton;

    wxStaticText* myProjectsText;
    std::vector<TaskProjectComp*> projectsList;
};

struct TaskPanel {
    wxPanel* taskPanel;
    wxPanel* topPanel;
    wxScrolled<wxPanel>* bottomPanel;
    wxButton* addTaskButton;
    wxTextCtrl* projectNameTextCtrl;
    TaskProjectComp* currentTaskCompList = nullptr;
};

struct CalanderDialog {
    wxDialog* dialog;
    wxButton* doneButton;
    wxButton* cancelButton;
    wxCalendarCtrl* calender;
    wxBoxSizer* mainSizer;
    std::pair<TaskComp*, TaskComp::ChangingDate>* currentTaskPair = nullptr;
};

class MainFrame : public wxFrame {
public:
    template <class... Args>
    MainFrame(Args... args);
    void setup();

    ~MainFrame() override = default;

private:
    wxSplitterWindow* m_mainSplitter;
    wxPanel* m_mainPanel;
    Sidebar m_sidebar;
    TaskPanel m_taskPanel;
    CalanderDialog m_calDialog;

    const wxSize DEFAULT_FRAME_DIMS = wxSize(800, 600);
    const int SIDEBAR_WIDTH = 200;

    void addTaskPanel();
    void addSidebar();
    void addCalDialog();

    void setProject(TaskProjectComp*);

    void refreshSidebar();
    void refreshTaskPanel();

    // Events
    void onProjectChange(wxCommandEvent&);
    void onTaskChecked(wxCommandEvent&);
    void onAddProjectButtonClicked(wxCommandEvent&);
    void onAddTaskButtonClicked(wxCommandEvent&);
    void onCalDialogRequest(wxCommandEvent&);
    void onCalDialogDonePressed(wxCommandEvent&);
    void onProjectNameChanged(wxCommandEvent&);
};

template <class... Args>
MainFrame::MainFrame(Args... args)
    : wxFrame(std::forward<Args>(args)...) {

    m_mainSplitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE);

    m_mainSplitter->SetMinimumPaneSize(200);
    m_mainSplitter->SetSashPosition(200);
    m_mainSplitter->SetSashGravity(0);

    addSidebar();
    addTaskPanel();
    addCalDialog();

    m_mainSplitter->SplitVertically(m_sidebar.sideBarPanel, m_taskPanel.taskPanel);
    Bind(EVT_CHANGE_PROJECT, &MainFrame::onProjectChange, this);
    Bind(EVT_TASK_FINISHED, &MainFrame::onTaskChecked, this);
    Bind(EVT_REQUEST_CAL_DIALOG, &MainFrame::onCalDialogRequest, this);


    Layout();
    SetClientSize(DEFAULT_FRAME_DIMS);
    SetMinClientSize(DEFAULT_FRAME_DIMS);
    SetBackgroundColour(wxColor(255, 255, 255));
    setProject(m_sidebar.projectsList[static_cast<int>(Utility::DEFAULT_PROJECTS::INBOX)]);
}

} // namespace TodoList::Gui
