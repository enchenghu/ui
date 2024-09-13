/******************************************************************************
 * Copyright 2022 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef FAKER_POINT_GENERATOR_HPP_
#define FAKER_POINT_GENERATOR_HPP_

#include <memory>
#include <vector>
#include <functional>

struct Point
{
  float azimuth;
  float elevation;
  float distance;
  float intensity;
  float x;
  float y;
  float z;
};

struct PointCloud
{
  /* data */
  std::vector<Point> points;
};

class FakerPointGenerator 
{
public:
  using PointCloudPackCallbackType = std::function<void(std::shared_ptr<PointCloud> &)>;
  FakerPointGenerator(){
    point_cloud_ = std::make_shared<PointCloud>();
  };
  ~FakerPointGenerator(){};

  void setPointCloudCallback(PointCloudPackCallbackType cb) {cb_=cb;};
  void createFlatWallScan(double wall_distance, double hfov, double horizontal_resolution = 0.1);
  void createCurvedWallScan(double wall_distance, double hfov, double horizontal_resolution = 0.1);
  void createFloorScan(double height, double hfov, double horizontal_resolution = 0.1);
  void updateSinCosTable();
  void generate(double max_distance_noise, double max_drop_rate = 0.0);

private:
  // 
  std::vector<float> raw_azimuths_;
  std::vector<float> raw_elevations_;
  std::vector<float> raw_distances_;
  std::vector<float> raw_intensities_;
  // sin, cos table
  std::vector<float> sin_raw_azimuths_;
  std::vector<float> cos_raw_azimuths_;
  std::vector<float> sin_raw_elevations_;
  std::vector<float> cos_raw_elevations_;
  //
  PointCloudPackCallbackType cb_;
  std::shared_ptr<PointCloud> point_cloud_;
};

#endif  // FAKER_POINT_GENERATOR_HPP_
