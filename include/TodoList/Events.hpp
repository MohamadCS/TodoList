#pragma once

#include "wx/event.h"

/*Event Decleration*/
wxDECLARE_EVENT(EVT_CHANGE_PROJECT,wxCommandEvent);
wxDECLARE_EVENT(EVT_TASK_FINISHED,wxCommandEvent);






/*Events Ids*/
const wxEventType EVT_CHANGE_PROJECT_ID = wxNewEventType();
const wxEventType EVT_TASK_FINISHED_ID = wxNewEventType();
