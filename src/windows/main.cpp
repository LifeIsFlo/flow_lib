#include <flow_io.h>
#include <flow_win32_input.h>
#include <flow_win32_window.h>
#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    switch (uMsg)
    {
    case WM_INPUT:
        flow::win32::Win32Input::get_instance()->on_raw_input((HRAWINPUT)lParam);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main()
{
    HINSTANCE hInstance = GetModuleHandle(nullptr);

    flow::win32::Win32Window window{};
    window.Show(hInstance, WindowProc);

    flow::win32::Win32Input win32Input{};
    win32Input.hwndTarget = window.GetHandle();

    if (win32Input.connect())
    {
        while (true)
        {
            if (!window.tick())
                break;
            win32Input.poll();

            if (win32Input.get_key_down(flow::input::Keys::ESCAPE))
                break;

            std::cout << std::to_string(win32Input.get_mouse(flow::input::MOUSE_BUTTON_ALL)) << "\n";
        }
    }
}