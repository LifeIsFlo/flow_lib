#pragma once
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <flow_input_protocol.h>
#include <flow_wl_event_bind.h>
#include <flow_wl_fwd.h>

namespace flow::wayland
{
    class WaylandInput : public input::IInputProtocol
    {
        wl_keyboard* m_keyboard = nullptr;
        wl_pointer* m_pointer = nullptr;
        wl_seat* m_seat = nullptr;

        WaylandContext* m_wlContext = nullptr;
        WaylandWindow* m_wlWindow = nullptr;

        static input::MouseButton evdev_to_button(uint32_t code)
        {
            using B = input::MouseButton;
            switch (code)
            {
                case BTN_LEFT:
                    return B::MOUSE_BUTTON_0;
                case BTN_MIDDLE:
                    return B::MOUSE_BUTTON_1;
                case BTN_RIGHT:
                    return B::MOUSE_BUTTON_2;
                case BTN_SIDE:
                    return B::MOUSE_BUTTON_3;
                case BTN_EXTRA:
                    return B::MOUSE_BUTTON_4;
                case BTN_FORWARD:
                    return B::MOUSE_BUTTON_5;
                case BTN_BACK:
                    return B::MOUSE_BUTTON_6;
                case BTN_TASK:
                    return B::MOUSE_BUTTON_7;
                default:
                    return static_cast<B>(0); // unknown button
            }
        }

        static std::array<input::Keys, 256> g_evdevToKeys;
        static std::array<input::Keys, 256> make_evdev_to_keys()
        {
            using K = input::Keys;
            std::array<K, 256> t{}; // all NONE

            // Letters (evdev order follows the physical QWERTY layout)
            t[KEY_Q] = K::Q;
            t[KEY_W] = K::W;
            t[KEY_E] = K::E;
            t[KEY_R] = K::R;
            t[KEY_T] = K::T;
            t[KEY_Y] = K::Y;
            t[KEY_U] = K::U;
            t[KEY_I] = K::I;
            t[KEY_O] = K::O;
            t[KEY_P] = K::P;
            t[KEY_A] = K::A;
            t[KEY_S] = K::S;
            t[KEY_D] = K::D;
            t[KEY_F] = K::F;
            t[KEY_G] = K::G;
            t[KEY_H] = K::H;
            t[KEY_J] = K::J;
            t[KEY_K] = K::K;
            t[KEY_L] = K::L;
            t[KEY_Z] = K::Z;
            t[KEY_X] = K::X;
            t[KEY_C] = K::C;
            t[KEY_V] = K::V;
            t[KEY_B] = K::B;
            t[KEY_N] = K::N;
            t[KEY_M] = K::M;

            // Digits: KEY_1..KEY_9 are consecutive (2..10), KEY_0 comes after them (11)
            for (int i = 0; i < 9; ++i)
                t[KEY_1 + i] = static_cast<K>('1' + i);
            t[KEY_0] = K::K0;

            // Function keys: F1..F10 are consecutive (59..68), F11/F12 are separate (87/88)
            for (int i = 0; i < 10; ++i)
                t[KEY_F1 + i] = static_cast<K>(static_cast<unsigned char>(K::F1) + i);
            t[KEY_F11] = K::F11;
            t[KEY_F12] = K::F12;

            // Arrows and navigation
            t[KEY_UP] = K::UP;
            t[KEY_DOWN] = K::DOWN;
            t[KEY_LEFT] = K::LEFT;
            t[KEY_RIGHT] = K::RIGHT;
            t[KEY_INSERT] = K::INS;
            t[KEY_DELETE] = K::DEL;
            t[KEY_HOME] = K::HOME;
            t[KEY_END] = K::END;
            t[KEY_PAGEUP] = K::PGUP;
            t[KEY_PAGEDOWN] = K::PGDN;

            // Modifiers (already split left/right in evdev)
            t[KEY_LEFTSHIFT] = K::L_SHIFT;
            t[KEY_RIGHTSHIFT] = K::R_SHIFT;
            t[KEY_LEFTCTRL] = K::L_CTRL;
            t[KEY_RIGHTCTRL] = K::R_CTRL;
            t[KEY_LEFTALT] = K::L_ALT;
            t[KEY_RIGHTALT] = K::R_ALT; // AltGr on many layouts
            t[KEY_LEFTMETA] = K::L_SUPER;
            t[KEY_RIGHTMETA] = K::R_SUPER;

            // Whitespace and editing
            t[KEY_SPACE] = K::SPACE;
            t[KEY_TAB] = K::TAB;
            t[KEY_BACKSPACE] = K::BACK;
            t[KEY_ESC] = K::ESCAPE;
            t[KEY_ENTER] = K::RETURN;
            t[KEY_KPENTER] = K::ENTER;
            t[KEY_PAUSE] = K::PAUSE;
            t[KEY_SCROLLLOCK] = K::SCROLL;
            t[KEY_CAPSLOCK] = K::CAPS_LOCK;

            // Numpad
            t[KEY_KP0] = K::NP0;
            t[KEY_KP1] = K::NP1;
            t[KEY_KP2] = K::NP2;
            t[KEY_KP3] = K::NP3;
            t[KEY_KP4] = K::NP4;
            t[KEY_KP5] = K::NP5;
            t[KEY_KP6] = K::NP6;
            t[KEY_KP7] = K::NP7;
            t[KEY_KP8] = K::NP8;
            t[KEY_KP9] = K::NP9;
            t[KEY_KPASTERISK] = K::NP_MUL;
            t[KEY_KPSLASH] = K::NP_DIV;
            t[KEY_KPPLUS] = K::NP_ADD;
            t[KEY_KPMINUS] = K::NP_SUB;
            t[KEY_KPDOT] = K::NP_DECIMAL;

            // Punctuation (same positions as your Win32 OEM mapping, US layout names)
            t[KEY_DOT] = K::PERIOD;
            t[KEY_EQUAL] = K::EQUALS;
            t[KEY_COMMA] = K::COMMA;
            t[KEY_MINUS] = K::MINUS;
            t[KEY_SEMICOLON] = K::OEM_1;  // ;:
            t[KEY_SLASH] = K::OEM_2;      // /?
            t[KEY_GRAVE] = K::OEM_3;      // `~
            t[KEY_LEFTBRACE] = K::OEM_4;  // [{
            t[KEY_BACKSLASH] = K::OEM_5;  // \|
            t[KEY_RIGHTBRACE] = K::OEM_6; // ]}
            t[KEY_APOSTROPHE] = K::OEM_7; // '"
            t[KEY_102ND] = K::OEM_8;      // extra key on ISO keyboards (next to left shift)

            return t;
        }

    public:
        WaylandInput(WaylandContext* wlContext, WaylandWindow* wlWindow) : m_wlContext(wlContext), m_wlWindow(wlWindow)
        {}
        wl_seat* get_seat() { return m_seat; }

    public:
        bool connect() override;

    protected:
        void poll_platform() override;

    private:
        void seat_caps(wl_seat* seat, uint32_t caps)
        {
            if ((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !m_keyboard)
            {
                m_keyboard = wl_seat_get_keyboard(seat);
                wl_keyboard_add_listener(m_keyboard, &m_keyboardListener, this);
            }
            if ((caps & WL_SEAT_CAPABILITY_POINTER) && !m_pointer)
            {
                m_pointer = wl_seat_get_pointer(seat);
                wl_pointer_add_listener(m_pointer, &m_pointerListener, this);
            }
        }
        void kb_key(wl_keyboard*, uint32_t, uint32_t, uint32_t key, uint32_t state)
        {
            bool down = state == WL_KEYBOARD_KEY_STATE_PRESSED; // KEY_* from input-event-codes.h

            const auto k = key < g_evdevToKeys.size() ? g_evdevToKeys[key] : input::Keys::NONE;
            m_pending.push_back({k, down});
        }
        void ptr_motion(wl_pointer*, uint32_t, wl_fixed_t x, wl_fixed_t y)
        {
            m_pendingMouse.clientPos = {(float)wl_fixed_to_double(x), (float)wl_fixed_to_double(y)};
        }
        void ptr_axis(wl_pointer*, uint32_t, uint32_t axis, wl_fixed_t value)
        {
            if (axis == 0)
            {
                m_pendingMouse.scroll = wl_fixed_to_double(value);
            }
        }
        void ptr_button(wl_pointer*, uint32_t, uint32_t, uint32_t button, uint32_t state)
        {
            bool down = state == WL_POINTER_BUTTON_STATE_PRESSED;

            if (down)
            {
                m_pendingMouse.pressed |= evdev_to_button(button);
            }
            else
            {
                m_pendingMouse.released |= evdev_to_button(button);
            }
        }

        wl_seat_listener m_seatListener = {
            .capabilities = WaylandEventBind<&WaylandInput::seat_caps>::call,
            .name = no_op<wl_seat*, const char*>,
        };
        wl_keyboard_listener m_keyboardListener = {
            .keymap = no_op<wl_keyboard*, uint32_t, int32_t, uint32_t>,
            .enter = no_op<wl_keyboard*, uint32_t, wl_surface*, wl_array*>,
            .leave = no_op<wl_keyboard*, uint32_t, wl_surface*>,
            .key = WaylandEventBind<&WaylandInput::kb_key>::call,
            .modifiers = no_op<wl_keyboard*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t>,
            .repeat_info = no_op<wl_keyboard*, int32_t, int32_t>,
        };

        wl_pointer_listener m_pointerListener = {
            .enter = no_op<wl_pointer*, uint32_t, wl_surface*, wl_fixed_t, wl_fixed_t>,
            .leave = no_op<wl_pointer*, uint32_t, wl_surface*>,
            .motion = WaylandEventBind<&WaylandInput::ptr_motion>::call,
            .button = WaylandEventBind<&WaylandInput::ptr_button>::call,
            .axis = WaylandEventBind<&WaylandInput::ptr_axis>::call,
            .frame = no_op<wl_pointer*>,
            .axis_source = no_op<wl_pointer*, uint32_t>,
            .axis_stop = no_op<wl_pointer*, uint32_t, uint32_t>,
            .axis_discrete = no_op<wl_pointer*, uint32_t, int32_t>,
            .axis_value120 = no_op<wl_pointer*, uint32_t, int32_t>,
            .axis_relative_direction = no_op<wl_pointer*, uint32_t, uint32_t>,
            .warp = no_op<wl_pointer*, wl_fixed_t, wl_fixed_t>};
    };
}