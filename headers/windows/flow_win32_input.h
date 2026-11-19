#pragma once
#include <flow_input_protocol.h>
#include <flow_input_events.h>
#include <flow_buttons_mouse.h>
#include <flow_keys.h>
#include <vector>
#include <windows.h>

namespace flow::win32
{
    class Win32Input : public input::IInputProtocol
    {
        static Win32Input* m_instance;

    public:
        bool connect() override;

        HWND hwndTarget = nullptr;
        struct ButtonMap
        {
            USHORT down, up;
            uint8_t bit;
        };
        static constexpr ButtonMap kButtons[] = {
            {RI_MOUSE_LEFT_BUTTON_DOWN, RI_MOUSE_LEFT_BUTTON_UP, input::MOUSE_BUTTON_0},
            {RI_MOUSE_MIDDLE_BUTTON_DOWN, RI_MOUSE_MIDDLE_BUTTON_UP, input::MOUSE_BUTTON_1},
            {RI_MOUSE_RIGHT_BUTTON_DOWN, RI_MOUSE_RIGHT_BUTTON_UP, input::MOUSE_BUTTON_2},
            {RI_MOUSE_BUTTON_4_DOWN, RI_MOUSE_BUTTON_4_UP, input::MOUSE_BUTTON_3},
            {RI_MOUSE_BUTTON_5_DOWN, RI_MOUSE_BUTTON_5_UP, input::MOUSE_BUTTON_4},
        };
        static std::array<input::Keys, 256> g_vkToKey;
        static constexpr std::array<input::Keys, 256> make_vk_to_key()
        {
            using K = input::Keys;
            std::array<K, 256> t{}; // all NONE

            // Letters and digits: VK codes equal their ASCII values
            for (int c = 'A'; c <= 'Z'; ++c)
                t[c] = static_cast<K>(c);
            for (int c = '0'; c <= '9'; ++c)
                t[c] = static_cast<K>(c);

            // Function keys (VK_F1..VK_F12 are consecutive)
            for (int i = 0; i < 12; ++i)
                t[VK_F1 + i] = static_cast<K>(static_cast<unsigned char>(K::F1) + i);

            // Numpad digits (consecutive)
            for (int i = 0; i < 10; ++i)
                t[VK_NUMPAD0 + i] = static_cast<K>(static_cast<unsigned char>(K::NP0) + i);

            // Arrows and navigation
            t[VK_UP] = K::UP;
            t[VK_DOWN] = K::DOWN;
            t[VK_LEFT] = K::LEFT;
            t[VK_RIGHT] = K::RIGHT;
            t[VK_INSERT] = K::INS;
            t[VK_DELETE] = K::DEL;
            t[VK_HOME] = K::HOME;
            t[VK_END] = K::END;
            t[VK_PRIOR] = K::PGUP;
            t[VK_NEXT] = K::PGDN; // Win32 names for PageUp/PageDown

            // Modifiers: left, right and generic all collapse to your single entry
            t[VK_SHIFT] = K::L_SHIFT;
            t[VK_LSHIFT] = K::L_SHIFT;
            t[VK_RSHIFT] = K::R_SHIFT;
            t[VK_CONTROL] = K::L_CTRL;
            t[VK_LCONTROL] = K::L_CTRL;
            t[VK_RCONTROL] = K::R_CTRL;
            t[VK_MENU] = K::L_ALT;
            t[VK_LMENU] = K::L_ALT;
            t[VK_RMENU] = K::R_ALT;
            t[VK_LWIN] = K::L_SUPER;
            t[VK_RWIN] = K::R_SUPER;

            // Whitespace and editing
            t[VK_SPACE] = K::SPACE;
            t[VK_TAB] = K::TAB;
            t[VK_BACK] = K::BACK;
            t[VK_ESCAPE] = K::ESCAPE;
            t[VK_RETURN] = K::RETURN;
            t[VK_PAUSE] = K::PAUSE;
            t[VK_SCROLL] = K::SCROLL;
            t[VK_CAPITAL] = K::CAPS_LOCK;

            // Numpad operators
            t[VK_MULTIPLY] = K::NP_MUL;
            t[VK_DIVIDE] = K::NP_DIV;
            t[VK_ADD] = K::NP_ADD;
            t[VK_SUBTRACT] = K::NP_SUB;
            t[VK_DECIMAL] = K::NP_DECIMAL;

            // Punctuation (US layout names; the VK names are layout-neutral)
            t[VK_OEM_PERIOD] = K::PERIOD;
            t[VK_OEM_PLUS] = K::EQUALS;
            t[VK_OEM_COMMA] = K::COMMA;
            t[VK_OEM_MINUS] = K::MINUS;
            t[VK_OEM_1] = K::OEM_1; // ;:
            t[VK_OEM_2] = K::OEM_2; // /?
            t[VK_OEM_3] = K::OEM_3; // `~
            t[VK_OEM_4] = K::OEM_4; // [{
            t[VK_OEM_5] = K::OEM_5; // \|
            t[VK_OEM_6] = K::OEM_6; // ]}
            t[VK_OEM_7] = K::OEM_7; // '"
            t[VK_OEM_8] = K::OEM_8;

            return t;
        }

        void on_raw_input(HRAWINPUT h);

        static Win32Input* get_instance()
        {
            return m_instance;
        }

    protected:
        void poll_platform() override;
    };
}