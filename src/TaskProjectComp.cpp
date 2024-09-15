#include "../include/TodoList/TaskProjectComp.hpp"
#include "wx/colour.h"
#include "wx/osx/stattext.h"
#include "wx/stattext.h"

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
      taskListComp(new TaskCompList),
      projectName(projectName),
      projectId(projectId),
      unselectedColor(wxColor(242, 233, 222)),
      selectedColor(wxColor(206, 202, 205)),
      textColor() {

    SetName("Project Panel");

    SetMinSize(DEFAULT_SIZE);

    auto& appCore = AppCore::instance();

    projectNameText =
        new wxStaticText(this, wxID_ANY, projectName, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);

    taskListComp->m_taskList = (taskList == nullptr) ? appCore.newTaskList() : taskList;
    mainSizer = new wxBoxSizer(wxHORIZONTAL);

    mainSizer->AddStretchSpacer(1);
    mainSizer->Add(projectNameText, wxSizerFlags(1).CenterVertical());
    mainSizer->AddStretchSpacer(1);
    Layout();

    Bind(wxEVT_LEFT_DOWN, &TaskProjectComp::onPanelLeftClick, this);
    SetWindowStyle(GetWindowStyle() | wxBORDER_DOUBLE);
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
    Refresh();
}

void TaskProjectComp::select(wxBoxSizer* sizer) {
    auto sizerFlags = wxSizerFlags().Proportion(1).Expand().Border(wxALL, 10);

    auto add_comp = [&sizer, &sizerFlags](auto&& control) mutable {
        if (control != nullptr) {
            sizer->Add(std::forward<decltype(control)>(control), sizerFlags);
            control->Show();
        }
    };

    for (auto* taskCompPtr : taskListComp->m_taskCompVec) {
        add_comp(taskCompPtr);
    }

    auto& appCore = AppCore::instance();
    appCore.setCurrentProjectId(projectId);
    SetBackgroundColour(selectedColor);
    Refresh();
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

    SetBackgroundColour(unselectedColor);
    Refresh();
}

void TaskProjectComp::addTask(Task* task, wxPanel* control) {
    wxLogDebug("Creating a new task");
    auto* taskComp = new TaskComp(control, wxID_ANY, task);
    auto& appCore = AppCore::instance();

    if (appCore.getCurrentProjectId() == projectId) {
        taskComp->Show();
        control->GetSizer()->Add(taskComp);
    } else {
        taskComp->Hide();
    }
    taskListComp->m_taskCompVec.push_back(taskComp);
    Refresh();
}
