#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>

#include <dvs_msgs/EventArray.h>
#include <rosbag/bag.h>
#include <rosbag/view.h>

#define PCL_NO_PRECOMPILE
#include <pcl/io/pcd_io.h>

#include "event_io.hpp"
#include "event_point_type.hpp"

using namespace std;

EventIO::EventIO(string bag_name, vector<string> topics, uint16_t img_width, uint16_t img_height)
    : img_width_(img_width), img_height_(img_height) {
    // 读取rosbag并保存至容器
    rosbag::Bag bag;
    bag.open(bag_name + ".bag");
    rosbag::View view(bag, rosbag::TopicQuery(topics));

    for (auto message : view) {
        dvs_msgs::EventArray::ConstPtr instance = message.instantiate<dvs_msgs::EventArray>();
        if (instance != nullptr) {
            for (auto event : instance->events)
                event_stream_.push_back(event);
        }
    }
    cout << "Load done." << endl;
    bag.close();
}

EventIO::~EventIO() {}

// 输出事件的文件
bool EventIO::Write(std::string output_name, double time_length, string type) {
    if (type == "pcd") {
        // pcl点云格式(极性被转换成了颜色)
        pcl::PointCloud<DVS::EventPoint> cloud;
        cloud.is_dense = false;

        find_if(event_stream_.begin(), event_stream_.end(), [&](dvs_msgs::Event &event) -> bool {
            if (event.ts.toSec() - event_stream_.begin()->ts.toSec() <= time_length) {
                DVS::EventPoint point(event, img_width_, img_height_,
                                      event_stream_.begin()->ts.toSec());
                cloud.push_back(point);
                return false;
            } else
                return true;
        });
        pcl::io::savePCDFileBinary(output_name + ".pcd", cloud);

    } else if (type == "txt") {
        // txt格式
        ofstream output(output_name + ".txt", ios::trunc);
        output << fixed << setprecision(10);

        find_if(event_stream_.begin(), event_stream_.end(), [&](dvs_msgs::Event &event) -> bool {
            if (event.ts.toSec() - event_stream_.begin()->ts.toSec() <= time_length) {
                // clang-format off
                output  << event.ts.toSec() << setw(output_width_)
                        << event.x          << setw(output_width_)
                        << event.y          << setw(output_width_)
                        << to_string(event.polarity) << endl;
                // clang-format on
                return false;
            } else
                return true;
        });
        output.close();

    } else {
        cout << "Unsupported write type!" << endl;
        return false;
    }
    cout << "Output done." << endl;
    return true;
}

// 测试事件时序
bool EventIO::CheckTimeSequence() {
    for (auto i = event_stream_.begin(); i < event_stream_.end() - 1; i++) {
        if (i->ts.toSec() > (i + 1)->ts.toSec()) {
            cout << "Time sequence wrong at " << i->ts.toSec() << " ." << endl;
            return false;
        }
    }
    return true;
}