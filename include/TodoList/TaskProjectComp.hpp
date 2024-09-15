#pragma once

#include "Events.hpp"
#include "TaskComp.hpp"
#include "wx/generic/panelg.h"
#include "wx/osx/stattext.h"

#include <cstdint>

#include <wx/event.h>
#include <wx/panel.h>
#include <wx/sizer.h>

struct TaskProjectComp : public wxPanel {
    wxBoxSizer* mainSizer;
    TaskCompList* taskListComp;
    wxStaticText* projectNameText;
    wxString projectName;

    wxColor unselectedColor; 
    wxColor selectedColor; 
    wxColor textColor;

    std::uint32_t projectId;

    TaskProjectComp(wxWindow* parent, wxWindowID id, std::uint32_t projectId, const std::string& projectName,
                    TaskList* taskList = nullptr, const wxPoint& postion = wxDefaultPosition,
                    const wxSize& size = DEFAULT_SIZE);

    inline static const wxSize DEFAULT_SIZE = wxSize(200,50);

    void onPanelLeftClick(wxMouseEvent&);
    void select(wxBoxSizer*);
    void unselect(wxBoxSizer*);
    void addTask(Task* newTask, wxPanel*);

    void allocateControls();
    void setControlsLayout();
    void setBindings();
    void setStyle();
};
