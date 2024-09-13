/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef COLORMAP_HPP_
#define COLORMAP_HPP_

#include <string>
#include <vector>

namespace autox {
namespace pointview {

class Colormap {
 public:
  Colormap();
  ~Colormap() {}
  bool setType(const std::string& type);
  void setRange(double min, double max);
  void setRangeMin(double min);
  void setRangeMax(double max);

  // v -> [0, 255]
  void get(double v, std::uint8_t& r, std::uint8_t& g, std::uint8_t& b);
  // v -> rgba[255,255,255,255]
  uint32_t get(double v);

 private:
  void initColormap(const std::uint8_t* r, const std::uint8_t* g,
                    const std::uint8_t* b);

  std::vector<std::uint8_t> r_;
  std::vector<std::uint8_t> g_;
  std::vector<std::uint8_t> b_;
  std::vector<std::uint32_t> rgba_;

  double range_min_{0};
  double range_max_{255};
};

}  // namespace pointview
}  // namespace autox

#endif  // COLORMAP_HPP_
