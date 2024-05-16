#pragma once
#include "point.h"
#include "pointcloud.h"
#include "welib3d_export.h"
#include <memory>
#include <stdint.h>
#include <vcruntime.h>

namespace we {

struct MagicSORFilterSettings {
    size_t image_width_;
    size_t image_height_;
    size_t minimal_cluster_size_;
    float sigma_multiplier_;
    uint8_t minimum_neighbours_;
};

class MagicSORFilter {
  public:
    WELIB3D_EXPORT explicit MagicSORFilter(const MagicSORFilterSettings &set);
    WELIB3D_EXPORT void apply(StructuredPointCloud<Point3f> &pcd);
    WELIB3D_EXPORT ~MagicSORFilter();

    MagicSORFilter(const MagicSORFilter &) = delete;
    MagicSORFilter(MagicSORFilter &&) = delete;

    MagicSORFilter &operator=(const MagicSORFilter &) = delete;
    MagicSORFilter &operator=(MagicSORFilter &&) = delete;

  private:
    struct impl;
    std::unique_ptr<impl> pimpl_;
};

} // namespace we