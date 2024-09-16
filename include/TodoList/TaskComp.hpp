#pragma once

#include "AppCore.hpp"

#include <wx/checkbox.h>
#include <wx/dcclient.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/window.h>

struct TaskComp;

struct TaskCompList{
    std::string name;
    std::vector<TaskComp*> m_taskCompVec;
    TaskList* m_taskList;

    void show();
    void hide();
};

struct TaskComp : public wxPanel {
public:

    Task* m_task;
    wxStaticText* m_taskText;
    wxStaticText* m_duoDateText;
    wxStaticText* m_deadLineText;
    wxTextCtrl* m_textCtrl;
    wxCheckBox* m_checkBox;
    wxBoxSizer* m_mainSizer;

    std::vector<TaskCompList*> m_taskCompLists;

    void setStyle();
    void setBindings();
    void allocateControls();
    void setControlsLayout();
    void onKeyPressedTextCtrl(wxKeyEvent&);
    void onPanelDoubleLeftClick(wxMouseEvent&);
    void cancelTextInsertion();
    void onPaint(wxPaintEvent& event);

    TaskComp(wxWindow* parent, wxWindowID id, Task* taskPtr,
             const wxPoint& postion = wxDefaultPosition,
             const wxSize& size = DEFAULT_SIZE);

    void hide();
    void show();

    inline static const wxSize DEFAULT_SIZE = wxSize(200, 200);
};
