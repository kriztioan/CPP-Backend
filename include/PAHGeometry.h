#ifndef _PAHGEOMETRY_H_
#define _PAHGEOMETRY_H_

#include "Atom.h"

#include <algorithm>
#include <array>
#include <functional>
#include <numeric>
#include <vector>

#include <cmath>

class PAHGeometry {

public:
  typedef std::vector<Atom>::iterator iterator;

  PAHGeometry();

  PAHGeometry(const std::vector<Atom> &atoms);

  Atom operator[](std::size_t idx);

  iterator begin() noexcept { return (_atoms.begin()); }

  iterator end() noexcept { return (_atoms.end()); }

  void diagonalize();

  const double &getMass() const;

  std::vector<std::array<double, 3>> const &getDimensions() const;

  std::vector<std::vector<int>> &getBonds();

private:
  std::vector<Atom> _atoms;

  double _mass;

  std::vector<std::array<double, 3>> _dimensions;

  std::vector<std::vector<int>> _bonds;
};

inline Atom PAHGeometry::operator[](std::size_t idx) { return (_atoms[idx]); }

inline const double &PAHGeometry::getMass() const { return (_mass); }

inline std::vector<std::array<double, 3>> const &
PAHGeometry::getDimensions() const {
  return (_dimensions);
}

#endif /* _PAHGEOMETRY_H_ */