#include <iostream>
#include <vector>

#include "event_io.hpp"
#include "event_viewer.hpp"

using namespace std;

using EventIOPtr = boost::shared_ptr<EventIO>;

// rosbag信息
string         bag_path = "/home/ubuntu/Dataset/dvs/";
string         bag_name = "dvs_circle";
vector<string> topics   = {"/dvs/events"};

string output_path = "/home/ubuntu/Code/event_plot/output/";
string output_name = "dvs_circle";

uint16_t img_width  = 640;
uint16_t img_height = 480;

int main(int argc, char *argv[]) {
    EventIOPtr e_io(new EventIO(bag_path + bag_name, topics, img_width, img_height));
    e_io->CheckTimeSequence();
    e_io->Write(output_path + output_name, 5, "pcd");

    // EventViewer   e_viewer(e_io);
    // boost::thread viewer_thread = boost::thread(&EventViewer::VizLoop, &e_viewer);
    // viewer_thread.join();

    return 0;
}