#include <vector>

#include <dvs_msgs/Event.h>

class EventIO {
public:
    EventIO(std::string              bag_name,
            std::vector<std::string> topics,
            uint16_t                 img_width,
            uint16_t                 img_height);
    ~EventIO();


    bool Write(std::string output_name, double time_length, std::string type = "pcd");
    bool CheckTimeSequence();

    std::vector<dvs_msgs::Event> event_stream_;

private:
    uint16_t img_width_;
    uint16_t img_height_;

    const int output_width_ = 15;
};
