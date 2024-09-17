#pragma once

#include "Events.hpp"
#include "TaskComp.hpp"

#include <cstdint>

#include <vector>
#include <wx/event.h>
#include <wx/panel.h>
#include <wx/sizer.h>

struct TaskComp;

struct TaskProjectComp : public wxPanel {
    std::uint32_t projectId;
    bool isCurrentProject;
    TaskList* taskList;
    wxString projectName;

    std::vector<TaskComp*> taskListComp;
    wxBoxSizer* mainSizer;
    wxStaticText* projectNameText;

    wxColor unselectedColor = wxColor(250, 250, 250);
    wxColor selectedColor = wxColor(238, 238, 238);
    wxColor textColor = wxColor(0, 0, 0);

    inline static const wxSizerFlags SIZER_FLAGS = wxSizerFlags().Proportion(0).Expand().Border(wxALL, 10);

    TaskProjectComp(wxWindow* parent, wxWindowID id, std::uint32_t projectId, const std::string& projectName,
                    std::optional<TaskList*> taskList = std::nullopt, const wxPoint& postion = wxDefaultPosition,
                    const wxSize& size = DEFAULT_SIZE);

    inline static const wxSize DEFAULT_SIZE = wxSize(50, 50);

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
