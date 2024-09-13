/******************************************************************************
 * Copyright 2022 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "fake_point_generator.h"

#include <cmath>

static const float hs128_elev_angle_map[] = {
    14.436,     13.535,     13.081786,  12.624,     12.165246,  11.702,
    11.238522,  10.771,     10.305007,  9.83,       9.356123,   8.88,
    8.401321,   7.921,      7.43808,    6.952581,   6.466905,   5.977753,
    5.487,      4.995801,   4.501,      4.007293,   3.509,      3.012822,
    2.512,      2.013,      1.885,      1.761,      1.637,      1.511,
    1.385875,   1.2582,     1.13,       1.008459,   0.88,       0.756,
    0.63,       0.505,      0.378591,   0.251,      0.124,      -0.00015,
    -0.129,     -0.2541,    -0.38,      -0.5061259, -0.63235,   -0.7597898,
    -0.8872418, -1.012168,  -1.141,     -1.2662,    -1.393,     -1.519337,
    -1.646275,  -1.773301,  -1.900587,  -2.026912,  -2.155,     -2.2815,
    -2.409,     -2.534932,  -2.662501,  -2.789024,  -2.916055,  -3.043698,
    -3.172,     -3.299,     -3.425,     -3.552222,  -3.680335,  -3.806265,
    -3.932954,  -4.06183,   -4.19,      -4.318,     -4.444,     -4.570508,
    -4.699079,  -4.824327,  -4.950584,  -5.080608,  -5.209,     -5.336,
    -5.463,     -5.589088,  -5.718031,  -5.842508,  -5.968246,  -6.099661,
    -6.607262,  -7.117295,  -7.624327,  -8.133802,  -8.639587,  -9.149,
    -9.652353,  -10.16,     -10.665443, -11.17,     -11.671568, -12.174,
    -12.673194, -13.173,    -13.669682, -14.166,    -14.660411, -15.154,
    -15.644783, -16.135,    -16.622221, -17.106088, -17.592171, -18.071976,
    -18.54765,  -19.029597, -19.50071,  -19.978461, -20.44479,  -20.918108,
    -21.37943,  -21.848107, -22.30422,  -22.768055, -23.21878,  -23.677577,
    -24.12274,  -25.01577};

void FakerPointGenerator::updateSinCosTable()
{
  size_t n = raw_azimuths_.size();
  sin_raw_azimuths_.resize(n);
  cos_raw_azimuths_.resize(n);
  sin_raw_elevations_.resize(n);
  cos_raw_elevations_.resize(n);
  for(size_t i = 0; i < n; i++) {
    sin_raw_azimuths_[i] = sin(raw_azimuths_[i]);
    cos_raw_azimuths_[i] = cos(raw_azimuths_[i]);
    sin_raw_elevations_[i] = sin(raw_elevations_[i]);
    cos_raw_elevations_[i] = cos(raw_elevations_[i]);
  }
}

void FakerPointGenerator::createFlatWallScan(double wall_distance, double hfov, double horizontal_resolution)
{
  if (hfov <= 0 || hfov > 360) {
    return;
  }
  double azimuth, elevation, distance;
  // clear data
  raw_azimuths_.clear();
  raw_elevations_.clear();
  raw_distances_.clear();
  raw_intensities_.clear();
  // creat data
  for(int i = 0; i < 360 / horizontal_resolution; i++) {
    azimuth = (i * horizontal_resolution / 180 * 3.14159);
    // HFOV
    if (azimuth > hfov/2 && azimuth < (360-hfov/2)) {
      continue;
    }
    for(int j = 0; j < 128; j++) {
      elevation = (hs128_elev_angle_map[j] / 180 * 3.14159);
      distance = wall_distance / cos(azimuth) / cos(elevation);
      if (distance > 200) {
        continue;
      }
      raw_azimuths_.push_back(azimuth);
      raw_elevations_.push_back(elevation);
      raw_distances_.push_back(distance);
      raw_intensities_.push_back(rand()%256);
    }
  }
  updateSinCosTable();
}

void FakerPointGenerator::createCurvedWallScan(double wall_distance, double hfov, double horizontal_resolution)
{
  if (hfov <= 0 || hfov > 360) {
    return;
  }
  double azimuth, elevation, distance;
  // clear data
  raw_azimuths_.clear();
  raw_elevations_.clear();
  raw_distances_.clear();
  raw_intensities_.clear();
  // creat data
  for(int i = 0; i < 360 / horizontal_resolution; i++) {
    azimuth = (i * horizontal_resolution / 180 * 3.14159);
    // HFOV
    if (azimuth > hfov/2 && azimuth < (360-hfov/2)) {
      continue;
    }
    for(int j = 0; j < 128; j++) {
      elevation = (hs128_elev_angle_map[j] / 180 * 3.14159);
      distance = wall_distance / cos(elevation);
      if (distance > 200) {
        continue;
      }
      raw_azimuths_.push_back(azimuth);
      raw_elevations_.push_back(elevation);
      raw_distances_.push_back(distance);
      raw_intensities_.push_back(rand()%256);
    }
  }
  updateSinCosTable();
}

void FakerPointGenerator::createFloorScan(double height, double hfov, double horizontal_resolution)
{
  if (hfov <= 0 || hfov > 360) {
    return;
  }
  double azimuth, elevation, distance;
  // clear data
  raw_azimuths_.clear();
  raw_elevations_.clear();
  raw_distances_.clear();
  raw_intensities_.clear();
  // creat data
  for(int i = 0; i < 360 / horizontal_resolution; i++) {
    azimuth = (i * horizontal_resolution / 180 * 3.14159);
    // HFOV
    if (azimuth > hfov/2 && azimuth < (360-hfov/2)) {
      continue;
    }
    for(int j = 0; j < 128; j++) {
      elevation = (hs128_elev_angle_map[j] / 180 * 3.14159);
      if (elevation >= -0.01) {
        continue;
      }
      distance = - height / sin(elevation);
      if (distance > 200) {
        continue;
      }
      raw_azimuths_.push_back(azimuth);
      raw_elevations_.push_back(elevation);
      raw_distances_.push_back(distance);
      raw_intensities_.push_back(rand()%256);
    }
  }
  updateSinCosTable();
}

void FakerPointGenerator::generate(double max_distance_noise, double max_drop_rate)
{
  size_t n = raw_azimuths_.size();
  int drop_rate = max_drop_rate * (0.001*(rand() % 1000));
  for(size_t i = 0; i < n; i++) {
    auto distance = raw_distances_[i] + 0.001*(rand() % 1000) * max_distance_noise;
    auto z = distance * (sin_raw_elevations_[i]);
    auto y = - distance * (cos_raw_elevations_[i]) * (sin_raw_azimuths_[i]);
    auto x = distance * (cos_raw_elevations_[i]) * (cos_raw_azimuths_[i]);
    // calc point
    Point p;
    p.azimuth = raw_azimuths_[i];
    p.elevation = raw_elevations_[i];
    p.distance = distance;
    p.intensity = raw_intensities_[i];
    p.x = x;
    p.y = y;
    p.z = z;
    // add point
    if (0.001*(rand() % 1000) > drop_rate) {
      point_cloud_->points.push_back(p);
    }
  }
  if(cb_) {
    cb_(point_cloud_);
  }
  point_cloud_->points.clear();
}
