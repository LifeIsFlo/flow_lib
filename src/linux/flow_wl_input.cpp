#include "flow_wl_input.h"
#include <flow_wl_context.h>

namespace flow::wayland
{
    std::array<input::Keys, 256> WaylandInput::g_evdevToKeys = make_evdev_to_keys();

    bool WaylandInput::connect()
    {
        m_seat = m_wlContext->bind<wl_seat>(wl_seat_interface, 5);
        if (m_seat)
        {
            wl_seat_add_listener(m_seat, &m_seatListener, this);
            return true;
        }
        else
            return false;
    }

    void WaylandInput::poll_platform()
    {
    }
}