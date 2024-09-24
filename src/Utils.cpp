#include "../include/TodoList/Utils.hpp"
#include <ctime>

namespace TodoList::Utility {

std::string timePointToStr(const Core::TimePoint& timePoint) {
    return std::format("{:%d %B}", timePoint);
}

// FIXME: Always returns true for some reason.
bool isToday(const TodoList::Core::TimePoint& date) {
    auto now = std::chrono::system_clock::now();

    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto date_time_t = std::chrono::system_clock::to_time_t(date);

    std::tm* now_tm = std::localtime(&now_time_t);
    std::tm* date_tm = std::localtime(&date_time_t);

    return (now_tm->tm_year == date_tm->tm_year) && (now_tm->tm_mon == date_tm->tm_mon) &&
           (now_tm->tm_mday == date_tm->tm_mday);
}

} // namespace TodoList::Utility
