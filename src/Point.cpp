#include "Point.h"

Point::Point()
    : _coordinates({0.0, 0.0, 0.0}), _color("000000"), _size(1), _symbol(48) {
  type = Type::I_Point;
}

Point *Point::clone() const { return new Point(*this); }
void Point::setCoordinates(double x, double y, double z) {

  _coordinates = {x, y, z};
}