#pragma once
#include "flow_float4x4.h"

namespace flow
{
    struct transform_buffer
    {
        float4x4a transformMatrix;
        float4x4a rotationMatrix;
    };
}