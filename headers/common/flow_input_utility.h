#pragma once
#include "flow_input_protocol.h"
#include <concepts>

namespace flow::input
{
    class InputUtility
    {
    public:
        template <std::derived_from<IInputProtocol> T, typename... Args>
        static T* CreateProtocol(Args&&... args)
        {
        }
    }
}