# streamflood

Multi virtual terminal streams library for c++

## Functionality

Streamflood allows you to have multiple panes within the terminal running your program.
The panes can have a fixed size or be relative to the window size. Within your program,
you choose to output text streams to any of these panes. The library takes care of
layouting them in a way that the pane will fit on your screen.

In this example, there are four panes, of which three fits in one window:
![Smaller pane 1](https://github.com/nilsbore/streamflood/blob/master/docs/pane_1.png)
By pressing "0", you go the corresponding window, containing the last pane:
![Smaller pane 0](https://github.com/nilsbore/streamflood/blob/master/docs/pane_0.png)

By resizing the window to be bigger, it can display all panes at the same time:
![Larger pane with all](https://github.com/nilsbore/streamflood/blob/master/docs/pane_all.png)

## Library usage

You can check out the [example](https://github.com/nilsbore/streamflood/blob/master/example/clock_streams.cpp)
for info on how to use the code. Briefly, you instantiate a `streamflood::Streams` object,
add a new stream by specifying its name and dimensions, `streams.add_stream("stream0", 50, 200)`
and finally write to the stream `streams["stream0"] << "Writing to stream0!"`.
