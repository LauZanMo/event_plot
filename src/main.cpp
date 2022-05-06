#include <iostream>
#include <vector>

#include "event_io.hpp"

using namespace std;

// rosbag信息
string         bag_path = "/home/ubuntu/Dataset/dvs/";
string         bag_name = "dvs_circle.bag";
vector<string> topics   = {"/dvs/events"};

string output_path = "/home/ubuntu/Code/event_plot/output/";
string output_name = "dvs_circle";

uint16_t img_width  = 640;
uint16_t img_height = 480;

int main(int argc, char *argv[]) {
    EventIO e_io(bag_path + bag_name, topics, img_width, img_height);
    e_io.CheckTimeSequence();
    e_io.Write(output_path + output_name, 5, "pcd");

    return 0;
}