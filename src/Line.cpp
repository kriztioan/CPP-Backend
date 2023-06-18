#include "Line.h"

Line::Line()
    : _startcoordinates({-1.0, -1.0, 0.0}), _endcoordinates({1.0, 1.0, 0.0}) {
  type = Type::I_Line;
}
Line *Line::clone() const { return new Line(*this); }
void Line::setStartCoordinates(double x, double y, double z) {

  _startcoordinates = {x, y, z};
}

void Line::setEndCoordinates(double x, double y, double z) {

  _endcoordinates = {x, y, z};
}