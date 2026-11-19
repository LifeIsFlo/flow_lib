#pragma once

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <linux/input-event-codes.h>
#include <xdg-shell-client-protocol.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client.h>

namespace flow::wayland
{
    class WaylandContext;
    class WaylandWindow;
    class WaylandInput;
    struct WaylandGlobalInfo;
}