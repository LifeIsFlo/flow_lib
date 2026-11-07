#pragma once
#include "flow_io.h"
#include <chrono>
#include <string>
#include <vector>

namespace flow::util
{
    void string_split(std::string_view in, std::vector<std::string_view>& out, char sep);

    struct StageTimer
    {
        const char* name;
        std::chrono::high_resolution_clock::time_point t0 = std::chrono::high_resolution_clock::now();
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

        void start()
        {
            t0 = std::chrono::high_resolution_clock::now();
        }

        float stop()
        {
            t1 = std::chrono::high_resolution_clock::now();
            auto us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
            flow::io::Console::WriteLine(std::string(name) + ": " + std::to_string(static_cast<float>(us) / 1000.0f) + " ms");
            return static_cast<float>(us) / 1000.0f;
        }
    };
}
