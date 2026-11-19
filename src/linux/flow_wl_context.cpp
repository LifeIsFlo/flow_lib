#include "flow_wl_context.h"
#include <poll.h>

namespace flow::wayland
{
    bool WaylandContext::connect()
    {
        m_display = wl_display_connect(nullptr); // uses $WAYLAND_DISPLAY
        if (!m_display)
        {
            fputs("no Wayland compositor\n", stderr);
            return false;
        }

        m_registry = wl_display_get_registry(m_display);
        wl_registry_add_listener(m_registry, &m_registryListener, this);
        wl_display_roundtrip(m_display); // wait until all globals have been announced

        m_compositor = bind<wl_compositor>(wl_compositor_interface, 4);
        m_shm = bind<wl_shm>(wl_shm_interface, 1);
        m_wmBase = bind<xdg_wm_base>(xdg_wm_base_interface, 1);

        if (!m_compositor || !m_shm || !m_wmBase)
        {
            fputs("missing globals\n", stderr);
            return false;
        }

        xdg_wm_base_add_listener(m_wmBase, &m_wmBaseListener, this);

        return true;
    }

    bool WaylandContext::disconnect()
    {
        wl_display_disconnect(m_display);
        return true;
    }

    void WaylandContext::pump()
    {
        while (wl_display_prepare_read(m_display) != 0)
            wl_display_dispatch_pending(m_display);
        wl_display_flush(m_display);

        pollfd pfd{wl_display_get_fd(m_display), POLLIN, 0};
        if (poll(&pfd, 1, 0) > 0) // timeout 0 = don't wait
            wl_display_read_events(m_display);
        else
            wl_display_cancel_read(m_display);

        wl_display_dispatch_pending(m_display);
    }

    WaylandContext::~WaylandContext()
    {
    }
}