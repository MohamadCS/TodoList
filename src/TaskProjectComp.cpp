#include "../include/TodoList/TaskProjectComp.hpp"
#include "wx/checkbox.h"
#include "wx/event.h"
#include "wx/gdicmn.h"
#include "wx/log.h"
#include "wx/sizer.h"
#include <cstdint>

wxDEFINE_EVENT(EVT_CHANGE_PROJECT, wxCommandEvent);

TaskProjectComp::TaskProjectComp(wxWindow* parent, wxWindowID id, TaskCompList* taskPtr, std::uint32_t projectId,
                                 const wxPoint& postion, const wxSize& size)
    : wxPanel(parent, id, postion, size),
      taskListComp(taskPtr),
      taskList(AppCore::instance().newTaskList()),
      projectId(projectId) {

    if (taskListComp == nullptr) {
        wxLogDebug("Got nullptr");
        exit(0);
    }
    SetMinSize(DEFAULT_SIZE);

    mainSizer = new wxBoxSizer(wxVERTICAL);
    projectNameText = new wxStaticText(this, wxID_ANY, taskListComp->name);

    mainSizer->Add(projectNameText, wxSizerFlags().Expand());

    this->Bind(wxEVT_LEFT_DOWN, &TaskProjectComp::onPanelLeftClick, this);

    this->SetWindowStyle(GetWindowStyle() | wxBORDER_DOUBLE);
    SetBackgroundColour(*wxWHITE);
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

    for (auto* taskCompPtr : taskListComp->m_taskCompVec) {
        sizer->Add(taskCompPtr);
        taskCompPtr->Show();
    }

    auto& appCore = AppCore::instance();
    appCore.setCurrentProjectId(projectId);
    SetBackgroundColour(*wxLIGHT_GREY);
    Refresh();
}

void TaskProjectComp::unselect(wxBoxSizer* sizer) {
    for (auto* taskCompPtr : taskListComp->m_taskCompVec) {
        taskCompPtr->cancelTextInsertion();
        sizer->Detach(taskCompPtr);
        taskCompPtr->Hide();
    }

    SetBackgroundColour(*wxWHITE);
    Refresh();
}

void TaskProjectComp::addTask(Task* task) {
    auto* taskComp = new TaskComp(GetParent(), wxID_ANY, task);
    auto& appCore = AppCore::instance();
    if(appCore.getCurrentProjectId() == projectId){
        taskComp->Show();
    } else{
        taskComp->Hide();
    }
    taskListComp->m_taskCompVec.push_back(taskComp);
}
