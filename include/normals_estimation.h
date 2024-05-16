#pragma once
#include "point.h"
#include "pointcloud.h"
#include "welib3d_export.h"
#include <memory>
#include <stdint.h>

namespace we {

struct NormalsEstimatorSettings {
    size_t image_width_;
    size_t image_height_;
    uint8_t window_size_;
    float max_angle_;
    bool filter_by_angle_;
};

class NormalsEstimator {
  public:
    WELIB3D_EXPORT explicit NormalsEstimator(const NormalsEstimatorSettings &set);
    WELIB3D_EXPORT void estimate(StructuredPointCloud<Point3f> &pcd);
    WELIB3D_EXPORT ~NormalsEstimator();

  private:
    struct impl;
    std::unique_ptr<impl> pimpl_;
};

} // namespace we