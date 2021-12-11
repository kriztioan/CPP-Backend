#include "Point.h"

Point::Point() : _coordinates({3.0, 0.0, 0.0}), _size(1), _symbol(-1) {}

void Point::setCoordinates(double x, double y, double z) {

  _coordinates = {x, y, z};
}