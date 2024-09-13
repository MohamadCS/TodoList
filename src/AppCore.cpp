#include "../include/TodoList/AppCore.hpp"


AppCore& AppCore::instance(){
    static AppCore app;
    return app;
}


