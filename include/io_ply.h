#pragma once
#include "point.h"
#include "pointcloud.h"
#include "welib3d_export.h"
#include <istream>
#include <ostream>
#include <string_view>

namespace we {

WELIB3D_EXPORT bool save_ply(const we::PointCloud<we::Point3f> &pcd, const std::string_view path);

WELIB3D_EXPORT bool save_ply(const we::PointCloud<we::Point3f> &pcd, std::ostream &out_stream);

WELIB3D_EXPORT we::PointCloud<we::Point3f> load_ply(const std::string_view path);

WELIB3D_EXPORT we::PointCloud<we::Point3f> load_ply(std::istream &in_stream);

} // namespace we