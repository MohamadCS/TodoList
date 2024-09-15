#pragma once

#include "Events.hpp"
#include "TaskProjectComp.hpp"
#include "wx/colour.h"
#include "wx/gdicmn.h"
#include "wx/generic/panelg.h"
#include "wx/log.h"
#include "wx/osx/stattext.h"
#include "wx/scrolwin.h"

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

    wxButton* addProjectButton;


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
    wxBoxSizer* m_mainBoxSizer;
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
};

template <class... Args>
MainFrame::MainFrame(Args... args)
    : wxFrame(std::forward<Args>(args)...) {

    m_mainBoxSizer = new wxBoxSizer(wxHORIZONTAL);

    addSidebar();
    addTaskPanel();

    SetSizerAndFit(m_mainBoxSizer);
    SetClientSize(DEFAULT_FRAME_DIMS);

    SetBackgroundColour(wxColor(250, 244, 237));
    Refresh();
    Layout();

    Bind(EVT_CHANGE_PROJECT, &MainFrame::onProjectChange, this);
}
