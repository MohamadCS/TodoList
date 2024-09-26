#pragma once

#include "Events.hpp"
#include "TaskComp.hpp"

#include <expected>
#include <optional>
#include <vector>
#include <wx/event.h>
#include <wx/panel.h>
#include <wx/sizer.h>

namespace TodoList::Gui {
struct TaskComp;

class TaskProjectComp : public wxPanel {
public:
    enum class Error {
        TASK_EXISTS
    };

    TaskProjectComp(wxWindow* parent, wxWindowID id, const std::optional<std::string>& = std::nullopt,
                    std::optional<Core::TaskList*> taskList = std::nullopt, const wxPoint& postion = wxDefaultPosition,
                    const wxSize& size = DEFAULT_SIZE);

    /**
     * @brief Set project name
     *
     * @param guiOnly true if we want to set Gui elements text without Core::TaskList.
     */
    void setProjectName(const wxString&, bool guiOnly = false);

    /**
     * @brief Get project name Id
     *
     * @param gui false to get the Core::TaskList name, true gets the wxStaticText name.
     * @return
     */
    wxString getProjectName(bool gui = false) const;

    /**
     * @brief Returns Core::TaskList id.
     */
    Core::ID getProjectId() const;

    std::expected<TaskComp*, Error> addTask(wxPanel*, std::optional<Core::Task*> task = std::nullopt);

    void showProject(wxSizer*);

    void hideProject(wxSizer*);

    std::vector<TaskComp*>& getTaskCompList();
    const std::vector<TaskComp*>& getTaskCompList() const;

private:
    bool m_isCurrentProject;
    Core::TaskList* m_taskList;
    std::vector<TaskComp*> m_taskListComp;
    wxStaticText* m_projectNameText;

    inline static const auto UNSELECTED_COLOR = wxColor(250, 250, 250);
    inline static const auto SELECTED_COLOR = wxColor(238, 238, 238);
    inline static const auto TEXT_COLOR = wxColor(0, 0, 0);

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

} // namespace TodoList::Gui
