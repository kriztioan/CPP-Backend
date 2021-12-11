#include "MinMax.h"

std::array<double, 2> MinMax::min_max(const std::vector<double> &values,
                                      int flags) {

  std::array<double, 2> min_max({0.0, 0.0});

  for (const auto &v : values) {

    if (v < min_max[0]) {

      min_max[0] = v;
    }

    if (v > min_max[1]) {

      min_max[1] = v;
    }
  }

  if (flags != None) {

    MinMax::nice(min_max, flags);
  }

  return (min_max);
}

std::array<double, 2>
MinMax::min_max(const std::vector<std::vector<double>> &values, int flags) {

  std::array<double, 2> min_max({0.0, 0.0});

  for (const auto &vals : values) {

    for (const auto v : vals) {

      if (v < min_max[0]) {

        min_max[0] = v;
      }

      if (v > min_max[1]) {

        min_max[1] = v;
      }
    }
  }

  if (flags != None) {

    MinMax::nice(min_max, flags);
  }

  return (min_max);
}

std::array<double, 2> MinMax::min_max(
    const std::vector<std::vector<std::pair<double, double>>> &values,
    int flags) {

  std::array<double, 2> min_max({0.0, 0.0});

  for (const auto &vals : values) {

    for (const auto &v : vals) {

      if (v.second < min_max[0]) {

        min_max[0] = v.second;
      }

      if (v.second > min_max[1]) {

        min_max[1] = v.second;
      }
    }
  }

  if (flags != None) {

    MinMax::nice(min_max, flags);
  }

  return (min_max);
}

void MinMax::nice(std::array<double, 2> &min_max, int flags) {

  double range = min_max[1] - min_max[0];

  double expt = floor(log10(range));

  double f = range / pow(10.0, expt);

  if (f <= 1.0) {

    f = 1.0;
  } else if (f <= 2.0) {

    f = 2.0;
  } else if (f <= 5.0) {

    f = 5.0;
  } else {

    f = 10.0;
  }

  range = f * pow(10.0, expt);

  double d = range / 4.0;

  expt = floor(log10(d));

  f = d / pow(10.0, expt);

  if (f < 1.5) {

    f = 1.0;
  } else if (f < 3.0) {

    f = 2.0;
  } else if (f < 7.0) {

    f = 5.0;
  } else {

    f = 10.0;
  }

  d = f * pow(10.0, expt);

  min_max[0] = floor(min_max[0] / d) * d;

  if (flags & MinExtraRoom) {

    min_max[0] *= 0.90;
  }

  min_max[1] = ceil(min_max[1] / d) * d;

  if (flags & MaxExtraRoom) {

    min_max[1] = 1.10 * ceil(min_max[1] / d) * d;
  }
}