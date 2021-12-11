#ifndef _MINMAX_H_
#define _MINMAX_H_

#include <array>
#include <vector>

#include <utility>

#include <cmath>

class MinMax {

public:
  enum : int { None, Nice, MinExtraRoom, MaxExtraRoom };

  static std::array<double, 2> min_max(const std::vector<double> &values,
                                       int flags = None);

  static std::array<double, 2>
  min_max(const std::vector<std::vector<double>> &values, int flags = None);

  static std::array<double, 2>
  min_max(const std::vector<std::vector<std::pair<double, double>>> &values,
          int flags = None);

private:
  static void nice(std::array<double, 2> &min_max, int flags = None);
};

#endif /* _MINMAX_H_ */