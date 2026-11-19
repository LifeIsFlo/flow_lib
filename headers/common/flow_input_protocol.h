#pragma once
#include <flow_buttons_mouse.h>
#include <flow_input_events.h>
#include <flow_keys.h>
#include <flow_vector2.h>

namespace flow::input
{
    class IInputProtocol
    {
        std::unordered_set<Keys> m_currKeys;
        std::unordered_set<Keys> m_releasedKeys;
        std::unordered_set<Keys> m_pressedKeys;
        MouseState m_currMouse;

    protected:
        std::vector<KeyEvent> m_pending;
        MouseEvent m_pendingMouse;

    public:
        void poll()
        {
            poll_platform();
            poll_keyboard();
            poll_mouse();
        }

        std::unordered_set<Keys> get_all_keys() const
        {
            return m_currKeys;
        }
        std::unordered_set<Keys> get_all_keys_down() const
        {
            return m_pressedKeys;
        }
        std::unordered_set<Keys> get_all_keys_up() const
        {
            return m_releasedKeys;
        }

        bool get_key(const Keys& k) const
        {
            return m_currKeys.contains(k);
        }
        bool get_key_down(const Keys& k) const
        {
            return m_pressedKeys.contains(k);
        }
        bool get_key_up(const Keys& k) const
        {
            return m_releasedKeys.contains(k);
        }
        uint8_t get_mouse_down(const MouseButton& mouseButton) const
        {
            return m_currMouse.buttonsPressed & mouseButton;
        }
        uint8_t get_mouse(const MouseButton& mouseButton) const
        {
            return m_currMouse.buttonsDown & mouseButton;
        }
        uint8_t get_mouse_up(const MouseButton& mouseButton) const
        {
            return m_currMouse.buttonsReleased & mouseButton;
        }
        float get_mouse_scroll() const
        {
            return m_currMouse.scroll;
        }
        Vector2 get_mouse_delta() const
        {
            return m_currMouse.deltaPos;
        }
        Vector2 get_mouse_pos_client() const
        {
            return m_currMouse.clientPos;
        }
        Vector2 get_mouse_pos_absolute() const
        {
            return m_currMouse.absolutePos;
        }

    private:
        void poll_keyboard()
        {
            m_pressedKeys.clear();
            m_releasedKeys.clear();
            for (const KeyEvent& e : m_pending)
            {
                const input::Keys key = e.key;
                if (key == input::Keys::NONE)
                    continue;

                if (e.down)
                {
                    if (m_currKeys.insert(key).second)
                        m_pressedKeys.insert(key);
                }
                else
                {
                    if (m_currKeys.erase(key) > 0)
                        m_releasedKeys.insert(key);
                }
            }
            m_pending.clear();
        }

        void poll_mouse()
        {
            m_currMouse.buttonsPressed = m_pendingMouse.pressed;
            m_currMouse.buttonsReleased = m_pendingMouse.released;

            m_currMouse.deltaPos = m_pendingMouse.deltaPos;
            m_currMouse.scroll = m_pendingMouse.scroll;

            m_currMouse.buttonsDown |= m_pendingMouse.pressed;
            m_currMouse.buttonsDown &= ~m_pendingMouse.released;

            m_currMouse.absolutePos = m_pendingMouse.absolutePos;
            m_currMouse.clientPos = m_pendingMouse.clientPos;

            m_pendingMouse.nullify_keep_pos();
        }

    public:
        virtual ~IInputProtocol() = default;
        virtual bool connect() = 0;

    protected:
        virtual void poll_platform() = 0;
        IInputProtocol() = default;
    };
}