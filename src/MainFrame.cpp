#include "../include/TodoList/MainFrame.hpp"
#include "../include/TodoList/AppCore.hpp"
#include "../include/TodoList/TaskComp.hpp"
#include "wx/app.h"
#include "wx/event.h"
#include "wx/log.h"
#include "wx/sizer.h"
#include <wx/button.h>
#include <wx/statline.h>

void MainFrame::setup() {

    if (m_ready) {
        wxLogMessage("Already allocated main frame");
        wxExit();
    }

    setupSideBar();
    setupTaskPanel();

    m_mainBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    m_mainBoxSizer->Add(m_sidebarBoxSizer,
                        wxSizerFlags().Proportion(0).Expand());
    m_mainBoxSizer->Add(m_taskPanelBoxSizer,
                        wxSizerFlags().Proportion(2).Expand());

    SetSizerAndFit(m_mainBoxSizer);
    SetClientSize(DEFAULT_FRAME_DIMS);
}

void MainFrame::setupSideBar() {
    m_sidebarBoxSizer = new wxBoxSizer(wxVERTICAL);
    m_sidebar.projectsBoxSizer = new wxBoxSizer(wxVERTICAL);
    m_sidebar.defaultBoxSizer = new wxBoxSizer(wxVERTICAL);

    m_sidebarBoxSizer->Add(
        m_sidebar.defaultBoxSizer,
        wxSizerFlags().Proportion(0).Expand().FixedMinSize());

    m_sidebarBoxSizer->Add(m_sidebar.projectsBoxSizer,
                           wxSizerFlags().Proportion(2).Expand());

    m_sidebar.inboxButton = new wxButton(
        this, wxID_ANY, "Inbox", wxDefaultPosition, wxSize(300, SIDEBAR_WIDTH));
    m_sidebar.todayButton = new wxButton(
        this, wxID_ANY, "today", wxDefaultPosition, wxSize(300, SIDEBAR_WIDTH));

    m_sidebar.defaultBoxSizer->Add(m_sidebar.inboxButton,
                                   wxSizerFlags().Expand());
    m_sidebar.defaultBoxSizer->Add(m_sidebar.todayButton,
                                   wxSizerFlags().FixedMinSize());
}

void MainFrame::setupTaskPanel() {
    m_taskPanelBoxSizer = new wxBoxSizer(wxVERTICAL);
    auto& appCore = AppCore::instance();

    auto* taskPtr = appCore.newTask({}, {}, "hello", "hello", true);

    for (int i = 0; i < 30; ++i) {
        auto* hello = new TaskComp(this, wxID_ANY, taskPtr);
        m_taskPanelBoxSizer->Add(hello, wxSizerFlags().Expand().Border(wxALL, 10));
    }
}
