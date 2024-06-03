#pragma once
#include "point.h"
#include "pointcloud.h"
#include "welib3d_export.h"
#include <memory>

namespace we {

struct PointCloudHoleFillerSettings {
    int image_width_;
    int image_height_;
    we::Matrix3f intrinsic_;
    we::Matrix4f extrinsic_;
};

class PonintCloudHoleFiller {
  public:
    WELIB3D_EXPORT PonintCloudHoleFiller(const PointCloudHoleFillerSettings &set);
    WELIB3D_EXPORT ~PonintCloudHoleFiller();
    WELIB3D_EXPORT void fill(StructuredPointCloud3f &pcd, const float max_hole_radius);

  private:
    struct impl;
    std::unique_ptr<impl> pimpl_;
};

} // namespace we