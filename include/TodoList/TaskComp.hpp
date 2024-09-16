#pragma once

#include "AppCore.hpp"
#include <cstdint>
#include <map>
#include <wx/checkbox.h>
#include <wx/dcclient.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/window.h>

// struct TaskCompList{
//     std::string name;
//     std::vector<TaskComp*> m_taskCompVec;
//     TaskList* m_taskList;
//
//     // void show();
//     // void hide();
// };

struct TaskProjectComp;
struct TaskComp : public wxPanel {
public:
    Task* task;
    wxStaticText* taskText;
    wxStaticText* duoDateText;
    wxStaticText* deadLineText;
    wxTextCtrl* textCtrl;
    wxCheckBox* checkBox;
    wxBoxSizer* mainSizer;
    std::map<uint32_t, TaskProjectComp*> taskProjects;

    void setStyle();
    void setBindings();
    void allocateControls();
    void setControlsLayout();
    void onKeyPressedTextCtrl(wxKeyEvent&);
    void onPanelDoubleLeftClick(wxMouseEvent&);
    void cancelTextInsertion();
    void onPaint(wxPaintEvent& event);

    TaskComp(wxWindow* parent, wxWindowID id, Task* taskPtr, std::pair<uint32_t, TaskProjectComp*> taskProject,
             const wxPoint& postion = wxDefaultPosition, const wxSize& size = DEFAULT_SIZE);

    // void hide();
    // void show();

    inline static const wxSize DEFAULT_SIZE = wxSize(200, 200);
};
