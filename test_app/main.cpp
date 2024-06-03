#include <cstdio>
#include <cstdlib>
#include <exception>
#include <welib3d/hole_filling.h>
#include <welib3d/sensor3d_connector.h>
#include <welib3d/welib3d.h>

int main(int, char **) {

  using namespace we;
  using namespace std::chrono_literals;

  try {
    std::puts("Sensor init");
    Sensor3d sensor{{192, 168, 100, 1}};

    sensor.set<cmd::SENSOR_MODE>(SensorMode::MODE_3D);
    sensor.set<cmd::LED_PATTERN>(PatternType::PATTERN_28);
    sensor.set<cmd::EXPOSURE_TIME_LIMIT>(70ms);
    sensor.set<cmd::CONTRAST_COMPARISON_FILTER>(0.05f);
    sensor.set<cmd::EXPOSURE_TIME>(70ms);
    sensor.set<cmd::LED_POWER>(80);
    sensor.set<cmd::TRIGGER_SOURCE>(TriggerSource::SOFTWRARE);
    sensor.set<cmd::ACQUISITION_START>();
    sensor.set<cmd::SET_TRIGGER_SOFTWARE>();

    std::puts("== Capture point cloud");
    auto pcd{sensor.get_pointcloud()};

    sensor.set<cmd::ACQUISITION_STOP>();

    save_e57(pcd, "point_cloud.e57");

    std::puts("== Magic SOR...");
    MagicSORFilter{MagicSORFilterSettings{.image_width_ = pcd.width(),
                                          .image_height_ = pcd.height(),
                                          .minimal_cluster_size_ = 100,
                                          .sigma_multiplier_ = 0.1f}}
        .apply(pcd);

    save_e57(pcd, "point_cloud_filtered.e57");

    std::puts("== Normals estimation...");
    NormalsEstimator{NormalsEstimatorSettings{.image_width_ = pcd.width(),
                                              .image_height_ = pcd.height(),
                                              .window_size_ = 11,
                                              .max_angle_ = 80.0f,
                                              .filter_by_angle_ = false

                     }}
        .estimate(pcd);

    save_e57(pcd, "point_cloud_filtered_with_normals.e57");

    std::puts("== Filling holes...");

    we::PonintCloudHoleFiller{
        we::PointCloudHoleFillerSettings{
            .image_width_ = sensor.get<cmd::PIXEL_X_MAX>(),
            .image_height_ = sensor.get<cmd::PIXEL_Y_MAX>(),
            .intrinsic_ = sensor.get<cmd::INTRINSIC_MATRIX>(),
            .extrinsic_ = sensor.get<cmd::EXTRINSIC_MATRIX>(),
        }}
        .fill(pcd, 50.0f);

    save_e57(pcd, "point_cloud_holes_filled.e57");

  } catch (const std::exception &ex) {
    std::puts(ex.what());
    return EXIT_FAILURE;
  }

  std::puts("Done");
  return EXIT_SUCCESS;
}
