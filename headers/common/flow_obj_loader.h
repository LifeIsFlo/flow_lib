#pragma once
#include "flow_mesh.h"
#include "flow_model.h"
#include "flow_vector3.h"
#include <charconv>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <numeric>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

namespace flow
{
    class ObjLoader
    {
    public:
        static Model LoadModel(const std::string& assetLocation);
    };

}