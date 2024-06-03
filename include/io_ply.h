#pragma once
#include "point.h"
#include "pointcloud.h"
#include "welib3d_export.h"
#include <istream>
#include <memory>
#include <optional>
#include <ostream>
#include <string_view>

namespace we {

class PlyIO {
  public:
    WELIB3D_EXPORT PlyIO(std::ostream &out_stream);
    WELIB3D_EXPORT PlyIO(std::istream &in_stream);
    WELIB3D_EXPORT ~PlyIO();

    WELIB3D_EXPORT void write_vertices(std::span<const Point3f> vertices);
    WELIB3D_EXPORT void write_normals(std::span<const Point3f> normals);
    WELIB3D_EXPORT void write_faces(std::span<const Point3i> faces);
    WELIB3D_EXPORT void write_intensity(std::span<const uint16_t> intensity);

    [[nodiscard]] WELIB3D_EXPORT std::optional<std::vector<we::Point3f>> read_points();
    [[nodiscard]] WELIB3D_EXPORT std::optional<std::vector<we::Point3f>> read_point_normals();
    [[nodiscard]] WELIB3D_EXPORT std::optional<std::vector<we::Point3i>> read_faces();
    [[nodiscard]] WELIB3D_EXPORT std::optional<std::vector<uint16_t>> read_intensity();

  private:
    struct impl;
    std::unique_ptr<impl> pimpl_;
};

WELIB3D_EXPORT bool save_ply(const we::PointCloud<we::Point3f> &pcd, const std::string_view path);

WELIB3D_EXPORT bool save_ply(const we::PointCloud<we::Point3f> &pcd, std::ostream &out_stream);

WELIB3D_EXPORT we::PointCloud<we::Point3f> load_ply(const std::string_view path);

WELIB3D_EXPORT we::PointCloud<we::Point3f> load_ply(std::istream &in_stream);

} // namespace we