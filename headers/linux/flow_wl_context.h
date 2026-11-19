#pragma once
#include <flow_wl_fwd.h>
#include <flow_wl_global_info.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace flow::wayland
{
    class WaylandContext
    {
        wl_display* m_display = nullptr;
        wl_registry* m_registry = nullptr;
        wl_compositor* m_compositor = nullptr;
        wl_shm* m_shm = nullptr;
        xdg_wm_base* m_wmBase = nullptr;

        std::unordered_map<std::string, WaylandGlobalInfo> m_globals;

    public:
        bool connect(); // connect, registry, roundtrip, bind, add wmBase ping listener
        bool disconnect();
        void pump(); // your non-blocking prepare_read/read_events/dispatch_pending
        ~WaylandContext();

        wl_display* get_display() { return m_display; }
        wl_registry* get_registry() { return m_registry; }
        wl_compositor* get_compositor() { return m_compositor; }
        wl_shm* get_shm() { return m_shm; }
        xdg_wm_base* get_wm_base() { return m_wmBase; }

        template <typename T>
        T* bind(const wl_interface& iface, uint32_t maxVersion)
        {
            auto it = m_globals.find(iface.name);
            if (it == m_globals.end()) return nullptr;
            return static_cast<T*>(
                wl_registry_bind(m_registry, it->second.name, &iface, std::min(it->second.version, maxVersion)));
        }

    private:
        void on_ping(xdg_wm_base* wm, uint32_t serial) { xdg_wm_base_pong(wm, serial); }
        void on_global_remove(wl_registry*, uint32_t) {}
        void on_global(wl_registry*, uint32_t name, const char* iface, uint32_t version)
        {
            m_globals[iface] = {name, version};
        }
        static void on_ping(void* data, xdg_wm_base* wm, uint32_t serial)
        {
            static_cast<WaylandContext*>(data)->on_ping(wm, serial);
        }
        static void on_global(void* data, wl_registry* reg, uint32_t name, const char* iface, uint32_t version)
        {
            static_cast<WaylandContext*>(data)->on_global(reg, name, iface, version);
        }
        static void on_global_remove(void* data, wl_registry* reg, uint32_t name)
        {
            static_cast<WaylandContext*>(data)->on_global_remove(reg, name);
        }

        xdg_wm_base_listener m_wmBaseListener = {on_ping};
        wl_registry_listener m_registryListener = {on_global, on_global_remove};
    };
}