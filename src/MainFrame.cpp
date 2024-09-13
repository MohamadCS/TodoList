#include "../include/TodoList/MainFrame.hpp"
#include "wx/app.h"
#include "wx/event.h"
#include "wx/log.h"
#include "wx/sizer.h"

void MainFrame::setup() {

    if (m_ready) {
        wxLogMessage("Already allocated main frame");
        wxExit();
    }

    m_sidebarBoxSizer = new wxBoxSizer(wxVERTICAL);
    m_taskPanelBoxSizer = new wxBoxSizer(wxVERTICAL);
    m_mainBoxSizer = new wxBoxSizer(wxHORIZONTAL);

    SetSizerAndFit(m_mainBoxSizer);
}
