#include "flow_win32_input.h"
#include "flow_keys.h"
#include <vector>
#include <windows.h>

#include "flow_io.h"

namespace flow::win32
{
    std::array<input::Keys, 256> Win32Input::g_vkToKey = make_vk_to_key();
    Win32Input* Win32Input::m_instance = nullptr;

    bool Win32Input::connect()
    {
        if (!m_instance && hwndTarget)
        {
            g_vkToKey = make_vk_to_key();
            m_instance = this;

            RAWINPUTDEVICE rid[2]{};

            rid[0].usUsagePage = 0x01; // generic desktop
            rid[0].usUsage = 0x06;     // keyboard
            rid[0].hwndTarget = hwndTarget;

            rid[1].usUsagePage = 0x01;
            rid[1].usUsage = 0x02; // mouse
            rid[1].hwndTarget = hwndTarget;

            if (!RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE)))
            {
                std::cout << "RegisterRawInputDevices failed: " << GetLastError() << "\n";
                return false;
            }

            return true;
        }
        else
        {
            if (hwndTarget)
            {
                std::cout << "Win32Input already initialized!\n";
            }
            else
            {
                std::cout << "Specify hwndTarget!\n";
            }
            return false;
        }
    }

    void Win32Input::on_raw_input(HRAWINPUT h)
    {
        RAWINPUT raw;
        UINT size = sizeof(raw);
        if (GetRawInputData(h, RID_INPUT, &raw, &size, sizeof(RAWINPUTHEADER)) == (UINT)-1)
            return;

        if (raw.header.dwType == RIM_TYPEKEYBOARD)
        {
            const RAWKEYBOARD& kb = raw.data.keyboard;
            UINT vk = kb.VKey;
            if (vk == 255)
                return;

            const bool e0 = (kb.Flags & RI_KEY_E0) != 0;
            const bool down = (kb.Flags & RI_KEY_BREAK) == 0;

            switch (vk)
            {
            case VK_SHIFT:
                vk = (kb.MakeCode == 0x36) ? VK_RSHIFT : VK_LSHIFT;
                break;
            case VK_CONTROL:
                vk = e0 ? VK_RCONTROL : VK_LCONTROL;
                break;
            case VK_MENU:
                vk = e0 ? VK_RMENU : VK_LMENU;
                break;
            }

            auto key = g_vkToKey[vk];

            if (vk == VK_RETURN)
            {
                key = e0 ? input::Keys::ENTER : input::Keys::RETURN;
            }

            m_pending.push_back({key, down});
        }
        else if (raw.header.dwType == RIM_TYPEMOUSE)
        {
            const RAWMOUSE& m = raw.data.mouse;

            // Relative movement (the normal case for a mouse)
            if ((m.usFlags & MOUSE_MOVE_ABSOLUTE) == 0)
            {
                m_pendingMouse.deltaPos += Vector2(m.lLastX, -m.lLastY);
            }
            // else: absolute device (remote desktop, pen/touch emulation); lLastX/Y
            // are 0..65535 normalized coordinates, not deltas.

            for (const auto& b : kButtons)
            {
                if (m.usButtonFlags & b.down)
                {
                    m_pendingMouse.pressed |= b.bit;
                }
                if (m.usButtonFlags & b.up)
                {
                    m_pendingMouse.released |= b.bit;
                }
            }

            if (m.usButtonFlags & RI_MOUSE_WHEEL)
                m_pendingMouse.scroll += static_cast<SHORT>(m.usButtonData) / (float)WHEEL_DELTA;
        }
    }

    void Win32Input::poll_platform()
    {
        POINT p;
        if(GetCursorPos(&p)){
            m_pendingMouse.absolutePos = Vector2((float)p.x, (float)p.y);
            ScreenToClient(hwndTarget, &p);
            m_pendingMouse.clientPos = Vector2((float)p.x, (float)p.y);
        }
    }
}