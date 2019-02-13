#ifndef STREAMFLOOD_H
#define STREAMFLOOD_H

#include <ncurses.h>
#include <map>
#include <utility>
#include <sstream>

namespace streamflood {

class Streams
{
private:

    struct Window {

        size_t hash;
        size_t y0;
        size_t x0;
        size_t height;
        size_t width;
        uint8_t display_group;
        std::string window_string;
        WINDOW* window;

    };

    std::map<std::string, Window> windows;
    size_t screen_width;
    size_t screen_height;
    WINDOW* status_bar;
    uint8_t number_display_groups;
    uint8_t current_display_group;

    void recompute_layout();
    void render(bool resized=false);
    void render_status_bar();

public:

    Streams();

    ~Streams();

    bool spin();
    static std::pair<size_t, size_t> compute_string_shape(const std::string& window_string);

    void add_stream(const std::string& stream_name, int height, int width); // add one of these with double values relative to window

    void resize_stream(const std::string& stream_name, int height, int width); // add one of these with double values relative to window

    std::pair<int, int> get_stream_shape(const std::string& stream_name);

    class WindowStream {

    private:

        Streams& server;
        const std::string& stream_name;

    public:

        template <typename T>
        WindowStream& operator<<(T const& obj)
        {
            std::stringstream ss;
            ss << obj;
            Window& w = server.windows.at(stream_name);
            w.window_string = ss.str();
            if (w.display_group == server.current_display_group) {
                //server.render_status_bar();
                mvwaddstr(w.window, 0, 0, w.window_string.c_str());
                //wrefresh(server.status_bar);
                wrefresh(w.window);
            }
            return *this;
        }

        WindowStream(Streams& server, const std::string& stream_name) : server(server), stream_name(stream_name) {}

    };

    friend class WindowStream;

    WindowStream operator[] (const std::string& stream_name)
    {
        return WindowStream(*this, stream_name);
    }
};

}

#endif // STREAMFLOOD_H
