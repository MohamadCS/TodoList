#pragma once

#include "TaskComp.hpp"
#include "wx/event.h"
#include "wx/sizer.h"
#include <cstdint>
#include <wx/panel.h>

wxDECLARE_EVENT(EVT_CHANGE_PROJECT,wxCommandEvent);
const wxEventType EVT_CHANGE_PROJECT_ID = wxNewEventType();

struct TaskProjectComp : public wxPanel {
    wxStaticText* projectNameText;
    wxBoxSizer* mainSizer;
    TaskCompList* taskListComp;
    TaskList* taskList;
    std::uint32_t projectId;
    TaskProjectComp(wxWindow* parent, wxWindowID id, TaskCompList* taskListPtr,std::uint32_t projectId,
                    const wxPoint& postion = wxDefaultPosition, const wxSize& size = DEFAULT_SIZE);
    inline static const wxSize DEFAULT_SIZE = wxSize(200, 200);

    void onPanelLeftClick(wxMouseEvent&);
    void select(wxBoxSizer*);
    void unselect(wxBoxSizer*);
    void addTask(Task* newTask);

};
