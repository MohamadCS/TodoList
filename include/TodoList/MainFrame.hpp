#pragma once

#include "Events.hpp"
#include "TaskProjectComp.hpp"
#include "wx/colour.h"
#include "wx/gdicmn.h"
#include "wx/generic/panelg.h"
#include "wx/log.h"
#include "wx/scrolwin.h"
#include "wx/splitter.h"

#include <cstdint>
#include <map>
#include <utility>

#include <wx/event.h>
#include <wx/frame.h>
#include <wx/sizer.h>

struct Sidebar {
    wxBoxSizer* sidebarBoxSizer;
    wxBoxSizer* homeBoxSizer;
    wxBoxSizer* projectsBoxSizer;

    wxPanel* sideBarPanel;
    wxPanel* homePanel;
    wxScrolled<wxPanel>* projectsPanel;

    wxButton* addProjectButton;
    wxStaticText* myProjectsText;

    TaskProjectComp* todayProject;
    TaskProjectComp* inboxProject;

    std::map<std::uint32_t, TaskProjectComp*> projectList;
};

struct TaskPanel {
    wxBoxSizer* taskPanelBoxSizer;
    wxBoxSizer* topBoxSizer;
    wxBoxSizer* bottomBoxSizer;
    wxPanel* taskPanel;
    wxPanel* topPanel;
    wxScrolled<wxPanel>* bottomPanel;
    wxButton* addTaskButton;
    wxStaticText* projectNameText;
    TaskProjectComp* currentTaskCompList = nullptr;
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
    const wxSize DEFAULT_FRAME_DIMS = wxSize(800, 600);
    const int SIDEBAR_WIDTH = 200;

    void setupTaskPanel();
    void setupSideBar();
    void onProjectChange(wxCommandEvent&);
    void test();
    void addTaskPanel();
    void addSidebar();
    void setProject(TaskProjectComp*);
    void onAddProjectButtonClicked(wxCommandEvent&);
    void onAddTaskButtonClicked(wxCommandEvent&);
};

template <class... Args>
MainFrame::MainFrame(Args... args)
    : wxFrame(std::forward<Args>(args)...) {

    // m_mainSplitter = new wxBoxSizer(wxHORIZONTAL);
    m_mainSplitter =
        new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);

    m_mainSplitter->SetMinimumPaneSize(200);
    m_mainSplitter->SetSashPosition(200);
    m_mainSplitter->SetSashGravity(0);

    addSidebar();
    addTaskPanel();

    m_mainSplitter->SplitVertically(m_sidebar.sideBarPanel, m_taskPanel.taskPanel);

    Layout();
    SetClientSize(DEFAULT_FRAME_DIMS);
    SetMinClientSize(DEFAULT_FRAME_DIMS);
    m_mainSplitter->SetBackgroundColour(wxColor(255, 255, 255));
    SetBackgroundColour(wxColor(255, 255, 255));
    Bind(EVT_CHANGE_PROJECT, &MainFrame::onProjectChange, this);
}
