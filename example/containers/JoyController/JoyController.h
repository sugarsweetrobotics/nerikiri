#pragma once


#include "aqua2/gamepad.h"


struct JoyController {
    std::shared_ptr<ssr::aqua2::GamePad> gamepad;

    JoyController(): gamepad(nullptr) {}
};
