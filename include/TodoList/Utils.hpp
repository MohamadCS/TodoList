#pragma once

#include "AppCore.hpp"
#include <tuple>
#include <utility>

namespace TodoList::Utility {

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
