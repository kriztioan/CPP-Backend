#ifndef _LINE_H_
#define _LINE_H_

#include "LineProperties.h"

#include <array>

class Line : public LineProperties {

public:
  Line();

  void setStartCoordinates(const std::array<double, 3> &coordinates);

  void setStartCoordinates(double x, double y, double z);

  std::array<double, 3> const &getStartCoordinates() const;

  void setEndCoordinates(const std::array<double, 3> &coordinates);

  void setEndCoordinates(double x, double y, double z);

  std::array<double, 3> const &getEndCoordinates() const;

private:
  std::array<double, 3> _startcoordinates;

  std::array<double, 3> _endcoordinates;
};

inline void
Line::setStartCoordinates(const std::array<double, 3> &coordinates) {
  _startcoordinates = coordinates;
}

inline std::array<double, 3> const &Line::getStartCoordinates() const {
  return (_startcoordinates);
}

inline void Line::setEndCoordinates(const std::array<double, 3> &coordinates) {
  _endcoordinates = coordinates;
}

inline std::array<double, 3> const &Line::getEndCoordinates() const {
  return (_endcoordinates);
}

#endif /* _LINE_H_ */