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
    streams.add_stream("Clock 1", 20, 50);
    streams.add_stream("Clock 2", 20, 50);
    streams.add_stream("Clock 3", 20, 50);
    streams.add_stream("Clock 4", 20, 50);

    size_t counter = 0;
    bool ok = true;
    while (ok) {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X") << ", " << counter;
        //ss << in_time_t;

        //cout << ss.str() << endl;

        streams[string("Clock ") + to_string(counter % 4 + 1)] << ss.str();
        /*
        streams["Clock1"] << "Clock 1";
        streams["Clock2"] << "Clock 2";
        streams["Clock3"] << "Clock 3";
        streams["Clock4"] << "Clock 4";
        */

        counter = (counter + 1) % 1000;

        ok = streams.spin();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}
