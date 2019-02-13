#include <streamflood/streamflood.h>

#include <locale.h>
#include <term.h>
#include <sys/ioctl.h>
#include <shelf-pack.hpp>
#include <sstream>

namespace streamflood {

using namespace std;

Streams::Streams()
{
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);

    start_color();
    init_pair(1, COLOR_BLACK, COLOR_BLUE);
    init_pair(2, COLOR_BLACK, COLOR_RED);
    init_pair(3, COLOR_BLACK, COLOR_YELLOW);
    init_pair(4, COLOR_BLACK, COLOR_WHITE);
    init_pair(5, COLOR_BLACK, COLOR_GREEN);
    init_pair(6, COLOR_BLACK, COLOR_BLACK);

    screen_width = 0;
    screen_height = 0;

    status_bar = newwin(1, 1, 1, 1);
    wbkgd(status_bar, COLOR_PAIR(5));
    number_display_groups = 0;
    current_display_group = 0;
}

Streams::~Streams()
{
    nodelay(stdscr, FALSE);
    //getch();
    for (pair<const string, Window>& w : windows) {
        delwin(w.second.window);
    }
    delwin(status_bar);
    endwin();
}

pair<size_t, size_t> Streams::compute_string_shape(const string& window_string)
{
    size_t width = window_string.find_first_of('\n');
    //width = 100;

    size_t u = 0;
    string first_row = window_string.substr(0, width);
    const char *c_str = first_row.c_str();
    size_t charCount = 0;
    while (u < width)
    {
        u += mblen(&c_str[u], width - u);
        charCount += 1;
    }
    width = charCount+1;

    size_t height = std::count(window_string.begin(), window_string.end(), '\n');

    //cout << "Height: " << height << ", width: " << width << endl;

    return make_pair(height, width);
}

void Streams::recompute_layout()
{
    vector<string> to_be_placed;
    for (const pair<string, Window>& w : windows) {
        to_be_placed.push_back(w.first);
    }

    uint8_t display_group = 0;
    while (!to_be_placed.empty()) {
        // Initialize the sprite with a width and height..
        
        int start_ind;
        for (start_ind = 0; start_ind < to_be_placed.size()-1; ++start_ind) {
            mapbox::ShelfPack sprite(screen_width, screen_height-1);
            std::vector<mapbox::Bin> bins;
            /*for (const pair<string, Window>& w : windows) {
                bins.emplace_back(-1, w.second.width, w.second.height);
            }*/
            for (int i = start_ind; i < to_be_placed.size(); ++i) {
                const Window& w = windows[to_be_placed[i]];
                bins.emplace_back(-1, w.width, w.height);
            }

            // `pack()` returns a vector of Bin* pointers, with `x`, `y`, `w`, `h` values..
            std::vector<mapbox::Bin*> results = sprite.pack(bins);
            if (results.size() < to_be_placed.size() - start_ind) {
                continue;
            }

            for (int i = start_ind; i < to_be_placed.size(); ++i) {
                Window& w = windows[to_be_placed[i]];
                w.x0 = results[i-start_ind]->x;
                w.y0 = results[i-start_ind]->y;
                w.display_group = display_group;
            }

            display_group += 1;
            break;
        }

        if (start_ind == to_be_placed.size()-1) {
            Window& w = windows[to_be_placed[start_ind]];
            w.x0 = 0;
            w.y0 = 0;
            w.display_group = display_group;
            display_group += 1;
        }

        to_be_placed.resize(start_ind);
    }
    number_display_groups = display_group;
}

void Streams::render_status_bar()
{
    size_t offset = 0;
    std::stringstream ss;
    //ss << "Number display groups: " << int(number_display_groups) << " ";
    wbkgd(status_bar, COLOR_PAIR(5));
    for (int d = 0; d < int(number_display_groups); ++d) {
        ss << " ";
        if (d == current_display_group) {
            ss << "*";
        }
        ss << d << ":";
        //wbkgd(status_bar, COLOR_PAIR(5));
        wattron(status_bar, COLOR_PAIR(5));
        mvwaddstr(status_bar, 0, offset, ss.str().c_str());
        wattroff(status_bar, COLOR_PAIR(5));
        offset += ss.str().size();
        ss.str("");
        int counter = 0;
        for (const pair<string, Window>& w : windows) {
            if (w.second.display_group == d) {
                wattron(status_bar, COLOR_PAIR(5));
                mvwaddstr(status_bar, 0, offset, " ");
                wattroff(status_bar, COLOR_PAIR(5));
                offset += 1;
                wattron(status_bar, COLOR_PAIR(counter%4+1));
                mvwaddstr(status_bar, 0, offset, w.first.c_str());
                wattroff(status_bar, COLOR_PAIR(counter%4+1));
                offset += w.first.size();
            }
            ++counter;
        }
    }
    //wbkgd(status_bar, COLOR_PAIR(5));
    ss << " q: Quit";
    size_t length = ss.str().size();
    for (int i = length; i < screen_width; ++i) {
        ss << " ";
    }
    wattron(status_bar, COLOR_PAIR(5));
    mvwaddstr(status_bar, 0, offset, ss.str().c_str());
    wattroff(status_bar, COLOR_PAIR(5));
}

bool Streams::spin()
{
    struct winsize size;
    if (ioctl(0, TIOCGWINSZ, (char *) &size) < 0) {
        printf("TIOCGWINSZ error");
    }
    bool resized = size.ws_row != screen_height || size.ws_col != screen_width;
    screen_height = size.ws_row;
    screen_width = size.ws_col;

    int c = getch();

    bool switched = false;
    if (c < '0' + number_display_groups && c >= '0') {
        current_display_group = uint8_t(c - '0');
        switched = true;
    }

    if (resized || switched) {
        recompute_layout();
        render(resized);
    }

    return c != 27 && c != 'q' && c != KEY_F(1);
}

void Streams::render(bool resized)
{
    //printf("%d rows, %d columns\n", size.ws_row, size.ws_col);
    if (resized) {
        endwin();
        // Needs to be called after an endwin() so ncurses will initialize
        // itself with the new terminal dimensions.
        clear();
        refresh();

        wresize(status_bar, 1, screen_width);
        mvwin(status_bar, screen_height-1, 0);
    }

    if (current_display_group >= number_display_groups) {
        current_display_group = 0;
    }

    int counter = 0;
    for (pair<const string, Window>& w : windows) {
        if (w.second.window == NULL) {
            w.second.window = newwin(w.second.height, w.second.width, w.second.y0, w.second.x0);
        }
        else {
            wbkgd(w.second.window, COLOR_PAIR(6));
            wrefresh(w.second.window);
            mvwin(w.second.window, w.second.y0, w.second.x0);
            wresize(w.second.window, w.second.height, w.second.width);
            mvwin(w.second.window, w.second.y0, w.second.x0);
        }

        if (w.second.display_group == current_display_group) {
            wbkgd(w.second.window, COLOR_PAIR(counter%4+1));
        }
        else {
            wbkgd(w.second.window, COLOR_PAIR(6));
        }

        if (w.second.display_group == current_display_group) {
            mvwaddstr(w.second.window, 0, 0, w.second.window_string.c_str());
        }

        ++counter;
    }

    render_status_bar();
    
    for (pair<const string, Window>& w : windows) {
        if (w.second.display_group != current_display_group) {
            wrefresh(w.second.window);
        }
    }
    for (pair<const string, Window>& w : windows) {
        if (w.second.display_group == current_display_group) {
            wrefresh(w.second.window);
        }
    }
    wrefresh(status_bar);
    //echo();
    //noecho();
    
}

void Streams::add_stream(const string& stream_name, int height, int width)
{
    if (windows.count(stream_name) != 0) {
        return;
    }

    Window w;
    w.window_string = "";
    w.height = height;
    w.width = width;
    w.window = NULL;
    windows[stream_name] = w;

    recompute_layout();
    render();
}

void Streams::resize_stream(const string& stream_name, int height, int width)
{
    Window& w = windows.at(stream_name);
    if (w.height != height || w.width != width) {
        w.height = height;
        w.width = width;

        recompute_layout();
        render();
    }
}

pair<int, int> Streams::get_stream_shape(const std::string& stream_name)
{
    const Window& w = windows.at(stream_name);
    return make_pair(w.height, w.width);
}

} // namespace streamflood
