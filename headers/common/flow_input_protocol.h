#pragma once
#include "flow_input_state.h"

namespace flow::input
{
    class IInputProtocol
    {
    public:
        virtual ~IInputProtocol();
        virtual void UpdateState() = 0;
        virtual InputState GetState() = 0;
        virtual bool initialize() = 0;
    };
}