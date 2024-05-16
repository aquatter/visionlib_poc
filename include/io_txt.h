#pragma once
#include "point.h"
#include "pointcloud.h"
#include "welib3d_export.h"
#include <string_view>

namespace we {

enum class TxtFileFormat {
    XYZ,
    XYZI,
    XYZIRGB,
    XYZINxNyNz,
    XYZIRGBNxNyNz,
    XYZINxNyNzRGB,
    XYZRGB,
    XYZRGBI,
    XYZRGBINxNyNz,
    XYZRGBNxNyNzI,
    XYZNxNyNz,
    XYZNxNyNzRGBI,
    XYZNxNyNzIRGB
};

enum class TxtSeparator { SPACE, COMMA, SEMICOLON };

WELIB3D_EXPORT bool load_txt(StructuredPointCloud<Point3f> &pcd, size_t width, size_t heigth,
                             Point3f empty_value, TxtFileFormat format, TxtSeparator sep,
                             const std::string_view path);
} // namespace we