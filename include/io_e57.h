#pragma once
#include "point.h"
#include "pointcloud.h"
#include "welib3d_export.h"
#include <string_view>

namespace we {
WELIB3D_EXPORT bool load_e57(StructuredPointCloud<Point3f> &pcd, const std::string_view path);

WELIB3D_EXPORT bool save_e57(const StructuredPointCloud<Point3f> &pcd, const std::string_view path);
} // namespace we