#include "point.h"
#include "pointcloud.h"
#include "welib3d_export.h"
#include <memory>

namespace we {

#define SET_MAGIC_PROP(name)                                                                       \
    int name##_;                                                                                   \
    MagicFilterSetting &set_##name(const int name) {                                               \
        name##_ = name;                                                                            \
        return *this;                                                                              \
    }

/// @brief Magic filter settings
/// @param width point cloud width
/// @param height point cloud height
/// @param min_area threshold on minimal area of a component
/// @param num_bins number of Z slices for connected components analysis
/// @example
/// MagicFilter{MagicFilterSetting{}.set_width(2448).set_height(2048)}.apply(pcd)
struct MagicFilterSetting {
    MagicFilterSetting()
        : width_{0}
        , height_{0}
        , min_area_{20}
        , num_bins_{100}
        , reduce_{2} {}
    SET_MAGIC_PROP(width)
    SET_MAGIC_PROP(height)
    SET_MAGIC_PROP(min_area)
    SET_MAGIC_PROP(num_bins)
    SET_MAGIC_PROP(reduce)
};

class MagicFilter {
  public:
    WELIB3D_EXPORT explicit MagicFilter(const MagicFilterSetting &set);
    MagicFilter() = delete;
    MagicFilter(const MagicFilter &) = delete;
    MagicFilter(MagicFilter &&) = delete;
    WELIB3D_EXPORT ~MagicFilter();

    WELIB3D_EXPORT void apply(StructuredPointCloud<Point3f> &pcd);

  private:
    struct impl;
    std::unique_ptr<impl> pimpl_;
};

} // namespace we