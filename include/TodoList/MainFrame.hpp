#pragma once

#include <utility>
#include <wx/frame.h>
#include <wx/sizer.h>

struct Sidebar{
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

public:

private:

    wxBoxSizer* m_mainBoxSizer;
    wxBoxSizer* m_sidebarBoxSizer;
    wxBoxSizer* m_taskPanelBoxSizer;
    Sidebar m_sidebarControls;
    bool m_ready;
};

