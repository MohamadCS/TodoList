#pragma once

#include "Events.hpp"
#include "TaskProjectComp.hpp"
#include "wx/gdicmn.h"
#include "wx/generic/panelg.h"
#include "wx/log.h"
#include "wx/osx/stattext.h"

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
    wxPanel* projectsPanel;

    TaskProjectComp* todayProject;
    TaskProjectComp* inboxProject;

    std::map<std::uint32_t, TaskProjectComp*> projectList;
};

struct TaskPanel {
    wxBoxSizer* taskPanelBoxSizer;
    wxPanel* taskPanel;
    wxStaticText* projectName;
    TaskProjectComp* currentTaskCompList;
};

class MainFrame : public wxFrame {
public:
    template <class... Args>
    MainFrame(Args... args);
    void setup();
    ~MainFrame() override = default;

private:
    wxBoxSizer* m_mainBoxSizer;
    wxPanel* m_mainPanel;
    Sidebar m_sidebar;
    TaskPanel m_taskPanel;
    bool m_ready;

    const wxSize DEFAULT_FRAME_DIMS = wxSize(800, 600);
    const int SIDEBAR_WIDTH = 200;

    void setupTaskPanel();
    void setupSideBar();
    void onProjectChange(wxCommandEvent&);
    void test();
};

template <class... Args>
MainFrame::MainFrame(Args... args)
    : wxFrame(std::forward<Args>(args)...),
      m_ready(false) {

    m_mainBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    m_sidebar.sidebarBoxSizer = new wxBoxSizer(wxVERTICAL);
    m_sidebar.projectsBoxSizer = new wxBoxSizer(wxVERTICAL);
    m_sidebar.homeBoxSizer = new wxBoxSizer(wxVERTICAL);
    m_taskPanel.taskPanelBoxSizer = new wxBoxSizer(wxVERTICAL);

    m_taskPanel.taskPanel = new wxPanel(this);
    m_sidebar.sideBarPanel = new wxPanel(this);
    m_sidebar.homePanel = new wxPanel(m_sidebar.sideBarPanel, wxID_ANY, wxDefaultPosition);
    m_sidebar.projectsPanel = new wxPanel(m_sidebar.sideBarPanel, wxID_ANY, wxDefaultPosition);

    m_sidebar.projectsPanel->SetName("Projects Panel");
    m_sidebar.homePanel->SetName("Home Panel");
    m_sidebar.sideBarPanel->SetName("Sidebar Panel");
    m_taskPanel.taskPanel->SetName("Task Panel");

    m_sidebar.sideBarPanel->SetMinSize(wxSize(200, 200));
    m_sidebar.homePanel->SetMinSize(wxSize(200, 200));

    m_mainBoxSizer->Add(m_sidebar.sideBarPanel);
    m_mainBoxSizer->Add(m_taskPanel.taskPanel);

    m_sidebar.sidebarBoxSizer->Add(m_sidebar.homePanel);
    m_sidebar.sidebarBoxSizer->Add(m_sidebar.projectsPanel);

    m_sidebar.homePanel->SetSizerAndFit(m_sidebar.homeBoxSizer);
    m_sidebar.projectsPanel->SetSizerAndFit(m_sidebar.projectsBoxSizer);
    m_sidebar.sideBarPanel->SetSizerAndFit(m_sidebar.sidebarBoxSizer);
    m_taskPanel.taskPanel->SetSizerAndFit(m_taskPanel.taskPanelBoxSizer);
    SetSizerAndFit(m_mainBoxSizer);

    SetClientSize(DEFAULT_FRAME_DIMS);
    Refresh();
    Layout();

    Bind(EVT_CHANGE_PROJECT, &MainFrame::onProjectChange, this);
}

