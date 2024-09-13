#pragma once

class AppCore {
public:
    static AppCore& instance();

private:
    AppCore() = default;
};
