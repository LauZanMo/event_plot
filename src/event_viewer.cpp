#include <algorithm>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "event_viewer.hpp"

using namespace pangolin;
using namespace std;

EventViewer::EventViewer(boost::shared_ptr<EventIO> event_io)
    : event_io_(event_io), first_show_iter_(event_io->event_stream_.begin()) {
    VizInitialize();
}

EventViewer::~EventViewer() {}

// 可视化初始化
void EventViewer::VizInitialize() {
    CreateWindowAndBind(window_name_, window_width_, window_height_);
    glEnable(GL_DEPTH_TEST);
    GetBoundWindow()->RemoveCurrent();
}

// 可视化循环
bool EventViewer::VizLoop() {
    BindToContext(window_name_);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ViewInitialize();

    while (!ShouldQuit()) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 确定需要显示的首尾元素
        last_show_iter_ =
            find_if(first_show_iter_, event_io_->event_stream_.end(), [&](dvs_msgs::Event &event) {
                return event.ts.toSec() - first_show_iter_->ts.toSec() >= time_length_;
            });

        if (image_enable_)
            ViewImage();

        if (pointcloud_enable_)
            ViewPointCloud();

        // 更新首元素
        first_show_iter_ = last_show_iter_;

        boost::this_thread::sleep(boost::posix_time::milliseconds(10));
        FinishFrame();
    }

    GetBoundWindow()->RemoveCurrent();
    DestroyWindow(window_name_);
    return true;
}

void EventViewer::ViewInitialize() {
    // 初始化点云相机
    OpenGlMatrix projection =
        ProjectionMatrix(window_width_, window_height_, 420, 420, 320, 240, 0.01, 1000);
    pointcloud_cam_  = OpenGlRenderState(projection, ModelViewLookAt(0.5, 0.5, 1, 0, 0, 0, AxisZ));
    pointcloud_view_ = Display("view pointcloud")
                           .SetAspect(window_width_ / window_height_)
                           .SetHandler(new Handler3D(pointcloud_cam_));

    // 同时初始化装载图像的容器
    image_view_  = Display("view image").SetAspect(event_io_->img_width_ / event_io_->img_height_);
    img_texture_ = GlTexture(event_io_->img_width_, event_io_->img_height_, GL_RGB, false, 0,
                             GL_BGR, GL_UNSIGNED_BYTE);

    Display("view event")
        .SetBounds(0.0, 1.0, 0.0, 1.0)
        .SetLayout(LayoutEqual)
        .AddDisplay(pointcloud_view_)
        .AddDisplay(image_view_);
}

void EventViewer::ViewPointCloud() {
    pointcloud_view_.Activate(pointcloud_cam_);

    glBegin(GL_POINTS);

    for_each(first_show_iter_, last_show_iter_, [&](dvs_msgs::Event &event) {
        // 着色
        if (event.polarity)
            glColor3f(1.0, 0, 0);
        else
            glColor3f(0, 0, 1.0);
        // 画点
        glVertex3d(event.x * 0.001, event.ts.toSec() - first_show_iter_->ts.toSec(),
                   event.y * 0.001);
    });

    glEnd();
}

void EventViewer::ViewImage() {
    // 转化为OpenCV图像
    cv::Mat event_image = cv::Mat::zeros(event_io_->img_height_, event_io_->img_width_, CV_8UC3);
    for_each(first_show_iter_, last_show_iter_, [&event_image](dvs_msgs::Event &event) {
        cv::Vec<uchar, 3> *data_ptr = event_image.ptr<cv::Vec<uchar, 3>>(event.y);
        if (event.polarity)
            data_ptr[event.x][2] = 255;
        else
            data_ptr[event.x][0] = 255;
    });

    // 上传并画图
    img_texture_.Upload(event_image.data, GL_BGR, GL_UNSIGNED_BYTE);
    image_view_.Activate();
    img_texture_.RenderToViewportFlipY();
}