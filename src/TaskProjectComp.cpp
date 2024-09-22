#include "../include/TodoList/TaskProjectComp.hpp"
#include "../include/TodoList/Utils.hpp"
#include <optional>
#include <tuple>
#include <utility>
#include <wx/checkbox.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/window.h>

namespace TodoList::Gui {

TaskProjectComp::TaskProjectComp(wxWindow* parent, wxWindowID id, const std::optional<std::string>& projectName,
                                 std::optional<Core::TaskList*> taskList, const wxPoint& postion, const wxSize& size)
    : wxPanel(parent, id, postion, DEFAULT_SIZE),
      m_isCurrentProject(false) {

    SetName("Project Panel");
    SetSizer(new wxBoxSizer(wxVERTICAL));

    auto& appCore = Core::App::instance();
    m_taskList = taskList.value_or(appCore.newTaskList());
    m_taskList->name = projectName.value_or(m_taskList->name);

    allocateControls();
    setControlsLayout();
    setBindings();
    setStyle();

    Fit();
}

void TaskProjectComp::allocateControls() {
    m_projectNameText = new wxStaticText(this, wxID_ANY, getProjectName(), wxDefaultPosition, wxDefaultSize,
                                         wxALIGN_CENTER_HORIZONTAL | wxST_ELLIPSIZE_END);
}

void TaskProjectComp::setBindings() {
    Bind(wxEVT_LEFT_DOWN, &TaskProjectComp::onPanelLeftClick, this);
    Bind(wxEVT_PAINT, &TaskProjectComp::onPaint, this);
}

void TaskProjectComp::setControlsLayout() {
    SetMinSize(DEFAULT_SIZE);

    GetSizer()->AddStretchSpacer(1);
    GetSizer()->Add(m_projectNameText, wxSizerFlags(1).Expand().FixedMinSize());
    GetSizer()->AddStretchSpacer(1);
}

void TaskProjectComp::setStyle() {
    SetBackgroundColour(UNSELECTED_COLOR);
    SetWindowStyle(GetWindowStyle() | wxNO_BORDER);
}

void TaskProjectComp::onPanelLeftClick(wxMouseEvent& ev) {
    auto& appCore = Core::App::instance();

    if (appCore.getCurrentProjectId() == getProjectId()) {
        return;
    }

    wxCommandEvent projectChangeEvent{EVT_CHANGE_PROJECT};
    projectChangeEvent.SetClientData(this);
    wxPostEvent(this, std::move(projectChangeEvent));
    Utility::refresh(GetParent());
    ev.Skip();
}

void TaskProjectComp::showProject(wxSizer* sizer) {

    auto add_comp = [&sizer, this](auto&& control) mutable {};

    for (auto* taskCompPtr : m_taskListComp) {
        if (!taskCompPtr->task->checked) {
            sizer->Add(taskCompPtr, SIZER_FLAGS);
            taskCompPtr->Show();
        }
    }

    m_isCurrentProject = true;
    auto& appCore = Core::App::instance();
    appCore.setCurrentProjectId(getProjectId());
    Utility::refresh(this);
}

void TaskProjectComp::hideProject(wxSizer* sizer) {

    for (auto* taskCompPtr : m_taskListComp) {
        taskCompPtr->cancelTextInsertion();
        sizer->Detach(taskCompPtr);
        taskCompPtr->Hide();
    }

    m_isCurrentProject = false;
    Layout();
}

std::expected<TaskComp*, TaskProjectComp::Error> TaskProjectComp::addTask(wxPanel* control,
                                                                          std::optional<Core::Task*> optTask) {
    wxLogDebug("Creating a new task");

    auto& appCore = Core::App::instance();
    auto* task = optTask.value_or(appCore.newTask("", "", false, m_taskList));
    auto* taskComp = new TaskComp(control, wxID_ANY, task, {getProjectId(), this});

    if (appCore.getCurrentProjectId() == getProjectId()) {
        control->GetSizer()->Add(taskComp, SIZER_FLAGS);
    } else {
        taskComp->Hide();
    }

    m_taskListComp.push_back(taskComp);

    Utility::refresh(std::make_tuple(taskComp, this));
    return taskComp;
}

void TaskProjectComp::onPaint(wxPaintEvent&) {
    wxPaintDC dc(this);
    auto color = m_isCurrentProject ? SELECTED_COLOR : UNSELECTED_COLOR;
    dc.SetBrush(color);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRoundedRectangle(wxRect(wxDefaultPosition, GetSize()), 10);
}

void TaskProjectComp::setProjectName(const wxString& newName, bool guiOnly) {

    if (!guiOnly) {
        m_taskList->name = newName;
    }

    m_projectNameText->SetLabel(newName);

    Utility::refresh(this);
}

wxString TaskProjectComp::getProjectName(bool gui) const {
    if (gui) {
        return m_projectNameText->GetLabel();
    }
    return m_taskList->name;
}

Core::ID TaskProjectComp::getProjectId() const {
    return m_taskList->taskListId;
}

} // namespace TodoList::Gui
