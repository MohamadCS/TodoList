#include "../include/TodoList/TaskProjectComp.hpp"
#include "wx/colour.h"
#include "wx/osx/stattext.h"
#include <memory>
#include <wx/checkbox.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/log.h>
#include <wx/osx/window.h>
#include <wx/sizer.h>
#include <wx/window.h>

#include <cstdint>
#include <utility>

TaskProjectComp::TaskProjectComp(wxWindow* parent, wxWindowID id, std::uint32_t projectId,
                                 const std::string& projectName, TaskList* taskList, const wxPoint& postion,
                                 const wxSize& size)
    : wxPanel(parent, id, postion, DEFAULT_SIZE),
      taskListComp(std::make_unique<TaskCompList>()),
      projectName(projectName),
      projectId(projectId),
      isCurrentProject(false),
      unselectedColor(wxColor(250, 250, 250)),
      selectedColor(wxColor(238, 238, 238)),
      textColor() {

    SetName("Project Panel");

    SetMinSize(DEFAULT_SIZE);

    auto& appCore = AppCore::instance();

    projectNameText =
        new wxStaticText(this, wxID_ANY, projectName, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);

    taskListComp->m_taskList = (taskList == nullptr) ? appCore.newTaskList() : taskList;

    mainSizer = new wxBoxSizer(wxVERTICAL);

    mainSizer->AddStretchSpacer(1);
    mainSizer->Add(projectNameText, wxSizerFlags(1).Expand().FixedMinSize());
    mainSizer->AddStretchSpacer(1);

    Bind(wxEVT_LEFT_DOWN, &TaskProjectComp::onPanelLeftClick, this);
    Bind(wxEVT_PAINT, &TaskProjectComp::onPaint, this);
    SetSizerAndFit(mainSizer);
    SetWindowStyle(GetWindowStyle() | wxNO_BORDER);
    SetBackgroundColour(unselectedColor);
}

void TaskProjectComp::onPanelLeftClick(wxMouseEvent& ev) {
    auto& appCore = AppCore::instance();

    if (appCore.getCurrentProjectId() == projectId) {
        return;
    }

    wxCommandEvent projectChangeEvent{EVT_CHANGE_PROJECT};
    projectChangeEvent.SetClientData(this);
    wxPostEvent(this, std::move(projectChangeEvent));
    GetParent()->Refresh();
    GetParent()->Layout();
    ev.Skip();
}

void TaskProjectComp::select(wxBoxSizer* sizer) {

    auto add_comp = [&sizer, this](auto&& control) mutable {
        if (control != nullptr) {
            sizer->Add(std::forward<decltype(control)>(control), this->SIZER_FLAGS);
            control->Show();
        }
    };

    for (auto* taskCompPtr : taskListComp->m_taskCompVec) {
        add_comp(taskCompPtr);
    }

    isCurrentProject = true;
    auto& appCore = AppCore::instance();
    appCore.setCurrentProjectId(projectId);
    Refresh();
    Layout();
}

void TaskProjectComp::unselect(wxBoxSizer* sizer) {

    auto detach_comp = [&sizer](auto&& control) mutable {
        if (control != nullptr) {
            sizer->Detach(std::forward<decltype(control)>(control));
            control->Hide();
        }
    };

    for (auto* taskCompPtr : taskListComp->m_taskCompVec) {
        taskCompPtr->cancelTextInsertion();
        detach_comp(taskCompPtr);
    }

    isCurrentProject = false;

    Layout();
}

TaskComp* TaskProjectComp::addTask(Task* task, wxPanel* control) {
    wxLogDebug("Creating a new task");
    auto* taskComp = new TaskComp(control, wxID_ANY, task);
    auto& appCore = AppCore::instance();

    if (appCore.getCurrentProjectId() == projectId) {
        control->GetSizer()->Add(taskComp, SIZER_FLAGS);
    } else {
        taskComp->Hide();
    }
    taskListComp->m_taskCompVec.push_back(taskComp);
    control->Refresh();
    control->Layout();
    return taskComp;
}

void TaskProjectComp::onPaint(wxPaintEvent&) {
    wxPaintDC dc(this);

    // Draw the background
    auto color = isCurrentProject ? selectedColor : unselectedColor;
    dc.SetBrush(color);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRoundedRectangle(wxRect(wxDefaultPosition, GetSize()), 10);
}
