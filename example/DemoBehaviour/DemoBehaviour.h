#pragma once

#include <chrono>
#include <string>
#include <vector>

#include <juiz/geometry.h>

struct DemoBehaviour {
    int count;
    juiz::Pose3D pose;
    DemoBehaviour(): count(0) {}
};
