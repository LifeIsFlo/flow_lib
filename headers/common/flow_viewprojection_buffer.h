#pragma once
#include "flow_float4x4.h"
#include "flow_vector3.h"

namespace flow
{
    struct viewprojection_buffer
    {
        float4x4a view;
        float4x4a projection;
    };
}