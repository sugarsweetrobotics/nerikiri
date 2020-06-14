#pragma once

#include <chrono>
#include <string>
#include <vector>

struct Turtle {
    std::string name;
    double vx;
    double vy;
    double vth;

    double x;
    double y;
    double th;

    Turtle(const std::string& name ) : name(name), vx(0), vy(0), vth(0), x(0), y(0), th(0) {}
};

struct TurtleSim {
    std::chrono::system_clock::time_point start;
    std::chrono::system_clock::time_point now;
    std::vector<Turtle> turtles;

    TurtleSim(): start(std::chrono::system_clock::now()) {
        turtles.push_back(Turtle("turtle0"));
    }
};