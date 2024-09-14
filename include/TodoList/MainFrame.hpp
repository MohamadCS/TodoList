#pragma once

#include <utility>
#include <wx/frame.h>
#include <wx/sizer.h>

struct Sidebar{
    // inbox, today, info ...
    wxBoxSizer* defaultBoxSizer;

    // project list
    wxBoxSizer* projectsBoxSizer;

    wxButton* inboxButton;
    wxButton* todayButton;
};

struct TaskPanel{
};

class MainFrame : public wxFrame {

public:
    template <class... Args>
    MainFrame(Args... args)
        : wxFrame(std::forward<Args>(args)...) , m_ready(false){
    }

    void setup();


    ~MainFrame() override = default;

private:
    const wxSize DEFAULT_FRAME_DIMS = wxSize(800,600);
    const int SIDEBAR_WIDTH = 200;
private:
    wxBoxSizer* m_mainBoxSizer;
    wxBoxSizer* m_sidebarBoxSizer;
    wxBoxSizer* m_taskPanelBoxSizer;
    Sidebar m_sidebar;
    bool m_ready;
private:
    void setupTaskPanel();
    void setupSideBar();

};

