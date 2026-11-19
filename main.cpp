// main.cpp: build with -std=c++20
#include "xdg-shell-client-protocol.h" // generated, see build steps below
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <linux/input-event-codes.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client.h>

struct App
{
    wl_display* display = nullptr;
    wl_registry* registry = nullptr;
    wl_compositor* compositor = nullptr;
    wl_shm* shm = nullptr;
    wl_seat* seat = nullptr;
    xdg_wm_base* wmBase = nullptr;

    wl_surface* surface = nullptr;
    xdg_surface* xSurface = nullptr;
    xdg_toplevel* toplevel = nullptr;

    wl_keyboard* keyboard = nullptr;
    wl_pointer* pointer = nullptr;

    int width = 800, height = 600;
    bool running = true;
};

// ---------- registry: discover what the compositor offers ----------
static void on_global(void* data, wl_registry* reg, uint32_t name, const char* iface, uint32_t version)
{
    auto* app = static_cast<App*>(data);
    if (!strcmp(iface, wl_compositor_interface.name))
        app->compositor = (wl_compositor*)wl_registry_bind(reg, name, &wl_compositor_interface, 4);
    else if (!strcmp(iface, wl_shm_interface.name))
        app->shm = (wl_shm*)wl_registry_bind(reg, name, &wl_shm_interface, 1);
    else if (!strcmp(iface, xdg_wm_base_interface.name))
        app->wmBase = (xdg_wm_base*)wl_registry_bind(reg, name, &xdg_wm_base_interface, 1);
    else if (!strcmp(iface, wl_seat_interface.name))
        app->seat = (wl_seat*)wl_registry_bind(reg, name, &wl_seat_interface, std::min(version, 5u));
}
static void on_global_remove(void*, wl_registry*, uint32_t)
{
}
static const wl_registry_listener registryListener = {on_global, on_global_remove};

// ---------- xdg: the window protocol ----------
static void on_ping(void*, xdg_wm_base* wm, uint32_t serial)
{
    xdg_wm_base_pong(wm, serial);
}
static const xdg_wm_base_listener wmBaseListener = {on_ping};

static void on_xdg_surface_configure(void*, xdg_surface* xs, uint32_t serial)
{
    xdg_surface_ack_configure(xs, serial); // must acknowledge before attaching a buffer
}
static const xdg_surface_listener xSurfaceListener = {on_xdg_surface_configure};

static void on_toplevel_configure(void* data, xdg_toplevel*, int32_t w, int32_t h, wl_array*)
{
    // compositor suggests a size (0 = "your choice"); a real engine would resize its buffers here
    (void)data;
    (void)w;
    (void)h;
}
static void on_toplevel_close(void* data, xdg_toplevel*)
{
    static_cast<App*>(data)->running = false;
}
static const xdg_toplevel_listener toplevelListener = {on_toplevel_configure, on_toplevel_close};

// ---------- keyboard ----------
static void kb_keymap(void*, wl_keyboard*, uint32_t, int32_t fd, uint32_t)
{
    close(fd);
} // xkb keymap, ignored
static void kb_enter(void*, wl_keyboard*, uint32_t, wl_surface*, wl_array*)
{
    puts("[keyboard focus gained]");
}
static void kb_leave(void*, wl_keyboard*, uint32_t, wl_surface*)
{
    puts("[keyboard focus lost]");
}
static void kb_key(void* data, wl_keyboard*, uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
{
    auto* app = static_cast<App*>(data);
    bool down = state == WL_KEYBOARD_KEY_STATE_PRESSED;
    printf("evdev key %u %s\n", key, down ? "down" : "up"); // KEY_* from input-event-codes.h
    if (key == KEY_ESC && down)
        app->running = false;
}

static void kb_modifiers(void*, wl_keyboard*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)
{
}
static void kb_repeat_info(void*, wl_keyboard*, int32_t, int32_t)
{
}

static const wl_keyboard_listener keyboardListener = {
    .keymap = kb_keymap,
    .enter = kb_enter,
    .leave = kb_leave,
    .key = kb_key,
    .modifiers = kb_modifiers,
    .repeat_info = kb_repeat_info,
};

// ---------- pointer ----------
static void ptr_enter(void*, wl_pointer*, uint32_t, wl_surface*, wl_fixed_t x, wl_fixed_t y)
{
    printf("pointer enter at %.1f, %.1f\n", wl_fixed_to_double(x), wl_fixed_to_double(y));
}
static void ptr_leave(void*, wl_pointer*, uint32_t, wl_surface*)
{
    puts("pointer leave");
}
static void ptr_motion(void*, wl_pointer*, uint32_t, wl_fixed_t x, wl_fixed_t y)
{
    printf("pointer %.1f, %.1f\n", wl_fixed_to_double(x), wl_fixed_to_double(y));
}
static void ptr_button(void*, wl_pointer*, uint32_t, uint32_t, uint32_t button, uint32_t state)
{
    printf("button %u %s\n", button, state == WL_POINTER_BUTTON_STATE_PRESSED ? "down" : "up");
}
static void ptr_axis(void*, wl_pointer*, uint32_t, uint32_t axis, wl_fixed_t value)
{
    printf("scroll axis %u value %.2f\n", axis, wl_fixed_to_double(value));
}
static void ptr_frame(void*, wl_pointer*)
{
}
static void ptr_axis_source(void*, wl_pointer*, uint32_t)
{
}
static void ptr_axis_stop(void*, wl_pointer*, uint32_t, uint32_t)
{
}
static void ptr_axis_discrete(void*, wl_pointer*, uint32_t, int32_t)
{
}

static const wl_pointer_listener pointerListener = {
    .enter = ptr_enter,
    .leave = ptr_leave,
    .motion = ptr_motion,
    .button = ptr_button,
    .axis = ptr_axis,
    .frame = ptr_frame,
    .axis_source = ptr_axis_source,
    .axis_stop = ptr_axis_stop,
    .axis_discrete = ptr_axis_discrete,
};

// ---------- seat: which input devices exist (can change at runtime) ----------
static void seat_caps(void* data, wl_seat* seat, uint32_t caps)
{
    auto* app = static_cast<App*>(data);
    if ((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !app->keyboard)
    {
        app->keyboard = wl_seat_get_keyboard(seat);
        wl_keyboard_add_listener(app->keyboard, &keyboardListener, app);
    }
    if ((caps & WL_SEAT_CAPABILITY_POINTER) && !app->pointer)
    {
        app->pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(app->pointer, &pointerListener, app);
    }
}
static void seat_name(void*, wl_seat*, const char*)
{
}
static const wl_seat_listener seatListener = {
    .capabilities = seat_caps,
    .name = seat_name,
};

// ---------- a solid-colour buffer so the window actually appears ----------
static wl_buffer* make_buffer(App& app)
{
    int stride = app.width * 4;
    int size = stride * app.height;
    int fd = memfd_create("flow-buffer", 0);
    ftruncate(fd, size);
    auto* px = (uint32_t*)mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    std::fill(px, px + app.width * app.height, 0xFF202830u); // XRGB8888
    wl_shm_pool* pool = wl_shm_create_pool(app.shm, fd, size);
    wl_buffer* buf = wl_shm_pool_create_buffer(pool, 0, app.width, app.height, stride, WL_SHM_FORMAT_XRGB8888);
    wl_shm_pool_destroy(pool);
    close(fd);
    return buf; // (leaks the mapping; fine for a demo)
}

int main()
{
    App app;
    app.display = wl_display_connect(nullptr); // uses $WAYLAND_DISPLAY
    if (!app.display)
    {
        fputs("no Wayland compositor\n", stderr);
        return 1;
    }

    app.registry = wl_display_get_registry(app.display);
    wl_registry_add_listener(app.registry, &registryListener, &app);
    wl_display_roundtrip(app.display); // wait until all globals have been announced

    if (!app.compositor || !app.shm || !app.wmBase || !app.seat)
    {
        fputs("missing globals\n", stderr);
        return 1;
    }

    xdg_wm_base_add_listener(app.wmBase, &wmBaseListener, &app);
    wl_seat_add_listener(app.seat, &seatListener, &app);

    // surface + role = window
    app.surface = wl_compositor_create_surface(app.compositor);
    app.xSurface = xdg_wm_base_get_xdg_surface(app.wmBase, app.surface);
    xdg_surface_add_listener(app.xSurface, &xSurfaceListener, &app);
    app.toplevel = xdg_surface_get_toplevel(app.xSurface);
    xdg_toplevel_add_listener(app.toplevel, &toplevelListener, &app);
    xdg_toplevel_set_title(app.toplevel, "FlowWindow");
    xdg_toplevel_set_app_id(app.toplevel, "flow.window");

    wl_surface_commit(app.surface);    // initial commit with no buffer asks the compositor to configure us
    wl_display_roundtrip(app.display); // configure arrives, we ack it

    wl_buffer* buf = make_buffer(app);
    wl_surface_attach(app.surface, buf, 0, 0);
    wl_surface_commit(app.surface); // now the window is mapped and can receive focus

    while (app.running && wl_display_dispatch(app.display) != -1)
    {
    }

    wl_display_disconnect(app.display);
}