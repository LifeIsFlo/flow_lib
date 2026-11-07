#pragma once
#include "flow_dxfwd.h"
#include "flow_input_protocol.h"
#include "flow_input_state.h"
#include <vector>

namespace flow::win32
{
    class Win32Input : public flow::input::IInputProtocol
    {
    public:
        bool initialize() override;
        void UpdateState() override;
        flow::input::InputState GetState() override;

        void CleanUp();
    };
}