#pragma once

#include <string>
#include <iostream>

enum DEBUG_LEVEL {
        ALL = 0xFF,
        NONE = 0,
        OTHER = 1,
        BEHAVIOUR = 2,
        ERROR_HANDLING = 4,
        IO = 8,
};

#define SELECTED_LEVELS NONE

template<DEBUG_LEVEL level, typename T>
void debug_msg(const T msg) {
        if constexpr (level & (SELECTED_LEVELS)) {
                std::cout << msg << "\n";
        }
}