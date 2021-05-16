#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

#include <juiz/juiz.h>
#include <juiz/container.h>

#include "TurtleSim.h"

using namespace juiz;

extern "C" {

    static void updateTurtlePos(Turtle* t, double dt) {
        double dx = t->vx * dt;
        double dth = t->vth * dt;
        t->x = t->x + dx * cos(t->th + dth / 2);
        t->y = t->y + dx * sin(t->th + dth / 2);
        t->th = t->th + dth;
        while (t->th > M_PI) t->th -= M_PI*2;
        while (t->th < -M_PI) t->th += M_PI*2;
        //std::cout << "Turtle(" << t->name << " (" << t->x << "," << t->y << "," << t->th << ") )" << std::endl;
    }

    NK_OPERATION  void* TurtleSim_update() {
        return containerOperationFactory<TurtleSim>(
            {
                {"typeName", "update"},
                {"defaultArg", {
                    {"interval_ms", "10"}
                }},
            },
            [](auto& container, auto arg) {
                //std::cout << "TurtleSim_update called." << std::endl;
                auto now = std::chrono::system_clock::now();
                auto dt = std::chrono::duration_cast<std::chrono::microseconds>(now - container.now).count() / 1000000.0;
                container.now = now;
                for(auto& t : container.turtles) {
                    updateTurtlePos(&t, dt);
                }
                return Value(arg);
            });
    }

}