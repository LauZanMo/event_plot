#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <pangolin/pangolin.h>

#include "event_io.hpp"

class EventViewer {
public:
    EventViewer(boost::shared_ptr<EventIO> event_io);
    ~EventViewer();
    bool VizLoop();

private:
    void VizInitialize();
    void ViewInitialize();

    void ViewPointCloud();
    void ViewImage();

    const std::string window_name_   = "Event Viewer";
    const int         window_width_  = 1280;
    const int         window_height_ = 720;

    boost::shared_ptr<EventIO> event_io_;

    std::vector<dvs_msgs::Event>::iterator first_show_iter_;
    std::vector<dvs_msgs::Event>::iterator last_show_iter_;

    double time_length_       = 0.01;
    bool   pointcloud_enable_ = true;
    bool   image_enable_      = true;

    pangolin::OpenGlRenderState pointcloud_cam_;
    pangolin::View              pointcloud_view_;
    pangolin::View              image_view_;
    pangolin::GlTexture         img_texture_;
};
