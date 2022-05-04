#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include <dvs_msgs/Event.h>
#include <dvs_msgs/EventArray.h>
#include <rosbag/bag.h>
#include <rosbag/view.h>

using namespace std;

// rosbag信息
string         bag_path = "/home/ubuntu/Dataset/dvs/";
string         bag_name = "dvs_circle.bag";
vector<string> topics   = {"/dvs/events"};

string output_path  = "/home/ubuntu/Code/event_plot/output/";
string output_name  = "events.txt";
int    output_width = 15;

int main(int argc, char *argv[]) {
    rosbag::Bag bag;
    bag.open(bag_path + bag_name);

    vector<dvs_msgs::Event> event_stream;
    rosbag::View            view(bag, rosbag::TopicQuery(topics));
    for (auto message : view) {
        dvs_msgs::EventArray::ConstPtr instance = message.instantiate<dvs_msgs::EventArray>();
        if (instance != nullptr) {
            for (auto event : instance->events)
                event_stream.push_back(event);
        }
    }
    // 测试事件时间顺序
    // for (auto i = event_stream.begin(); i < event_stream.end() - 1; i++) {
    //     if (i->ts.toSec() > (i + 1)->ts.toSec())
    //         cout << "order wrong" << endl;
    // }
    bag.close();

    ofstream output((output_path + output_name).c_str(), ios::trunc);
    output << fixed << setprecision(10);
    for (int i = 0; event_stream[i].ts.toSec() - event_stream.begin()->ts.toSec() <= 5; i++) {
        // clang-format off
        output  << event_stream[i].ts.toSec() << setw(output_width)
                << event_stream[i].x          << setw(output_width)
                << event_stream[i].y          << setw(output_width)
                << to_string(event_stream[i].polarity) << endl;
        // clang-format on
    }

    output.close();

    return 0;
}