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
    std::unique_ptr<TaskCompList> taskListComp; 
    wxStaticText* projectNameText;
    wxString projectName;
    bool isCurrentProject;

    wxColor unselectedColor; 
    wxColor selectedColor; 
    wxColor textColor;

    inline static const wxSizerFlags SIZER_FLAGS  = wxSizerFlags().Proportion(0).Expand().Border(wxALL, 10);

    std::uint32_t projectId;

    TaskProjectComp(wxWindow* parent, wxWindowID id, std::uint32_t projectId, const std::string& projectName,
                    TaskList* taskList = nullptr, const wxPoint& postion = wxDefaultPosition,
                    const wxSize& size = DEFAULT_SIZE);

    inline static const wxSize DEFAULT_SIZE = wxSize(50,50);

    void onPanelLeftClick(wxMouseEvent&);
    void select(wxBoxSizer*);
    void unselect(wxBoxSizer*);
    TaskComp* addTask(Task* newTask, wxPanel*);

    void allocateControls();
    void onPaint(wxPaintEvent&);
    void paintNow();
    void setControlsLayout();
    void setBindings();
    void setStyle();
};
