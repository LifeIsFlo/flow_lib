#pragma once
#include <flow_wl_fwd.h>

namespace flow::wayland
{
    template <auto Member>
    struct WaylandEventBind;

    template <typename Self, typename... Args, void (Self::*Member)(Args...)>
    struct WaylandEventBind<Member>
    {
        static void call(void* data, Args... args)
        {
            (static_cast<Self*>(data)->*Member)(args...);
        }
    };

    template <typename... Args>
    static void no_op(void*, Args...) {}
}