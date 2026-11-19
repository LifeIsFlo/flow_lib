#include <flow_util.h>
#include <flow_wl_context.h>
#include <flow_wl_input.h>
#include <flow_wl_window.h>
#include <iostream>

static wl_buffer* make_buffer(flow::wayland::WaylandContext* wlContext, flow::wayland::WaylandWindow* window)
{
    int width = window->get_width();
    int height = window->get_height();

    int stride = width * 4;
    int size = stride * height;
    int fd = memfd_create("flow-buffer", 0);
    ftruncate(fd, size);
    auto* px = (uint32_t*)mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    std::fill(px, px + width * height, 0xFF202830u); // XRGB8888
    wl_shm_pool* pool = wl_shm_create_pool(wlContext->get_shm(), fd, size);
    wl_buffer* buf = wl_shm_pool_create_buffer(pool, 0, width, height, stride, WL_SHM_FORMAT_XRGB8888);
    wl_shm_pool_destroy(pool);
    close(fd);
    return buf; // (leaks the mapping; fine for a demo)
}

int main()
{
    flow::wayland::WaylandContext wlContext{};
    if (!wlContext.connect()) return 1;

    flow::wayland::WaylandWindow window{&wlContext, 800, 600};
    window.show();

    window.attach_surface_buffer(make_buffer(&wlContext, &window), 0, 0);
    window.commit_surface();

    flow::wayland::WaylandInput wlInput{&wlContext, &window};
    wlInput.connect();

    //flow::util::StageTimer mainTimer{"mainTimer"};
    while (window.tick())
    {
        wlContext.pump();
        wlInput.poll();

        std::cout << std::to_string(wlInput.get_mouse(flow::input::MOUSE_BUTTON_ALL)) << "\n";

        if(wlInput.get_key_down(flow::input::Keys::ESCAPE)) break;
    }

    wlContext.disconnect();

    return 0;
}