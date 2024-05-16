#pragma once
#include "point.h"
#include "pointcloud.h"
#include "welib3d_export.h"
#include <memory>
#include <stdint.h>

namespace we {

struct SORFilterSettings {
    size_t image_width_;
    size_t image_height_;
    uint8_t minimum_neighbours_;
    float sigma_multiplier_;
};

class SORFilter {
  public:
    WELIB3D_EXPORT explicit SORFilter(const SORFilterSettings &set);
    WELIB3D_EXPORT void apply(StructuredPointCloud<Point3f> &pcd);
    WELIB3D_EXPORT ~SORFilter();

    SORFilter(const SORFilter &) = delete;
    SORFilter(SORFilter &&) = delete;

    SORFilter &operator=(const SORFilter &) = delete;
    SORFilter &operator=(SORFilter &&) = delete;

  private:
    struct impl;
    std::unique_ptr<impl> pimpl_;
};

} // namespace we