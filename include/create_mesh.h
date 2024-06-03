#pragma once
#include "mesh.h"
#include "pointcloud.h"
#include "welib3d_export.h"
#include <optional>

namespace we {
struct MeshRecSettings {
    float resolution_;
    float samples_per_node_;
    std::optional<float> max_hole_radius_;
};

WELIB3D_EXPORT we::Mesh3f create_mesh(const we::PointCloud3f &pcd, const MeshRecSettings &set);
} // namespace we