#include "flow_wl_window.h"
#include "flow_wl_context.h"

namespace flow::wayland
{
    bool WaylandWindow::show()
    {
        // surface + role = window
        m_surface = wl_compositor_create_surface(m_wlContext->get_compositor());
        m_xSurface = xdg_wm_base_get_xdg_surface(m_wlContext->get_wm_base(), m_surface);
        m_toplevel = xdg_surface_get_toplevel(m_xSurface);
        if (!(m_surface && m_xSurface && m_toplevel)) return false;
        xdg_surface_add_listener(m_xSurface, &xSurfaceListener, this);
        xdg_toplevel_add_listener(m_toplevel, &toplevelListener, this);
        xdg_toplevel_set_title(m_toplevel, "FlowWindow");
        xdg_toplevel_set_app_id(m_toplevel, "flow.window");
        xdg_toplevel_set_min_size(m_toplevel, m_width, m_height);
        xdg_toplevel_set_max_size(m_toplevel, m_width, m_height);

        wl_surface_commit(m_surface);
        wl_display_roundtrip(m_wlContext->get_display());

        shouldRun = true;
        return true;
    }

    bool WaylandWindow::tick() { return shouldRun; }
}