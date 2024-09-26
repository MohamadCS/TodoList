#pragma once

#include "AppCore.hpp"
#include <format>
#include <functional>
#include <iostream>
#include <tuple>
#include <utility>

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

/************************Template functions implementation************************/

template <class T>
void refresh(const T& comp) {
    comp->Layout();
    comp->Refresh();
}

template <class... Comps>
void refresh(const std::tuple<Comps...>& components) {
    std::apply([](auto&&... comps) { (refresh(std::forward<decltype(comps)>(comps)), ...); }, components);
}

template <class Iterator>
std::string convertContainerToListStr(Iterator begin, Iterator end, std::function<std::string(Iterator)> itemFormat,
                                      const std::string& sep = ",",
                                      const std::optional<std::vector<std::string>>& sepList = std::nullopt) {
    std::stringstream columnsQuery;
    std::function<std::string(int)> genSep;

    if (sepList.has_value()) {
        genSep = [&sepList](int i) { return sepList.value()[i]; };
    } else {
        genSep = [&sep](int i) { return sep; }; // PERF: Find a way to replace the function call.
    }

    int idx = 0;
    for (Iterator it = begin; it != end; ++it) {
        columnsQuery << itemFormat(it);

        if (std::next(it) != end) {
            columnsQuery << genSep(idx++);
        }
    }

    return columnsQuery.str();
}
} // namespace TodoList::Utility
