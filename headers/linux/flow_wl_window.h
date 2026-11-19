#pragma once
#include <flow_wl_fwd.h>

namespace flow::wayland
{
    class WaylandWindow
    {
        wl_surface* m_surface = nullptr;
        xdg_surface* m_xSurface = nullptr;
        xdg_toplevel* m_toplevel = nullptr;

        WaylandContext* m_wlContext = nullptr;

        int32_t m_width;
        int32_t m_height;
        bool fullScreen;
        bool m_resized;

        int32_t m_pendingW;
        int32_t m_pendingH;
        bool m_pendingFullscreen;

        bool shouldRun;

    public:
        WaylandWindow(WaylandContext* wlContext, int width, int height)
            : m_wlContext(wlContext), m_width(width), m_height(height) {}

        bool show();
        bool tick();

        void commit_surface() { wl_surface_commit(m_surface); }
        void attach_surface_buffer(wl_buffer* buffer, int32_t x, int32_t y) { wl_surface_attach(m_surface, buffer, x, y); }

        wl_surface* get_surface() { return m_surface; }
        xdg_surface* get_surface_xdg() { return m_xSurface; }
        xdg_toplevel* get_toplevel() { return m_toplevel; }

        int32_t get_width() { return m_width; }
        int32_t get_height() { return m_height; }

    private:
        void on_toplevel_close(xdg_toplevel*) { shouldRun = false; }
        void on_toplevel_configure(xdg_toplevel*, int32_t w, int32_t h, wl_array* states)
        {
            m_pendingW = w;
            m_pendingH = h;
            m_pendingFullscreen = false;
            auto* begin = static_cast<uint32_t*>(states->data);
            auto* end = begin + states->size / sizeof(uint32_t);
            for (uint32_t* s = begin; s != end; ++s)
            {
                switch (*s)
                {
                    case XDG_TOPLEVEL_STATE_FULLSCREEN:
                        m_pendingFullscreen = true;
                        break;
                    case XDG_TOPLEVEL_STATE_MAXIMIZED:
                        break;
                    case XDG_TOPLEVEL_STATE_ACTIVATED:
                        break;
                    default:
                        break;
                }
            }
        }
        void on_xdg_surface_configure(xdg_surface* xs, uint32_t serial)
        {
            xdg_surface_ack_configure(xs, serial);
            if (m_pendingW > 0 && m_pendingH > 0 && (m_pendingW != m_width || m_pendingH != m_height))
            {
                m_width = m_pendingW;
                m_height = m_pendingH;
                m_resized = true; // renderer checks this, resizes swapchain / wl_egl_window
            }
        }
        static void on_xdg_surface_configure(void* data, xdg_surface* xs, uint32_t serial)
        {
            static_cast<WaylandWindow*>(data)->on_xdg_surface_configure(xs, serial); // must acknowledge before attaching a buffer
        }
        static void on_toplevel_configure(void* data, xdg_toplevel* toplevel, int32_t w, int32_t h, wl_array* array)
        {
            static_cast<WaylandWindow*>(data)->on_toplevel_configure(toplevel, w, h, array);
        }
        static void on_toplevel_close(void* data, xdg_toplevel* toplevel)
        {
            static_cast<WaylandWindow*>(data)->on_toplevel_close(toplevel);
        }
        static void on_toplevel_configure_bounds(void*, xdg_toplevel*, int32_t, int32_t) {}
        static void on_toplevel_wm_capabilities(void*, xdg_toplevel*, wl_array*) {}

        xdg_surface_listener xSurfaceListener = {on_xdg_surface_configure};
        xdg_toplevel_listener toplevelListener = {on_toplevel_configure, on_toplevel_close, on_toplevel_configure_bounds, on_toplevel_wm_capabilities};
    };
}