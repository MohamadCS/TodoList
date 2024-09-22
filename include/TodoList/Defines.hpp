#pragma once
#include "AppCore.hpp"

namespace TodoList::Utility {

enum class Views {
    MAIN_FRAME = 0,
    SETTINGS_FRAME,
    LOGIN_FRAME
};

// Project idx
constexpr Core::ID TODAY_IDX = 0;
constexpr Core::ID INBOX_IDX = 1;

} // namespace TodoList::Utility
