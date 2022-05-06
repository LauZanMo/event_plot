#include <algorithm>

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

    Display("view event")
        .SetBounds(0.0, 1.0, 0.0, 1.0)
        .SetLayout(LayoutEqual)
        .AddDisplay(pointcloud_view_);

    while (!ShouldQuit()) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (pointcloud_enable_)
            ViewPointCloud();

        boost::this_thread::sleep(boost::posix_time::milliseconds(10));
        FinishFrame();
    }

    GetBoundWindow()->RemoveCurrent();
    DestroyWindow(window_name_);
    return true;
}

void EventViewer::ViewInitialize() {
    OpenGlMatrix projection =
        ProjectionMatrix(window_width_, window_height_, 420, 420, 320, 240, 0.01, 1000);
    pointcloud_cam_ = OpenGlRenderState(projection, ModelViewLookAt(0.5, 0.5, 1, 0, 0, 0, AxisZ));

    pointcloud_view_ = Display("view pointcloud")
                           .SetAspect(window_width_ / window_height_)
                           .SetHandler(new Handler3D(pointcloud_cam_));
}

void EventViewer::ViewPointCloud() {
    pointcloud_view_.Activate(pointcloud_cam_);

    // pangolin::glDrawColouredCube();
    glBegin(GL_POINTS);

    first_show_iter_ = find_if(
        first_show_iter_, event_io_->event_stream_.end(), [&](dvs_msgs::Event &event) -> bool {
            if (event.ts.toSec() - first_show_iter_->ts.toSec() <= time_length_) {
                // 着色
                if (event.polarity)
                    glColor3f(1.0, 0, 0);
                else
                    glColor3f(0, 0, 1.0);
                // 画点
                glVertex3d(event.x * 0.001, event.ts.toSec() - first_show_iter_->ts.toSec(),
                           event.y * 0.001);
                return false;
            } else
                return true;
        });

    glEnd();
}