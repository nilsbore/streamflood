#include <streamflood/streamflood.h>
#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> // put_time
#include <string>  // string
#include <iostream>
#include <thread>

using namespace std;

int main(int argc, char** argv)
{
    streamflood::Streams streams;
    streams.add_stream("Clock 1", 0.5, 0.5);
    streams.add_stream("Clock 2", 0.5, 0.5);
    streams.add_stream("Clock 3", 20, 50);
    streams.add_stream("Clock 4", 20, 50);

    size_t counter = 0;
    while (streams.spin()) {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X") << ", " << counter;
        streams[string("Clock ") + to_string(counter % 4 + 1)] << ss.str();
        counter = (counter + 1) % 1000;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}
