#pragma once

#include "AppCore.hpp"
#include <tuple>
#include <utility>
#include <iostream>
#include <format>

#define ENABLE_DEBUG 1

#if defined(ENABLE_DEBUG) && ENABLE_DEBUG == 1
#define DEBUG(line) line
#define LOG(...) std::cout << "(" << __FUNCTION__ << "," << __LINE__ << "):" << std::format(__VA_ARGS__) << '\n'
#else
#define DEBUG(line)
#define LOG(line)
#endif

namespace TodoList::Utility {

std::filesystem::path makePath(const std::vector<std::string_view>& strVec);
std::string timePointToStr(const Core::TimePoint& timePoint);

bool isToday(const TodoList::Core::TimePoint& date);

template <class T>
void refresh(const T& comp);

template <class... Comps>
void refresh(const std::tuple<Comps...>& components);

template <class T>
void refresh(const T& comp) {
    comp->Layout();
    comp->Refresh();
}

template <class... Comps>
void refresh(const std::tuple<Comps...>& components) {
    std::apply([](auto&&... comps) { (refresh(std::forward<decltype(comps)>(comps)), ...); }, components);
}

} // namespace TodoList::Utility
