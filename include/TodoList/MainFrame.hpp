#pragma once

#include "AppCore.hpp"
#include "TaskComp.hpp"
#include "TaskProjectComp.hpp"
#include "wx/event.h"
#include "wx/stattext.h"
#include <cstdint>
#include <map>
#include <utility>
#include <wx/frame.h>
#include <wx/sizer.h>

struct Sidebar {
    wxBoxSizer* sidebarBoxSizer;
    wxBoxSizer* homeBoxSizer;
    wxBoxSizer* projectsBoxSizer;
    TaskProjectComp* todayProject;
    TaskProjectComp* inboxProject;
    std::map<std::uint32_t, TaskProjectComp*> projectList;
};

struct TaskPanel {
    wxBoxSizer* taskPanelBoxSizer;
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
    Sidebar m_sidebar;
    TaskPanel m_taskPanel;
    bool m_ready;

    void setupTaskPanel();
    void setupSideBar();

    void onProjectChange(wxCommandEvent&);

    void test();

    const wxSize DEFAULT_FRAME_DIMS = wxSize(800, 600);
    const int SIDEBAR_WIDTH = 200;
};

template <class... Args>
MainFrame::MainFrame(Args... args)
    : wxFrame(std::forward<Args>(args)...),
      m_ready(false) {
}
