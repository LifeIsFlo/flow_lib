#include "flow_win32_window.h"

namespace flow::win32
{
    HRESULT Win32Window::Show(HINSTANCE& hInstance, WNDPROC wndProc)
    {
        WNDCLASSEX wc{};
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.lpfnWndProc = wndProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = m_className;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.style = m_wndClassStyle;

        RegisterClassEx(&wc);

        RECT rect = {0, 0, m_width, m_height};
        AdjustWindowRect(&rect, m_wndStyle, FALSE);

        m_hwnd = CreateWindowEx(0, m_className, m_wndTitle, m_wndStyle, 0, 0, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr);

        if (!m_hwnd)
            return E_FAIL;

        ShowWindow(m_hwnd, SW_SHOW);

        return S_OK;
    }

    bool Win32Window::tick()
    {
        MSG msg{};
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                return false;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return true;
    }
}