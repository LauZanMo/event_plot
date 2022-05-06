#pragma once

#include <cmath>
#include <dvs_msgs/Event.h>
#include <pcl/pcl_macros.h>
#include <pcl/point_types.h>

namespace DVS {
struct EIGEN_ALIGN16 _EventPoint {
    uint16_t x;
    double   y;
    uint16_t z;
    PCL_ADD_RGB;
    // uint8_t  polarity;
    PCL_MAKE_ALIGNED_OPERATOR_NEW
};

struct EventPoint : public _EventPoint {
    inline EventPoint(dvs_msgs::Event &e, uint16_t &img_width, uint16_t &img_height, double time) {
        x = e.x;
        y = e.ts.toSec() - time;
        // 为了和图像一致，Z轴需要翻转
        z = abs((int)e.y - (int)img_height);
        // 红色为正极性，蓝色为负极性
        if (e.polarity) {
            r = a = 255;
            g = b = 0;
        } else {
            b = a = 255;
            r = g = 0;
        }
    }
    PCL_MAKE_ALIGNED_OPERATOR_NEW
};
} // namespace DVS

// 为点云注册事件点
POINT_CLOUD_REGISTER_POINT_STRUCT(DVS::_EventPoint,
                                  // clang-format off
    (std::uint16_t, x, x)
    (double, y, y)
    (std::uint16_t, z, z)
    (float, rgb, rgb)
                                  // clang-format on
)
POINT_CLOUD_REGISTER_POINT_WRAPPER(DVS::EventPoint, DVS::_EventPoint)