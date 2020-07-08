#pragma once


#include "aqua2/keyinput.h"


struct KeyboardJoy {
    bool initialized;
    std::vector<float> axis;
    std::vector<bool> buttons;
    std::vector<bool> old_buttons;

    KeyboardJoy(): initialized(false) {
        axis.resize(2);
        buttons.resize(10);
        old_buttons.resize(10);
    }

    ~KeyboardJoy() {
        if (initialized) {
            ssr::aqua2::exit_scr();
        }
    }

};

inline void initialize(KeyboardJoy& joy) {
    ssr::aqua2::init_scr();
    joy.initialized = true;
}

inline void update(KeyboardJoy& joy) {
    for(int i = 0;i < 10;i++) {
        joy.old_buttons[i] = joy.buttons[i];
        joy.buttons[i] = false;
    }
    joy.axis[0] = 0;
    joy.axis[1] = 0;
    while(ssr::aqua2::kbhit()) {
        ssr::aqua2::AQUA2_KEY key = ssr::aqua2::getch();
        if (key < 0) break;
        switch (key) {
        case ssr::aqua2::AQUA2_KEY_UP:
            joy.axis[1] = -1.0;
            break;
        case ssr::aqua2::AQUA2_KEY_DOWN:
            joy.axis[1] = 1.0;
            break;
        case ssr::aqua2::AQUA2_KEY_LEFT:
            joy.axis[0] = -1.0;
            break;
        case ssr::aqua2::AQUA2_KEY_RIGHT:
            joy.axis[0] = 1.0;
            break;
        case '0': 
            joy.buttons[0] = true;
            break;
        case '1': 
            joy.buttons[1] = true;
            break;
        }

        
    }
}
