#pragma once
#include <flow_fwd.h>
#include <windows.h>

namespace flow::win32
{
    struct Win32Window
    {
        HWND m_hwnd;

    public:
        int m_height = 600;
        int m_width = 800;
        LPCSTR m_wndTitle = "FlowWindow";
        LPCSTR m_className = "FlowWindowClass";
        UINT m_wndClassStyle = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        UINT m_wndStyle = WS_POPUP | WS_VISIBLE;
        HRESULT Show(HINSTANCE& hInstance, WNDPROC wndProc);
        bool tick();
        HWND GetHandle()
        {
            return m_hwnd;
        }
        HWND* GetHandleAddress()
        {
            return &m_hwnd;
        }
    };
}