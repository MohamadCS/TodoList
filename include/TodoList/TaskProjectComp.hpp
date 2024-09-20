#pragma once

#include "Events.hpp"
#include "TaskComp.hpp"

#include <cstdint>

#include <optional>
#include <vector>
#include <wx/event.h>
#include <wx/panel.h>
#include <wx/sizer.h>

struct TaskComp;

class TaskProjectComp : public wxPanel {
public:
    TaskProjectComp(wxWindow* parent, wxWindowID id, std::uint32_t projectId,
                    const std::optional<std::string>& = std::nullopt,
                    std::optional<TaskList*> taskList = std::nullopt, const wxPoint& postion = wxDefaultPosition,
                    const wxSize& size = DEFAULT_SIZE);

    void setProjectName(const wxString&, bool guiOnly = false);
    wxString getProjectName(bool gui = false) const;
    std::uint32_t getProjectId() const;

    void select(wxBoxSizer*);
    void unselect(wxBoxSizer*);

    TaskComp* addTask(wxPanel*, std::optional<Task*> task = std::nullopt);

private:
    std::uint32_t m_projectId;
    bool m_isCurrentProject;
    TaskList* m_taskList;
    std::vector<TaskComp*> m_taskListComp;
    wxBoxSizer* m_mainSizer;
    wxStaticText* m_projectNameText;

    const wxColor unselectedColor = wxColor(250, 250, 250);
    const wxColor selectedColor = wxColor(238, 238, 238);
    const wxColor textColor = wxColor(0, 0, 0);

    inline static const wxSizerFlags SIZER_FLAGS = wxSizerFlags().Proportion(0).Expand().Border(wxALL, 10);
    inline static const wxSize DEFAULT_SIZE = wxSize(50, 50);

    void onPanelLeftClick(wxMouseEvent&);
    void allocateControls();
    void setControlsLayout();
    void setBindings();
    void setStyle();
    void onPaint(wxPaintEvent&);
    void paintNow();
};
